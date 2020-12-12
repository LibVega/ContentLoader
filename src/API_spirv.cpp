/*
 * MIT License - Copyright (c) 2020 Sean Moss
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the
 * 'LICENSE' file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

#include "./spirv/ReflectModule.hpp"

/// SPIRV API: Open Reflection Module
VEGA_API_EXPORT ReflectModule* vegaSpirvReflectModule(const uint32_t* code, size_t size, ReflectError* error)
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
VEGA_API_EXPORT ReflectError vegaSpirvGetModuleError(const ReflectModule* mod)
{
	return mod ? mod->error() : ReflectError::NullModule;
}

/// SPIRV API: Get module shader stage
VEGA_API_EXPORT ShaderStage vegaSpirvGetModuleStage(const ReflectModule* mod)
{
	return mod ? mod->stage() : ShaderStage::Invalid;
}

/// SPIRV API: Get module entry point
VEGA_API_EXPORT const char* vegaSpirvGetModuleEntryPoint(const ReflectModule* mod)
{
	return mod ? mod->entryPoint() : nullptr;
}

/// SPIRV API: Close Reflection Module
VEGA_API_EXPORT void vegaSpirvDestroyModule(ReflectModule* mod)
{
	if (mod) {
		delete mod;
	}
}
