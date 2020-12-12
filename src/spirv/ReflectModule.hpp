/*
 * MIT License - Copyright (c) 2020 Sean Moss
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the
 * 'LICENSE' file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

#pragma once

#include "../config.hpp"

#include "./spirv_reflect.h"


// Reflection error codes
enum class ReflectError : uint32_t
{
	None = 0,
	NullModule = 1, // Special public API error for passing a null module
	InvalidBytecode = 2,
	InvalidStage = 3,
}; // enum class ReflectError


// Reflection shader stages
enum class ShaderStage : uint32_t
{
	Invalid = 0,
	Vertex = 1,
	TessControl = 2,
	TessEval = 3,
	Geometry = 4,
	Fragment = 5
}; // enum class ShaderStage


// Manages the handle and data for a SPIRV reflection module
class ReflectModule final
{
public:
	ReflectModule(const uint32_t* code, size_t size);
	~ReflectModule();

	// Info
	inline ReflectError error() const { return error_; }
	inline bool hasError() const { return error_ != ReflectError::None; }

	// Shader reflect
	inline ShaderStage stage() const { return stage_; }
	inline const char* entryPoint() const { return module_->GetEntryPointName(); }

private:
	// Convert the library stage to the public API stage
	static ShaderStage ConvertStage(SpvReflectShaderStageFlagBits stage);

private:
	spv_reflect::ShaderModule* module_;
	ReflectError error_;
	ShaderStage stage_;
}; // class ReflectModule
