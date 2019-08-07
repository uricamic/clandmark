:: Builds and installs Python interface

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

::python interface build
cmake -G %CMAKE_GENERATOR% \
	-DPYTHON_LIBRARY=%PYTHON_LIBRARY% \
	-DPYTHON_EXECUTABLE=%PYTHON_EXECUTABLE% \
	-DPYTHON_INCLUDE_DIR=%PYTHON_INCLUDE_DIR% \
	-DCYTHON_EXECUTABLE=%CYTHON_EXECUTABLE% \
	-DBUILD_SHARED_LIBS=ON \
	-DCMAKE_INSTALL_PREFIX=%INSTALL_PATH% \
	-DBUILD_CPP_EXAMPLES=OFF \
	-DBUILD_MATLAB_BINDINGS=OFF \
	-DBUILD_PYTHON_BINDINGS=ON ..\

cmake --build . --target ReplicatePythonSourceTree %CMAKE_CONFIGURATION%
cmake --build . --target py_featurePool --config %CMAKE_CONFIGURATION%
cmake --build . --target py_flndmark --config %CMAKE_CONFIGURATION%

:: install
cmake --build . --target INSTALL --config %CMAKE_CONFIGURATION%

cd %OLDDIR%

pause
