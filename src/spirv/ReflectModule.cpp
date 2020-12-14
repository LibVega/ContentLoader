/*
 * MIT License - Copyright (c) 2020 Sean Moss
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the
 * 'LICENSE' file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

#include "./ReflectModule.hpp"

#define SET_BIT(name,idx) ((name)|(1<<(idx)))
#define CLR_BIT(name,idx) ((name)&(~(1<<(idx))))


// ====================================================================================================================
ReflectModule::ReflectModule(const uint32_t* code, size_t size)
	: error_{ ReflectError::None }
	, stage_{ ReflectStage::Invalid }
	, entryPoint_{ "" }
	, pushSize_{ 0 }
	, bindings_{ }
	, names_{ }
{
	// Create the module and check the results
	const spv_reflect::ShaderModule mod{ size, code };
	if (mod.GetResult() != SPV_REFLECT_RESULT_SUCCESS) {
		error_ = ReflectError::InvalidBytecode;
		return;
	}

	// Top-level info reflection
	stage_ = ConvertStage(mod.GetShaderStage());
	if (stage_ == ReflectStage::Invalid) {
		error_ = ReflectError::InvalidStage;
		return;
	}
	if (mod.GetEntryPointCount() != 1) {
		error_ = ReflectError::MultipleEntryPoints;
		return;
	}
	entryPoint_ = { mod.GetEntryPointName() };
	uint32_t pbCount;
	mod.EnumeratePushConstantBlocks(&pbCount, nullptr);
	if (pbCount > 1) {
		error_ = ReflectError::MultiplePushBlocks;
		return;
	}
	else if (pbCount == 1) {
		const auto pb = mod.GetPushConstantBlock(0);
		pushSize_ = pb->size;
	}

	// Reflect the bindings
	uint32_t count;
	mod.EnumerateDescriptorBindings(&count, nullptr);
	std::vector<SpvReflectDescriptorBinding*> descs; descs.resize(count);
	mod.EnumerateDescriptorBindings(&count, descs.data());
	for (const auto& desc : descs) {
		// Validate set and binding
		const uint32_t set = desc->set, slot = desc->binding;
		if (set >= VEGA_MAX_SET_COUNT) {
			error_ = ReflectError::BindingSetOutOfRange;
			bindingError_ = { set, slot };
			return;
		}
		if (slot >= VEGA_MAX_PER_SET_SLOTS) {
			error_ = ReflectError::BindingSlotOutOfRange;
			bindingError_ = { set, slot };
			return;
		}

		// Try to parse the binding
		error_ = ParseBinding(*desc, names_[set][slot], bindings_[set][slot]);
		if (error_ != ReflectError::None) {
			bindingError_ = { set, slot };
			return;
		}
	}
}

// ====================================================================================================================
ReflectModule::~ReflectModule()
{

}

// ====================================================================================================================
uint32_t ReflectModule::getSetMask(BindingSet set) const
{
	// Validate set index
	const uint32_t sidx = uint32_t(set);
	if (sidx >= VEGA_MAX_SET_COUNT) {
		return 0;
	}

	// Populate the mask
	const auto& setref = bindings_[sidx];
	uint32_t mask = 0;
	for (uint32_t i = 0; i < VEGA_MAX_PER_SET_SLOTS; ++i) {
		if (setref[i].name) {
			mask = SET_BIT(mask, i);
		}
		else {
			mask = CLR_BIT(mask, i);
		}
	}
	return mask;
}

// ====================================================================================================================
const BindingInfo* ReflectModule::getBinding(BindingSet set, uint32_t slot) const
{
	// Validate indices
	const uint32_t sidx = uint32_t(set);
	if (sidx >= VEGA_MAX_SET_COUNT) {
		return nullptr;
	}
	if (slot >= VEGA_MAX_PER_SET_SLOTS) {
		return nullptr;
	}

	// Get the info (return nullptr if not populated)
	const auto& bind = bindings_[sidx][slot];
	return bind.name ? &bind : nullptr;
}

// ====================================================================================================================
ReflectError ReflectModule::ParseBinding(const SpvReflectDescriptorBinding& desc, std::string& name, BindingInfo& bind)
{
	// Get general info
	name = { desc.name };
	bind.name = name.c_str();
	bind.set = BindingSet(desc.set); // Already checked before function call
	bind.slot = desc.binding; // Already checked before function call

	// Get and check the type and array size
	bind.type = ConvertBindingType(desc.descriptor_type);
	if (bind.type == BindingType::Unknown) {
		return ReflectError::UnsupportedBindingType;
	}
	if (!ValidateBindingSet(bind.type, bind.set)) {
		return ReflectError::InvalidBindingType;
	}
	const bool isBlock = (bind.type == BindingType::UniformBuffer) || (bind.type == BindingType::StorageBuffer);
	bind.arraySize = GetBindingArraySize(desc.array);

	// Get the block size
	if (isBlock) {
		bind.blockSize = desc.block.size;
	}

	// Get and check the image dims
	if (!isBlock && (bind.type != BindingType::Sampler)) {
		bind.imageDims = GetImageDims(desc.image);
		if (bind.imageDims == ImageDims::Unknown) {
			return ReflectError::InvalidImageDims;
		}
	}

	// Successful parse
	return ReflectError::None;
}

// ====================================================================================================================
ReflectStage ReflectModule::ConvertStage(SpvReflectShaderStageFlagBits stage)
{
	switch (stage)
	{
	case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT: return ReflectStage::Vertex;
	case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT: return ReflectStage::TessControl;
	case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT: return ReflectStage::TessEval;
	case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT: return ReflectStage::Geometry;
	case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT: return ReflectStage::Fragment;
	default: return ReflectStage::Invalid;
	}
}

// ====================================================================================================================
BindingType ReflectModule::ConvertBindingType(SpvReflectDescriptorType type)
{
	switch (type)
	{
	case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER: return BindingType::Sampler;
	case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: return BindingType::CombinedImageSampler;
	case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE: return BindingType::SampledImage;
	case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE: return BindingType::StorageImage;
	case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER: return BindingType::UniformTexelBuffer;
	case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER: return BindingType::StorageTexelBuffer;
	case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return BindingType::UniformBuffer;
	case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER: return BindingType::StorageBuffer;
	case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: return BindingType::InputAttachment;
	default: return BindingType::Unknown;
	}
}

// ====================================================================================================================
uint32_t ReflectModule::GetBindingArraySize(const SpvReflectBindingArrayTraits& traits)
{
	uint32_t total = (traits.dims_count ? 1 : 0);
	for (uint32_t i = 0; i < traits.dims_count; ++i) {
		const uint32_t size = traits.dims[i];
		if (size == UINT32_MAX) {
			return UINT32_MAX; // Indicates a spec-constant-sized array, or a runtime sized array
		}
		total *= size;
	}
	return total;
}

// ====================================================================================================================
ImageDims ReflectModule::GetImageDims(const SpvReflectImageTraits& traits)
{
	if (traits.ms) {
		return ImageDims::Unknown; // Do not support multi-sampled images
	}

	switch (traits.dim)
	{
	case SpvDim1D: return traits.arrayed ? ImageDims::E1DArray : ImageDims::E1D;
	case SpvDim2D: return traits.arrayed ? ImageDims::E2DArray : ImageDims::E2D;
	case SpvDim3D: return ImageDims::E3D;
	case SpvDimCube: return traits.arrayed ? ImageDims::CubeArray : ImageDims::Cube;
	case SpvDimBuffer: return ImageDims::Buffer;
	case SpvDimSubpassData: return ImageDims::SubpassInput;
	default: return ImageDims::Unknown;
	}
}

// ====================================================================================================================
bool ReflectModule::ValidateBindingSet(BindingType type, BindingSet set)
{
	switch (type)
	{
	case BindingType::Sampler: return set == BindingSet::Samplers;
	case BindingType::CombinedImageSampler: return set == BindingSet::Samplers;
	case BindingType::SampledImage: return set == BindingSet::Images;
	case BindingType::StorageImage: return set == BindingSet::Images;
	case BindingType::UniformTexelBuffer: return set == BindingSet::Buffers;
	case BindingType::StorageTexelBuffer: return set == BindingSet::Buffers;
	case BindingType::UniformBuffer: return set == BindingSet::Buffers;
	case BindingType::StorageBuffer: return set == BindingSet::Buffers;
	case BindingType::InputAttachment: return set == BindingSet::InputAttachments;
	default: return false;
	}
}
