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
	, descriptors_{ }
	, inputs_{ }
	, outputs_{ }
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
	uint32_t pbCount;
	module_->EnumeratePushConstantBlocks(&pbCount, nullptr);
	if (pbCount > 1) {
		error_ = ReflectError::InvalidPushBlockCount;
		return;
	}

	// Cache enumerate the members
	uint32_t count;
	module_->EnumerateDescriptorBindings(&count, nullptr);
	descriptors_.resize(count);
	module_->EnumerateDescriptorBindings(&count, const_cast<SpvReflectDescriptorBinding**>(descriptors_.data()));
	module_->EnumerateInputVariables(&count, nullptr);
	inputs_.resize(count);
	module_->EnumerateInputVariables(&count, const_cast<SpvReflectInterfaceVariable**>(inputs_.data()));
	module_->EnumerateOutputVariables(&count, nullptr);
	outputs_.resize(count);
	module_->EnumerateOutputVariables(&count, const_cast<SpvReflectInterfaceVariable**>(outputs_.data()));
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
uint32_t ReflectModule::pushSize() const
{
	// Guaranteed to not be >1 from ctor
	uint32_t count;
	SpvReflectBlockVariable* block;
	module_->EnumeratePushConstantBlocks(&count, &block);
	return count ? block->size : 0;
}

// ====================================================================================================================
VegaBool ReflectModule::reflectDescriptor(uint32_t index, DescriptorInfo* info) const
{
	// Validate
	if (index >= descriptors_.size()) {
		error_ = ReflectError::BadMemberIndex;
		return VEGA_FALSE;
	}
	const auto desc = descriptors_[index];

	// Get the descriptor info
	info->name = desc->name;
	info->set = desc->set;
	info->binding = desc->binding;
	info->inputIndex = desc->input_attachment_index;
	info->type = ConvertDescriptorType(desc->descriptor_type);
	if (info->type == DescriptorType::Unknown) {
		error_ = ReflectError::BadDescriptorType;
		return VEGA_FALSE;
	}
	info->size = desc->block.size;
	info->arraySize = GetArraySize(desc->array);
	info->image.type = GetImageType(desc->image);
	if (info->image.type == ImageType::Unknown) {
		error_ = ReflectError::BadImageType;
		return VEGA_FALSE;
	}
	info->image.ms = desc->image.ms ? VEGA_TRUE : VEGA_FALSE;

	return VEGA_TRUE;
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

// ====================================================================================================================
DescriptorType ReflectModule::ConvertDescriptorType(SpvReflectDescriptorType type)
{
	switch (type)
	{
	case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER: return DescriptorType::Sampler;
	case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE: return DescriptorType::Image;
	case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: return DescriptorType::ImageSampler;
	case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return DescriptorType::UniformBuffer;
	case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: return DescriptorType::InputAttachment;
	default: return DescriptorType::Unknown;
	}
}

// ====================================================================================================================
uint32_t ReflectModule::GetArraySize(const SpvReflectBindingArrayTraits& traits)
{
	uint32_t dim = (traits.dims_count > 0 ? 1 : 0);
	for (uint32_t i = 0; i < traits.dims_count; ++i) {
		dim *= traits.dims[i];
	}
	return dim;
}

// ====================================================================================================================
ImageType ReflectModule::GetImageType(const SpvReflectImageTraits& traits)
{
	switch (traits.dim)
	{
	case SpvDim1D: return traits.arrayed ? ImageType::Unknown : ImageType::E1D; // TODO: Array support
	case SpvDim2D: return traits.arrayed ? ImageType::Unknown : ImageType::E2D; // TODO: Array support
	case SpvDim3D: return ImageType::E3D;
	default: return ImageType::Unknown;
	}
}
