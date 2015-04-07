%% flandmark_compilemex.m
% Compilation script for flandmark mex-files.
%
% 2013-04-08, Michal Uricar

% clc;
clearvars; close all;

include = ' -I../libclandmark/ -I../3rd_party/rapidxml-1.13/ -I../3rd_party/CImg-1.5.6/ -I../3rd_party/CImg-1.5.6/plugins';

% Setup paths accpording to system platform
if (isunix)
    fprintf('Compiling mex-files on Unix platform...\n');

%     libclandmark = ' -L../build/libclandmark/ -lflandmark -lclandmark ';
%     libclandmark = ' -L../build/libclandmark/ -lflandmark -lclandmark  RPATH=''-Wl,-rpath-link,../bin/libclandmark/'' ';
%     libclandmark = ' -L../build/libclandmark/ -lflandmark -lclandmark  RPATH=''-Wl,-rpath-link,./'' ';
    libclandmark = ' ../build/libclandmark/libflandmark.a ../build/libclandmark/libclandmark.a -DDOUBLE_PRECISION=1 ';

end;

if (ispc)
    fprintf('Compiling mex-files on Windows platform...\n');

    libclandmark = ' -L../build/libclandmark/Release -lflandmark -lclandmark -DDOUBLE_PRECISION=1 ';

end;

if (ismac)
    fprintf('Compiling mex-files on Mac platform...\n');

    libclandmark = ' -L../build/libclandmark/ -lclandmark -lflandmark -DDOUBLE_PRECISION=1 ';

end;

% compile mexfiles
eval(['mex -v -O -largeArrayDims flandmark_interface_mex.cpp ' include libclandmark ' -output ./mex/flandmark_interface']);
eval(['mex -v -O -largeArrayDims featuresPool_interface_mex.cpp ' include libclandmark ' -output ./mex/featuresPool_interface']);

fprintf('Compilation finished.\n');
