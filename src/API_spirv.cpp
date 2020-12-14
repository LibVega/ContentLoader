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
VEGA_API_EXPORT ReflectError vegaSpirvGetStage(const ReflectModule* mod, ReflectStage* stage)
{
	if (mod) {
		if (!mod->hasError()) {
			*stage = mod->stage();
		}
		return mod->error();
	}
	return ReflectError::NullModule;
}

/// SPIRV API: Get module entry point
VEGA_API_EXPORT ReflectError vegaSpirvGetEntryPoint(const ReflectModule* mod, const char** entryPoint)
{
	if (mod) {
		if (!mod->hasError()) {
			*entryPoint = mod->entryPoint();
		}
		return mod->error();
	}
	return ReflectError::NullModule;
}

/// SPIRV API: Get module push block size
VEGA_API_EXPORT ReflectError vegaSpirvGetPushSize(const ReflectModule* mod, uint32_t* pushSize)
{
	if (mod) {
		if (!mod->hasError()) {
			*pushSize = mod->pushSize();
		}
		return mod->error();
	}
	return ReflectError::NullModule;
}

/// SPIRV API: Close Reflection Module
VEGA_API_EXPORT void vegaSpirvDestroyModule(ReflectModule* mod)
{
	if (mod) {
		delete mod;
	}
}
