/*
 * MIT License - Copyright (c) 2020 Sean Moss
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the
 * 'LICENSE' file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

#pragma once

#include "../config.hpp"

#define STB_VORBIS_HEADER_ONLY
#include "./dr_wav.h"
#include "./stb_vorbis.c"
#include "./dr_flac.h"


// Describes the different errors that can occur during audio file loading
enum class AudioError : uint32_t
{
	NO_ERROR = 0,			// Special no error state
	FILE_NOT_FOUND = 1,		// The file to open does not exist
	UNKNOWN_TYPE = 2,		// The file is not a known audio type
	INVALID_FILE = 3,		// The file failed to open (most likely an invalid header)
	BAD_DATA_READ = 4,		// Reading samples failed (most likely corrupt frame data)
	READ_AT_END = 5,		// Attempting to read a fully consumed file
	BAD_STATE_READ = 6,		// Attempting to read from a file object that is already errored
}; // enum class AudioError


// Describes the different types of supported audio files
enum class AudioType : uint32_t
{
	UNKNOWN = 0,	// Special unknown type signifying and error or uninitialized data
	WAV = 1,		// WAVE file
	VORBIS = 2,		// OGG/Vorbis file
	FLAC = 3,		// Flac file
}; // enum class AudioType


// Contains metadata information about an audio file
struct AudioInfo final
{
public:
	uint64_t totalFrames;
	uint32_t sampleRate;
	uint32_t channels;
}; // struct AudioInfo


// Represents a handle to a sound file for reading and streaming
// This type is the opaque pointer type used in the exported C# API
class AudioFile final
{
public:
	explicit AudioFile(const std::string& path);
	~AudioFile();

	inline const std::string& path() const { return path_; }
	inline AudioType type() const { return type_; }
	inline const AudioInfo& info() const { return info_; }
	inline uint64_t remaining() const { return remaining_; }
	inline AudioError error() const { return lastError_; }
	inline bool hasError() const { return lastError_ != AudioError::NO_ERROR; }

	// Returns the actual number of frames read, or 0 for an error
	uint64_t readFrames(uint64_t frameCount, int16_t* buffer);

	static AudioType DetectType(const std::string& path);
	
private:
	const std::string path_;
	const AudioType type_;
	union
	{
		drwav* wav;
		stb_vorbis* vorbis;
		drflac* flac;
	} handle_;
	AudioInfo info_;
	uint64_t remaining_;
	AudioError lastError_;
}; // class AudioFile
