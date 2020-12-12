/*
 * MIT License - Copyright (c) 2020 Sean Moss
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the
 * 'LICENSE' file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

#pragma once

#include "../config.hpp"

#ifdef VEGA_MSVC
#	pragma warning(disable: 26812) // Unscoped enums
#endif // VEGA_MSVC

#include "./spirv_reflect.h"

#include <vector>


// Reflection error codes
enum class ReflectError : uint32_t
{
	None = 0,
	NullModule = 1, // Special public API error for passing a null module
	InvalidBytecode = 2, // Bad bytecode or other parsing error
	InvalidStage = 3, // Unsupported shader stage
	InvalidPushBlockCount = 4, // Too many push blocks (>1)
	BadMemberIndex = 5, // Invalid index for accessing module members (descriptors, inputs, outputs)
	BadDescriptorType = 6, // Unsupported descriptor type
	BadImageType = 7, // Unsupported image or sampler dimensions
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


// Types for descriptor bindings
enum class DescriptorType : uint32_t
{
	Unknown = 0,
	Sampler = 1,
	Image = 2,
	ImageSampler = 3, // Combined image sampler
	UniformBuffer = 4,
	InputAttachment = 5
}; // enum class DescriptorType


// Possible image dimension types
enum class ImageType : uint32_t
{
	Unknown = 0,
	E1D = 1,
	E2D = 2,
	E3D = 3,
}; // enum class ImageType


// Descriptor Info (meant for interop with C# code, hence the packing)
#pragma pack(push, 1)
struct DescriptorInfo final
{
public:
	const char* name;    // Name
	uint32_t set;        // Set location
	uint32_t binding;    // Binding location
	uint32_t inputIndex; // Input attachment index, or zero
	DescriptorType type; // Type
	uint32_t size;       // For block types, this is the size in bytes, otherwise zero
	uint32_t arraySize;  // For all types, this is the total size of all dims of the array, otherwise zero
	struct {
		ImageType type;  // Image type
		VegaBool ms;     // If the image is multisampled
	} image;
}; // struct DescriptorInfo
#pragma pack(pop)


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
	inline uint32_t descriptorCount() const { return uint32_t(descriptors_.size()); }
	inline uint32_t inputCount() const { return uint32_t(inputs_.size()); }
	inline uint32_t outputCount() const { return uint32_t(outputs_.size()); }
	uint32_t pushSize() const;

	// Member reflect
	VegaBool reflectDescriptor(uint32_t index, DescriptorInfo* info) const;

private:
	// Convert the library stage to the public API stage
	static ShaderStage ConvertStage(SpvReflectShaderStageFlagBits stage);
	static DescriptorType ConvertDescriptorType(SpvReflectDescriptorType type);
	static uint32_t GetArraySize(const SpvReflectBindingArrayTraits& traits);
	static ImageType GetImageType(const SpvReflectImageTraits& traits);

private:
	spv_reflect::ShaderModule* module_;
	mutable ReflectError error_;
	ShaderStage stage_;
	std::vector<const SpvReflectDescriptorBinding*> descriptors_;
	std::vector<const SpvReflectInterfaceVariable*> inputs_;
	std::vector<const SpvReflectInterfaceVariable*> outputs_;
}; // class ReflectModule
