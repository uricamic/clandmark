%% trnasform_F2learning.m
% 
% 
% 2015-09-07, Michal Uricar
% 2016-01-06, Michal Uricar

clc;
close all;
clearvars;

%% Timestamp 

fprintf(1,'Started on %s\n\n', datestr(now));

%% User Settings

CLANDMARK_PATH = '/path/to/clandmark/root/directory/';
image_path = '/path/to/300W/images/';

%% Add paths

addpath('../functions/');
addpath([CLANDMARK_PATH 'matlab_interface/']);
addpath([CLANDMARK_PATH 'matlab_interface/mex/']);
addpath([CLANDMARK_PATH 'matlab_interface/functions/']);

%% Init CLandmark

flandmark = flandmark_class('../model/SV_init.xml', 1);

%% Prepare training data (i.e. obtain normalized frames, ground truth annotations in them and normalization constants)

SPLIT = 'SPLIT_1';

load(['TRN_' SPLIT '.mat']);

[Images, GTs, kappas] = prepareImagesGT(flandmark, F, image_path);
save(['data_TRN_' SPLIT '.mat'], 'Images', 'GTs', 'kappas');

load(['VAL_' SPLIT '.mat']);

[VALImages, VALGTs, VALkappas] = prepareImagesGT(flandmark, F, image_path);
save(['data_VAL_' SPLIT '.mat'], 'VALImages', 'VALGTs', 'VALkappas');

%% Timestamp 

fprintf(1,'Finished on %s\n\n', datestr(now));
