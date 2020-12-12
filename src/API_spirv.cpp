/*
 * MIT License - Copyright (c) 2020 Sean Moss
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the
 * 'LICENSE' file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

#include "./spirv/ReflectModule.hpp"

/// SPIRV API: Create Reflection Module
VEGA_API_EXPORT ReflectModule* vegaSpirvCreateModule(const uint32_t* code, size_t size, ReflectError* error)
{
	auto mod = new ReflectModule(code, size);
	*error = mod->error();
	if (mod->hasError()) {
		delete mod;
		return nullptr;
	}
	return mod;
}

/// SPIRV API: Get module error code
VEGA_API_EXPORT ReflectError vegaSpirvGetError(const ReflectModule* mod)
{
	return mod ? mod->error() : ReflectError::NullModule;
}

/// SPIRV API: Get module shader stage
VEGA_API_EXPORT ShaderStage vegaSpirvGetStage(const ReflectModule* mod)
{
	return mod ? mod->stage() : ShaderStage::Invalid;
}

/// SPIRV API: Get module entry point
VEGA_API_EXPORT const char* vegaSpirvGetEntryPoint(const ReflectModule* mod)
{
	return mod ? mod->entryPoint() : nullptr;
}

/// SPIRV API: Get module descriptor count
VEGA_API_EXPORT uint32_t vegaSpirvGetDescriptorCount(const ReflectModule* mod)
{
	return mod ? mod->descriptorCount() : 0;
}

/// SPIRV API: Get module input count
VEGA_API_EXPORT uint32_t vegaSpirvGetInputCount(const ReflectModule* mod)
{
	return mod ? mod->inputCount() : 0;
}

/// SPIRV API: Get module output count
VEGA_API_EXPORT uint32_t vegaSpirvGetOutputCount(const ReflectModule* mod)
{
	return mod ? mod->outputCount() : 0;
}

/// SPIRV API: Get module push block size
VEGA_API_EXPORT uint32_t vegaSpirvGetPushSize(const ReflectModule* mod)
{
	return mod ? mod->pushSize() : 0;
}

/// SPIRV API: Reflect module descriptor
VEGA_API_EXPORT VegaBool vegaSpirvReflectDescriptor(const ReflectModule* mod, uint32_t index, DescriptorInfo* info)
{
	return mod ? mod->reflectDescriptor(index, info) : VEGA_FALSE;
}

/// SPIRV API: Close Reflection Module
VEGA_API_EXPORT void vegaSpirvDestroyModule(ReflectModule* mod)
{
	if (mod) {
		delete mod;
	}
}
