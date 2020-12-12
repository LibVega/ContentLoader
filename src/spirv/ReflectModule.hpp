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
	InvalidBytecode = 1
}; // enum class ReflectError


// Manages the handle and data for a SPIRV reflection module
class ReflectModule final
{
public:
	ReflectModule(const uint32_t* code, size_t size);
	~ReflectModule();

	// Access
	inline ReflectError error() const { return error_; }
	inline bool hasError() const { return error_ != ReflectError::None; }

private:
	SpvReflectShaderModule module_;
	ReflectError error_;
}; // class ReflectModule
