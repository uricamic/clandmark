%% bmrm_compile_mex.m
% Compiles helper functions for a significant speed-ups in BMRMCONSTR2. 
% 
% 
% 2016-01-07, Michal Uricar

% clc;
clearvars; close all;

% Setup paths accpording to system platform
if (isunix)
    mex -v -largeArrayDims mxMultATB.cpp -lmwblas -output mxMultATB
    mex -v -largeArrayDims mxMultATx.cpp -lmwblas -output mxMultATx
    mex -v -largeArrayDims mxMultAx.cpp -lmwblas -output mxMultAx
end;

if (ispc)
    blaslib = fullfile(matlabroot, 'extern', 'lib', computer('arch'), 'microsoft', 'libmwblas.lib');
    mex('-v', '-largeArrayDims', 'mxMultATB.cpp', blaslib);
    mex('-v', '-largeArrayDims', 'mxMultATx.cpp', blaslib);
    mex('-v', '-largeArrayDims', 'mxMultAx.cpp', blaslib);
end;

if (ismac)
    % TODO
end;

fprintf('Compilation finished.\n');
