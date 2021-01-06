/*
 * MIT License - Copyright (c) 2020-2021 Sean Moss
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the
 * 'LICENSE' file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

#include "./image/ImageFile.hpp"

/// Image API: Open image file
VEGA_API_EXPORT ImageFile* vegaImageOpenFile(const char* const path, ImageError* error)
{
	auto handle = new ImageFile(path);
	*error = handle->error();
	if (handle->hasError()) {
		delete handle;
		return nullptr;
	}
	return handle;
}

/// Image API: Close image file
VEGA_API_EXPORT void vegaImageCloseFile(ImageFile* handle)
{
	if (handle) {
		delete handle;
	}
}

/// Image API: Image file type
VEGA_API_EXPORT ImageType vegaImageGetType(ImageFile* handle)
{
	return handle ? handle->type() : ImageType::UNKNOWN;
}

/// Image API: Image file error
VEGA_API_EXPORT ImageError vegaImageGetError(ImageFile* handle)
{
	return handle ? handle->error() : ImageError::NO_ERROR;
}

/// Image API: Image file size
VEGA_API_EXPORT void vegaImageGetSize(ImageFile* handle, uint32_t* width, uint32_t* height)
{
	*width = handle ? handle->info().width : 0;
	*height = handle ? handle->info().height : 0;
}

/// Image API: Image file channels
VEGA_API_EXPORT void vegaImageGetChannels(ImageFile* handle, ImageChannels* channels)
{
	*channels = handle ? handle->info().channels : ImageChannels::UNKNOWN;
}

/// Image API: Image get loaded data
VEGA_API_EXPORT void vegaImageGetLoadedData(ImageFile* handle, const uint8_t** data, ImageChannels* channels)
{
	*data = handle ? handle->dataPtr() : nullptr;
	*channels = handle ? handle->dataChannels() : ImageChannels::UNKNOWN;
}

/// Image API: Image load new data
VEGA_API_EXPORT uint32_t vegaImageLoadData(ImageFile* handle, const uint8_t** data, ImageChannels channels)
{
	if (!handle || !handle->loadData(data, channels)) {
		*data = nullptr;
		return 0;
	}
	return 1;
}