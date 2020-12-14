/*
 * MIT License - Copyright (c) 2020 Sean Moss
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the
 * 'LICENSE' file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

#include "./ReflectModule.hpp"


// ====================================================================================================================
ReflectModule::ReflectModule(const uint32_t* code, size_t size)
	: error_{ ReflectError::None }
	, stage_{ ReflectStage::Invalid }
	, entryPoint_{ "" }
	, pushSize_{ 0 }
{
	// Create the module and check the results
	const spv_reflect::ShaderModule mod{ size, code };
	if (mod.GetResult() != SPV_REFLECT_RESULT_SUCCESS) {
		error_ = ReflectError::InvalidBytecode;
		return;
	}
}

// ====================================================================================================================
ReflectModule::~ReflectModule()
{

}
