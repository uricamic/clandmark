%% flandmark_simple_example.m
% Test flandmark detector
% 
% 08-04-13 Michal Uricar

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

DIR = '../data/Images/';
IMGS = dir([DIR '*.jpg']);

N = 10;

%% Init flandmark

% model = './models/MultiView_frontal_1.xml';
% model = './models/MultiView_frontal_SINGLE.xml';
model = './models/8Lfrontal_LFW_SPLIT_1.xml';
% model = './models/8Lfrontal_LFW_SPLIT_SINGLE.xml';

T = flandmark_xmlread(model);

tic
flandmark = flandmark_class(model);
t = toc;
fprintf('MEX init:    Elapsed time %f ms\n', t*1000);


%% Run detector

timestat = zeros(N, 1);

for example = 1 : N
    
    % pick random image
%     idx = randi(numel(IMGS));
%     idx = example;
    idx = 1;
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
        tic
        P = flandmark.detect(Ibw, int32(bbox(i, :)));
        t1 = toc;
        fprintf('MEX detect:    Elapsed time %f ms \t %s \n', t1*1000, fname);
        
        timestat(example) = timestat(example)+t1;
        
        % show landmarks 
%         comps = ['S0'; 'S1'; 'S2'; 'S3'; 'S4'; 'S5'; 'S6'; 'S7'];
        comps = {}; for a = 1 : flandmark.getLandmarksCount(); comps{end+1} = ['S' num2str(a)]; end;
%         plot(P(1, 1), P(2, 1), 'bs', 'LineWidth', 1, 'MarkerSize', 5, 'MarkerFaceColor', 'b');
%         text(P(1, 1), P(2, 1), comps(1,:), 'color', 'b', 'FontSize', 12, 'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'right');
        plot(P(1, :), P(2, :), 'rs', 'LineWidth', 1, 'MarkerSize', 5, 'MarkerFaceColor', 'r');
        text(P(1, :), P(2, :), comps, 'color', 'r', 'FontSize', 12, 'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'right');
        for a = 1 : numel(T.edges(1, :)) 
            line([P(1, T.edges(1, a)) P(1, T.edges(2, a))], [P(2, T.edges(1, a)) P(2, T.edges(2, a))], 'color', 'b'); 
        end;
    end;
    
    pause(0.3);
    
end;

%% Destroy flandmark

clear flandmark;