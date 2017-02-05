:: Builds and installs CLandmark libraries (both static and dynamic version)

call build_setup_variables.bat

set OLDDIR=%CD%

if NOT EXIST %BUILD_DIR% (
	mkdir %BUILD_DIR%
)

cd %BUILD_DIR%

:: build static version of the libraries 
cmake -G %CMAKE_GENERATOR% -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=%INSTALL_PATH% -DBUILD_CPP_EXAMPLES=OFF -DBUILD_MATLAB_BINDINGS=OFF -DBUILD_PYTHON_BINDINGS=OFF ..\
cmake --build . --target clandmark --config %CMAKE_CONFIGURATION%
cmake --build . --target flandmark --config %CMAKE_CONFIGURATION%

:: build dynamic version of the libraries 
cmake -G %CMAKE_GENERATOR% -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=%INSTALL_PATH% -DBUILD_CPP_EXAMPLES=OFF -DBUILD_MATLAB_BINDINGS=OFF -DBUILD_PYTHON_BINDINGS=OFF ..\
cmake --build . --target clandmark --config %CMAKE_CONFIGURATION%
cmake --build . --target flandmark --config %CMAKE_CONFIGURATION%

:: install 
cmake --build . --target INSTALL --config %CMAKE_CONFIGURATION%

cd %OLDDIR%

pause
