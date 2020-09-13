/*
 * MIT License - Copyright (c) 2020 Sean Moss
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the
 * 'LICENSE' file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

#define DR_WAV_IMPLEMENTATION
#define DR_FLAC_IMPLEMENTATION
#include "./AudioFile.hpp"

#include <fstream>
#include <algorithm>


// ====================================================================================================================
AudioFile::AudioFile(const std::string& path)
	: path_{ path }
	, type_{ DetectType(path) }
	, handle_{ nullptr }
	, info_{ }
	, lastError_{ AudioError::NO_ERROR }
{
	// Unknown type cut out early
	if (type_ == AudioType::UNKNOWN) {
		lastError_ = AudioError::UNKNOWN_TYPE;
		return;
	}

	// Try to open file
	{
		std::ifstream file(path);
		if (!file.is_open()) {
			lastError_ = AudioError::FILE_NOT_FOUND;
			return;
		}
	}

	// Initialize the file handle
	if (type_ == AudioType::WAV) {
		handle_.wav = new drwav;
		if (!drwav_init_file(handle_.wav, path.c_str(), nullptr)) {
			delete handle_.wav;
			handle_.wav = nullptr;
			lastError_ = AudioError::INVALID_FILE;
		}
	}
	else if (type_ == AudioType::VORBIS) {
		int err;
		handle_.vorbis = stb_vorbis_open_filename(path.c_str(), &err, nullptr);
		if (!handle_.vorbis) {
			lastError_ = AudioError::INVALID_FILE;
		}
	}
	else if (type_ == AudioType::FLAC) {
		handle_.flac = drflac_open_file(path.c_str(), nullptr);
		if (!handle_.flac) {
			lastError_ = AudioError::INVALID_FILE;
		}
	}
	if (lastError_ != AudioError::NO_ERROR) {
		return;
	}

	// Load file information
	if (type_ == AudioType::WAV) {
		info_.totalFrames = handle_.wav->totalPCMFrameCount;
		info_.sampleRate = handle_.wav->sampleRate;
		info_.channels = handle_.wav->channels;
	}
	else if (type_ == AudioType::VORBIS) {
		const auto info = stb_vorbis_get_info(handle_.vorbis);
		const auto count = stb_vorbis_stream_length_in_samples(handle_.vorbis);
		info_.totalFrames = count;
		info_.sampleRate = info.sample_rate;
		info_.channels = info.channels;
	}
	else {
		info_.totalFrames = handle_.flac->totalPCMFrameCount;
		info_.sampleRate = handle_.flac->sampleRate;
		info_.channels = handle_.flac->channels;
	}
	remaining_ = info_.totalFrames;
}

// ====================================================================================================================
AudioFile::~AudioFile()
{
	if (type_ == AudioType::WAV && handle_.wav) {
		drwav_uninit(handle_.wav);
		delete handle_.wav;
	}
	else if (type_ == AudioType::VORBIS && handle_.vorbis) {
		stb_vorbis_close(handle_.vorbis);
	}
	else if (type_ == AudioType::FLAC && handle_.flac) {
		drflac_close(handle_.flac);
	}
}

// ====================================================================================================================
uint64_t AudioFile::readFrames(uint64_t frameCount, int16_t* buffer)
{
	// Check error states
	if (lastError_ != AudioError::NO_ERROR) {
		lastError_ = AudioError::BAD_STATE_READ;
		return 0;
	}
	if (remaining_ == 0) {
		lastError_ = AudioError::READ_AT_END;
		return 0;
	}

	// Get actual read size
	const uint64_t fCount = std::min(frameCount, remaining_);
	const uint64_t sCount = fCount * info_.channels;

	// Dispatch read command
	uint64_t actual = 0;
	if (type_ == AudioType::WAV) {
		actual = drwav_read_pcm_frames_s16(handle_.wav, fCount, buffer);
	}
	else if (type_ == AudioType::VORBIS) {
		actual = stb_vorbis_get_samples_short_interleaved(handle_.vorbis, int(info_.channels), buffer, int(sCount));
	}
	else {
		actual = drflac_read_pcm_frames_s16(handle_.flac, fCount, buffer);
	}

	// Report
	if (actual != fCount) {
		lastError_ = AudioError::BAD_DATA_READ;
		return 0;
	}
	else {
		lastError_ = AudioError::NO_ERROR;
		remaining_ -= actual;
		return uint64_t(actual);
	}
}

// ====================================================================================================================
AudioType AudioFile::DetectType(const std::string& path)
{
	const auto extPos = path.find_last_of('.');
	if (extPos == std::string::npos) {
		return AudioType::UNKNOWN;
	}
	auto ext = path.substr(extPos);
	std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
	
	if (ext == ".wav" || ext == ".wave") {
		return AudioType::WAV;
	}
	else if (ext == ".ogg") {
		return AudioType::VORBIS;
	}
	else if (ext == ".flac") {
		return AudioType::FLAC;
	}
	else {
		return AudioType::UNKNOWN;
	}
}
