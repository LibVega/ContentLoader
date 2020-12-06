/*
 * MIT License - Copyright (c) 2020 Sean Moss
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the
 * 'LICENSE' file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

#pragma once

#include "../config.hpp"

#define	STBI_NO_PSD
#define	STBI_NO_GIF
#define	STBI_NO_HDR
#define	STBI_NO_PIC
#define	STBI_NO_PNM
#define STBI_NO_FAILURE_STRINGS
#include "./stb_image.h"


// Describes the different errors that can occur during image file loading
enum class ImageError : uint32_t
{
	NO_ERROR = 0,			// Special no error state
	FILE_NOT_FOUND = 1,		// The file to open does not exist
	UNKNOWN_TYPE = 2,		// The file is not a known image type
	INVALID_FILE = 3,		// The file failed to open (most likely an invalid header)
	INVALID_CHANNELS = 4,	// The file has an unsupported channel count
	BAD_DATA_READ = 5,		// Reading samples failed (most likely corrupt image data)
	BAD_STATE_READ = 6,		// Attempting to read from a file object that is already errored
}; // enum class ImageError


// Describes the different types of supported image files
enum class ImageType : uint32_t
{
	UNKNOWN = 0,	// Special unknown type signifying an error or uninitialized data
	JPEG = 1,		// JPEG file
	PNG = 2,		// PNG file
	TGA = 3,		// TGA file
	BMP = 4,		// BMP file
}; // enum class ImageType


// Describes the various support component channels
enum class ImageChannels : uint32_t
{
	UNKNOWN = 0,	// Unknown or unsupported channel data
	GRAY = 1,		// Grayscale color data only
	GRAY_ALPHA = 2,	// Grayscale color data with alpha
	RGB = 3,		// RGB color data
	RGBA = 4		// RBG color data with alpha
}; // enum class ImageChannels


// Describes information about an image file
struct ImageInfo final
{
public:
	uint32_t width;
	uint32_t height;
	ImageChannels channels;
}; // struct ImageInfo


// Represents a handle to an image file for providing info and reading
class ImageFile final
{
public:
	explicit ImageFile(const std::string& path);
	~ImageFile();

	inline const std::string& path() const { return path_; }
	inline ImageType type() const { return type_; }
	inline const ImageInfo& info() const { return info_; }
	inline const uint8_t* dataPtr() const { return dataPtr_; }
	inline bool hasData() const { return !!dataPtr_; }
	inline ImageChannels dataChannels() const { return dataChannels_; }
	inline ImageError error() const { return lastError_; }
	inline bool hasError() const { return lastError_ != ImageError::NO_ERROR; }

	bool loadData(const uint8_t** dataptr, ImageChannels channels);

	static ImageType DetectType(const std::string& path);
	static int32_t GetChannelCount(ImageChannels ch);

private:
	const std::string path_;
	const ImageType type_;
	ImageInfo info_;
	uint8_t* dataPtr_;
	ImageChannels dataChannels_;
	ImageError lastError_;
}; // class ImageFile
