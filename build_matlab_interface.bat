:: Builds and installs MATLAB interface

call build_setup_variables.bat

set OLDDIR=%CD%

if NOT EXIST %BUILD_DIR% (
	mkdir %BUILD_DIR%
)

cd %BUILD_DIR%

:: check if the library was compiled 
IF NOT EXIST %INSTALL_PATH%\lib\clandmark.lib (
	call %OLDDIR%\build_clandmark_libs.bat
)

:: build MATLAB interface
cmake -G%CMAKE_GENERATOR% -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=%INSTALL_PATH% -DBUILD_CPP_EXAMPLES=OFF -DBUILD_MATLAB_BINDINGS=ON -DBUILD_PYTHON_BINDINGS=OFF ..\
cmake --build . --target flandmark_interface 	--config %CMAKE_CONFIGURATION%
cmake --build . --target featuresPool_interface --config %CMAKE_CONFIGURATION%

:: install 
cmake --build . --target INSTALL --config %CMAKE_CONFIGURATION%

cd %OLDDIR%

pause
