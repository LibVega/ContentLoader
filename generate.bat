@rem MIT License - Copyright (c) 2020 Sean Moss
@rem This file is subject to the terms and conditions of the MIT License, the text of which can be found in the
@rem 'LICENSE' file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.

@rem Windows script for generating the project build files with Premake.

@echo off

start /D "./premake" /W /B premake5.exe --file="../content.project" vs2019
