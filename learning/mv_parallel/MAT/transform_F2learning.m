%% trnasform_F2learning.m
% 
% 
% 2016-01-15, Michal Uricar

clc;
close all;
clearvars;

%% Timestamp 

fprintf(1,'Started on %s\n\n', datestr(now));

%% User Settings

CLANDMARK_PATH = '/path2/clandmark/';
image_path = '/path2/Faces/';

%% Add paths

addpath('../functions/');
addpath([CLANDMARK_PATH 'matlab_interface/']);
addpath([CLANDMARK_PATH 'matlab_interface/mex/']);
addpath([CLANDMARK_PATH 'matlab_interface/functions/']);

%% A

A.views = {'-profile', '-half-profile', 'frontal', 'half-profile', 'profile'};
A.visible_subset = {
  [1 2 3       7 8 9       12 13 14       17 18 19    21] % -profile
  [1 2 3 4 5 6 7 8 9 10 11 12 13 14       17 18 19 20 21] % -half-profile
  [1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21] %  frontal
  [1 2 3 4 5 6 7 8 9 10 11       14 15 16 17 18 19 20 21] %  half-profile
  [      4 5 6     9 10 11       14 15 16    18 19 20 21] %  profile
};

PHIS = numel(A.views);

save('../MAT/A.mat', 'A', 'PHIS');

%% Init flandmark pool

flandmark_pool = cell(PHIS, 1);

FLANDMARK_INIT_FILE = {
    '../model/JOINT_MV_SPLIT_1_-PROFILE_init.xml'
    '../model/JOINT_MV_SPLIT_1_-HALF-PROFILE_init.xml'
    '../model/JOINT_MV_SPLIT_1_FRONTAL_init.xml'
    '../model/JOINT_MV_SPLIT_1_HALF-PROFILE_init.xml'
    '../model/JOINT_MV_SPLIT_1_PROFILE_init.xml'
};

for phi = 1 : PHIS
    flandmark_pool{phi} = flandmark_class(FLANDMARK_INIT_FILE{phi}, true);
end;

% Feature Pool 
bw = flandmark_pool{1}.getBWsize();
featuresPool = featuresPool_class(bw(1), bw(2));
featuresPool.addLBPSparseFeatures();
for phi = 1 : PHIS
    flandmark_pool{phi}.setFeaturesPool(featuresPool.getHandle);
end;

%% models

models = cell(PHIS, 1);
for phi = 1 : PHIS
    models{phi}.ss = flandmark_pool{phi}.getNodesSearchSpaces();
    models{phi}.components = flandmark_pool{phi}.getNodesWindowSizes();
    models{phi}.bw = flandmark_pool{phi}.getBWsize();
    models{phi}.M = flandmark_pool{phi}.getLandmarksCount();
    models{phi}.Woffsets = flandmark_pool{phi}.getWoffsets();
    models{phi}.nodeWdims = flandmark_pool{phi}.getNodesDimensions();
    models{phi}.edgeWdims = flandmark_pool{phi}.getEdgesDimensions();
    models{phi}.view = A.views{phi};
    models{phi}.L = repmat({ones(models{phi}.bw')}, 1, models{phi}.M);
end;

%% Prepare training data (i.e. obtain normalized frames, ground truth annotations in them and normalization constants)

viewID = find(strcmp('frontal', A.views));

SPLIT = 'SPLIT_1';

% load(['TRN_' SPLIT '.mat']);
load(['../data/F_TRN.mat']);

[Images, GTs, Yaws, kappas] = jointmv_prepareImagesGT(flandmark_pool{viewID}, models, A, F, image_path);
save(['data_TRN_' SPLIT '.mat'], 'Images', 'GTs', 'Yaws', 'kappas');

% load(['VAL_' SPLIT '.mat']);
load(['../data/F_VAL.mat']);

[VALImages, VALGTs, VALYaws, VALkappas] = jointmv_prepareImagesGT(flandmark_pool{viewID}, models, A, F, image_path);
save(['data_VAL_' SPLIT '.mat'], 'VALImages', 'VALGTs', 'VALYaws', 'VALkappas');

%% Timestamp 

fprintf(1,'Finished on %s\n\n', datestr(now));
