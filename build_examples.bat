:: Builds and installs C++ examples 

call build_setup_variables.bat

set OLDDIR=%CD%

if NOT EXIST %BUILD_DIR% (
	mkdir %BUILD_DIR%
)

cd %BUILD_DIR%

:: check if the library was compiled 
IF NOT EXIST %INSTALL_PATH%\bin\clandmark.dll (
	call %OLDDIR%\build_clandmark_libs.bat
)

:: build examples
cmake -G %CMAKE_GENERATOR% -DOPENCV_DIR=%OpenCV_DIR% -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=%INSTALL_PATH% -DBUILD_CPP_EXAMPLES=ON -DBUILD_MATLAB_BINDINGS=OFF -DBUILD_PYTHON_BINDINGS=OFF ..\
cmake --build . --target static_input --config %CMAKE_CONFIGURATION%
cmake --build . --target video_input --config %CMAKE_CONFIGURATION%

:: install 
cmake --build . --target INSTALL --config %CMAKE_CONFIGURATION%

cd %OLDDIR%

pause
