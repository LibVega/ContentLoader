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

#define VEGA_MAX_SET_COUNT (4) // 4 sets total
#define VEGA_MAX_PER_SET_SLOTS (8) // 8 binding slots per binding set

/// Vega uses a slot-based binding model, and the binding types are specific to certain set indices:
///    (0) - Buffers, including all uniform and storage buffers (both readonly and read/write)
///    (1) - Read-Only texel objects, including samplers, sampled textures, and uniform texel buffers
///    (2) - Read-Write texel objects, including storage textures and storage texel buffers
///    (3) - Input attachments, this is managed by the library and cannot be explicitly bound by users
///


// Reflection error codes
enum class ReflectError : uint32_t
{
	None = 0,                    // No error
	NullModule = 1,              // Special public API error for passing a null module
	InvalidBytecode = 2,         // Bad bytecode or other parsing error
	InvalidStage = 3,            // Unsupported shader stage
	InvalidPushBlockCount = 4,   // Too many push blocks (>1)
	MultipleEntryPoints = 5,     // Module has more than one entry point
	UnsupportedBindingType = 6,  // The descriptor type is unknown or unsupported
	InvalidBindingType = 7,      // The descriptor type is invalid for the set it appears in
	BindingSetOutOfRange = 8,    // A descriptor is bound to an invalid set index (>= 
	BindingSlotOutOfRange = 9,   // A descriptor is bound to an invalid slot index (>= VEGA_MAX_PER_SET_SLOTS)
}; // enum class ReflectError


// Reflection shader stages
enum class ReflectStage : uint32_t
{
	Invalid = 0,
	Vertex = 1,
	TessControl = 2,
	TessEval = 3,
	Geometry = 4,
	Fragment = 5
}; // enum class ReflectStage


// The "namespaces" for the binding sets
enum class BindingSet : uint32_t
{
	Buffer = 0,            // Buffer objects set (=0)
	ReadOnlyTexel = 1,     // Read-Only texel objects set (=1)
	ReadWriteTexel = 2,    // Read/Write texel objects set (=2)
	InputAttachments = 3,  // Input attachments set (=3)
}; // enum class BindingSet


// Types for descriptor bindings
enum class BindingType : uint32_t
{
	Unknown = 0,
	Sampler = 1,
	CombinedImageSampler = 2,
	SampledImage = 3,
	StorageImage = 4,
	UniformTexelBuffer = 5,
	StorageTexelBuffer = 6,
	UniformBuffer = 7,
	StorageBuffer = 8,
	InputAttachment = 9
}; // enum class BindingType


// Possible image dimension types
enum class ImageDims : uint32_t
{
	Unknown = 0,
	E1D = 1,
	E1DArray = 2,
	E2D = 3,
	E2DArray = 4,
	E3D = 5,
	Cube = 6,
	CubeArray = 7
}; // enum class ImageDims


// Info about a binding slot (packed for C# interop)
#pragma pack(push, 1)
struct BindingInfo final
{
	const char* name;     // The declared binding name
	BindingSet set;       // Binding set (set # in 'layout(...)')
	uint32_t slot;        // Binding slot (binding # in 'layout(...)')
	BindingType type;     // The binding type
	uint32_t blockSize;   // Block size in bytes, if buffer type, UINT32_MAX if unknown/dynamic
	uint32_t arraySize;   // Array size in elements, along all dims, UINT32_MAX if unknown/dynamic
	ImageDims imageDims;  // The dims of the image, if image type
}; // struct BindingInfo
#pragma pack(pop)


// Manages the handle and data for a SPIRV reflection module
class ReflectModule final
{
public:
	ReflectModule(const uint32_t* code, size_t size);
	~ReflectModule();

	// Error
	inline ReflectError error() const { return error_; }
	inline bool hasError() const { return error_ != ReflectError::None; }

	// Top-level reflection
	inline ReflectStage stage() const { return stage_; }
	inline const char* entryPoint() const { return entryPoint_.c_str(); }
	inline uint32_t pushSize() const { return pushSize_; }

private:

private:
	ReflectError error_;
	ReflectStage stage_;
	std::string entryPoint_;
	uint32_t pushSize_;
}; // class ReflectModule
