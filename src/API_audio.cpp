/*
 * MIT License - Copyright (c) 2020-2021 Sean Moss
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the
 * 'LICENSE' file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

#include "./audio/AudioFile.hpp"

/// Audio API: Open sound file
VEGA_API_EXPORT AudioFile* vegaAudioOpenFile(const char* const path, AudioError* error)
{
	auto handle = new AudioFile(path);
	*error = handle->error();
	if (handle->hasError()) {
		delete handle;
		return nullptr;
	}
	return handle;
}

/// Audio API: Close sound file
VEGA_API_EXPORT void vegaAudioCloseFile(AudioFile* handle)
{
	if (handle) {
		delete handle;
	}
}

/// Audio API: Sound file type
VEGA_API_EXPORT AudioType vegaAudioGetType(AudioFile* handle)
{
	return handle ? handle->type() : AudioType::UNKNOWN;
}

/// Audio API: Sound file error
VEGA_API_EXPORT AudioError vegaAudioGetError(AudioFile* handle)
{
	return handle ? handle->error() : AudioError::NO_ERROR;
}

/// Audio API: Sound get frames
VEGA_API_EXPORT uint64_t vegaAudioGetFrameCount(AudioFile* handle)
{
	return handle ? handle->info().totalFrames : 0;
}

/// Audio API: Sound get sample rate
VEGA_API_EXPORT uint32_t vegaAudioGetSampleRate(AudioFile* handle)
{
	return handle ? handle->info().sampleRate : 0;
}

/// Audio API: Sound get channels
VEGA_API_EXPORT uint32_t vegaAudioGetChannelCount(AudioFile* handle)
{
	return handle ? handle->info().channels : 0;
}

/// Audio API: Sound get all info
VEGA_API_EXPORT void vegaAudioGetInfo(AudioFile* handle, uint64_t* frames, uint32_t* rate, uint32_t* channels)
{
	*frames = handle ? handle->info().totalFrames : 0;
	*rate = handle ? handle->info().sampleRate : 0;
	*channels = handle ? handle->info().channels : 0;
}

/// Audio API: Sound get remaining samples
VEGA_API_EXPORT uint64_t vegaAudioGetRemainingFrames(AudioFile* handle)
{
	return handle ? handle->remaining() : 0;
}

/// Audio API: Read frames
VEGA_API_EXPORT uint64_t vegaAudioReadFrames(AudioFile* handle, uint64_t frameCount, int16_t* buffer)
{
	return handle ? handle->readFrames(frameCount, buffer) : 0;
}
