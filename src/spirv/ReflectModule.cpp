/*
 * MIT License - Copyright (c) 2020 Sean Moss
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the
 * 'LICENSE' file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

#include "./ReflectModule.hpp"


// ====================================================================================================================
ReflectModule::ReflectModule(const uint32_t* code, size_t size)
	: module_{ new spv_reflect::ShaderModule(size, code) }
	, error_{ ReflectError::None }
	, stage_{ ShaderStage::Invalid }
{
	// Check the module results
	if (module_->GetResult() != SPV_REFLECT_RESULT_SUCCESS) {
		error_ = ReflectError::InvalidBytecode;
		return;
	}

	// Check the top-level module info
	stage_ = ConvertStage(module_->GetShaderStage());
	if (stage_ == ShaderStage::Invalid) {
		error_ = ReflectError::InvalidStage;
		return;
	}
}

// ====================================================================================================================
ReflectModule::~ReflectModule()
{
	// Destroy module
	if (module_) {
		delete module_;
		module_ = nullptr;
	}
}

// ====================================================================================================================
ShaderStage ReflectModule::ConvertStage(SpvReflectShaderStageFlagBits stage)
{
	switch (stage)
	{
	case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT: return ShaderStage::Vertex;
	case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT: return ShaderStage::TessControl;
	case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT: return ShaderStage::TessEval;
	case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT: return ShaderStage::Geometry;
	case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT: return ShaderStage::Fragment;
	default: return ShaderStage::Invalid;
	}
}
