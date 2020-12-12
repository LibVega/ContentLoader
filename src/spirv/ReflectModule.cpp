/*
 * MIT License - Copyright (c) 2020 Sean Moss
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the
 * 'LICENSE' file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

#include "./ReflectModule.hpp"


// ====================================================================================================================
ReflectModule::ReflectModule(const uint32_t* code, size_t size)
	: module_{ }
	, error_{ ReflectError::None }
{
	// Open the module
	const auto res = spvReflectCreateShaderModule(size, code, &module_);
	if (res != SPV_REFLECT_RESULT_SUCCESS) {
		error_ = ReflectError::InvalidBytecode;
		return;
	}
}

// ====================================================================================================================
ReflectModule::~ReflectModule()
{
	// Destroy module
	if (error_ != ReflectError::InvalidBytecode) {
		spvReflectDestroyShaderModule(&module_);
	}
}
