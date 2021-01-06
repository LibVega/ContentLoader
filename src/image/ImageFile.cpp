/*
 * MIT License - Copyright (c) 2020-2021 Sean Moss
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the
 * 'LICENSE' file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

#define STB_IMAGE_IMPLEMENTATION
#include "./ImageFile.hpp"

#include <algorithm>
#include <fstream>


// ====================================================================================================================
ImageFile::ImageFile(const std::string& path)
	: path_{ path }
	, type_{ DetectType(path) }
	, info_{ }
	, dataPtr_{ nullptr }
	, dataChannels_{ ImageChannels::UNKNOWN }
	, lastError_{ ImageError::NO_ERROR }
{
	// Unknown type cut out early
	if (type_ == ImageType::UNKNOWN) {
		lastError_ = ImageError::UNKNOWN_TYPE;
		return;
	}

	// Try to open file
	{
		std::ifstream file(path);
		if (!file.is_open()) {
			lastError_ = ImageError::FILE_NOT_FOUND;
			return;
		}
	}

	// Load the file information
	int x, y, channels;
	if (!stbi_info(path.c_str(), &x, &y, &channels)) {
		lastError_ = ImageError::INVALID_FILE;
		return;
	}
	info_.width = uint32_t(x);
	info_.height = uint32_t(y);
	switch (channels) {
		case 1: info_.channels = ImageChannels::GRAY; break;
		case 2: info_.channels = ImageChannels::GRAY_ALPHA; break;
		case 3: info_.channels = ImageChannels::RGB; break;
		case 4: info_.channels = ImageChannels::RGBA; break;
		default: lastError_ = ImageError::INVALID_CHANNELS; break;
	}
}

// ====================================================================================================================
ImageFile::~ImageFile()
{
	if (dataPtr_) {
		stbi_image_free(dataPtr_);
		dataPtr_ = nullptr;
	}
}

// ====================================================================================================================
bool ImageFile::loadData(const uint8_t** dataptr, ImageChannels channels)
{
	// Check for error state
	if (hasError()) {
		lastError_ = ImageError::BAD_STATE_READ;
		*dataptr = nullptr;
		return false;
	}

	// Check already loaded
	if (dataPtr_ && (channels == dataChannels_)) {
		*dataptr = dataPtr_;
		return true;
	}

	// Free old data
	if (dataPtr_) {
		stbi_image_free(dataPtr_);
		dataPtr_ = nullptr;
		dataChannels_ = ImageChannels::UNKNOWN;
	}

	// Get new data
	int x, y, c;
	auto data = stbi_load(path_.c_str(), &x, &y, &c, GetChannelCount(channels));
	if (!data || (x != info_.width) || (y != info_.height)) {
		if (data) {
			stbi_image_free(data);
		}
		lastError_ = ImageError::BAD_DATA_READ;
		*dataptr = nullptr;
		return false;
	}

	// Set values and return
	dataPtr_ = data;
	dataChannels_ = channels;
	lastError_ = ImageError::NO_ERROR;
	*dataptr = data;
	return true;
}

// ====================================================================================================================
ImageType ImageFile::DetectType(const std::string& path)
{
	const auto extPos = path.find_last_of('.');
	if (extPos == std::string::npos) {
		return ImageType::UNKNOWN;
	}
	auto ext = path.substr(extPos);
	std::transform(ext.begin(), ext.end(), ext.begin(), std::tolower);

	if (ext == ".jpg" || ext == ".jpeg") {
		return ImageType::JPEG;
	}
	else if (ext == ".png") {
		return ImageType::PNG;
	}
	else if (ext == ".tga") {
		return ImageType::TGA;
	}
	else if (ext == ".bmp") {
		return ImageType::BMP;
	}
	else {
		return ImageType::UNKNOWN;
	}
}

// ====================================================================================================================
int32_t ImageFile::GetChannelCount(ImageChannels ch)
{
	switch (ch) {
		case ImageChannels::GRAY: return 1;
		case ImageChannels::GRAY_ALPHA: return 2;
		case ImageChannels::RGB: return 3;
		case ImageChannels::RGBA: return 4;
		default: return 0;
	}
}
