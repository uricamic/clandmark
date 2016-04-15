%% simple_example.m
% Test flandmark detector
% 
% 04-15-16 Michal Uricar

clc; 
clearvars; close all;

%% Add path

addpath('../learning/flandmark/code/functions/');
 
% DOUBLE PRECISION
rmpath('./mex_single_precision/');
addpath('./mex/');

% SINGLE PRECISION
% rmpath('./mex/');
% addpath('./mex_single_precision/');

addpath('./functions/');

DIR = '../data/Images/';
IMGS = dir([DIR '*.jpg']);

N = 10;

%% Init flandmark

% CDPM
cdpm_model = './models/CDPM.xml';
cdpm_flandmark = flandmark_class(cdpm_model);
bw = cdpm_flandmark.getBWsize();
cdpm_featuresPool = featuresPool_class(bw(1), bw(2));
cdpm_featuresPool.addLBPSparseFeatures(0);
cdpm_flandmark.setFeaturesPool(cdpm_featuresPool.getHandle());

% FDPM
fdpm_model = './models/FDPM.xml';
fdpm_flandmark = flandmark_class(fdpm_model);
bw = fdpm_flandmark.getBWsize();
fdpm_featuresPool = featuresPool_class(bw(1), bw(2));
fdpm_featuresPool.addLBPSparseFeatures(0);
fdpm_flandmark.setFeaturesPool(fdpm_featuresPool.getHandle());

edges = fdpm_flandmark.getEdges();
landmark_names = fdpm_flandmark.getLandmarkNames();

%% Run detector

for example = 1 : N
    
    % pick random image
    idx = randi(numel(IMGS));
%     idx = example;
    filename = IMGS(idx).name;
    fname = filename(1:end-4);
    % load image and detected face bbox
    I = imread([DIR filename]);
    Ibw = rgb2gray(I);
    bbox = dlmread([DIR fname '.det']);

    % image output
    figure(1); clf(1);
    imshow(I, [], 'Border', 'tight'); hold on;
    % plotbox(bbox);

    for i = 1 : size(bbox, 1)
        [ P, refined_bbox, Stats ] = c2f_dpm(Ibw, int32(bbox(i, :)), cdpm_flandmark, fdpm_flandmark);
        fprintf('MEX detect:    Elapsed time %f ms \t %s \n', Stats.CDPM.overall+Stats.FDPM.overall, fname);
                
        % show landmarks 
        plot(P(1, :), P(2, :), 'rs', 'LineWidth', 1, 'MarkerSize', 5, 'MarkerFaceColor', 'r');
        text(P(1, :), P(2, :), landmark_names, 'color', 'r', 'FontSize', 12, 'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'right');
        for a = 1 : numel(edges(1, :)) 
            line([P(1, edges(1, a)) P(1, edges(2, a))], [P(2, edges(1, a)) P(2, edges(2, a))], 'color', 'b'); 
        end;
    end;
    
    pause(0.3);
    
end;

%% Destroy flandmark

clear cdpm_flandmark;
clear fdpm_flandmark;
clear cdpm_featuresPool;
clear fdpm_featuresPool;
