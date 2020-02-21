rem build rt on windows using nvidia opencl lib
@echo off

mkdir .\build
pushd .\build
cl ..\src\main.c ..\src\init.c ..\src\catch_event.c ..\src\error_handling.c ..\src\init_window.c^
	..\src\create_program.c ..\src\read_data.c^
					-I ../include ^
					-I ../Libs/libft/includes ^
					-I ../Libs/SDL/include ^
					/link /SUBSYSTEM:CONSOLE ^
					../Libs/libft/libft.lib ^
					../Libs\SDL\lib\win64\SDL2main.lib ^
					../Libs\SDL\lib\win64\SDL2.lib ^
					"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.2\lib\x64\OpenCL.lib"^
					"Shell32.lib"
rem /link /SUBSYSTEM:CONSOLE ^

popd