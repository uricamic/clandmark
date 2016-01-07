%% createModelFor300W
% 
% 
% 2014-12-15, Michal Uricar
% 2016-01-06, Michal Uricar

clc;
close all;
clearvars;

%% Timestamp

fprintf(1 ,'Started on %s \n\n', datestr(now));

%% User settings

% path to clandmark root directory
CLANDMARK_PATH = '/path/to/clandmark/root/directory/';

% path to directory with images 
FACES = '/path/to/300W/images/';

% model specific parameters
bw = [80; 80];              % normalized frame size (base window) [width, height]
bw_margin = [1.5; 1.5];     % extend normalized frame by multiple [w, h]
template_size = [13; 13];   % size of the patch for computing features (appearance model)
root_template_size = [21; 21];   % size of the patch of root_landmark
root_comp = 31;     % root component is se to be the tip of the nose

visualization = false;

% NOTE: if you would like to change the graph configuration, please change
% the edges variable in the following section

%% Add paths

addpath('../functions/');
addpath([CLANDMARK_PATH 'matlab_interface/']);
addpath([CLANDMARK_PATH 'matlab_interface/mex/']);
addpath([CLANDMARK_PATH 'matlab_interface/functions/']);

%% Load splits for 300W

load('../data/300W_landmarks.mat');    % MAT-file with annotation structure F

%% 68-landmarks variant

M = 68;             % 300W annotation contains 68 landmarks

% Edges have to form a tree and the format is [parent; child], 1-based
edges = [ 
    [2:9; 1:8] [16:-1:9; 17:-1:10] [58; 9], ...                                                 % outer countour
    [19:22; 18:21] [28; 22] [26:-1:23; 27:-1:24] [28; 23] , ...                                 % left and right brow + nose root connection 
    [38:40; 37:39] [28; 40] [38 39; 42 41] [45:-1:43; 46:-1:44] [28; 43] [44 45; 48 47], ...    % left and right eye + nose root connection 
    [29:31; 28:30] [31; 34] [33 34 35 34; 32 33 36 35] , ...                                    % nose
    [34; 52] , ...
    [50 51 52; 49 50 51] [50 51; 61 62] [52 53 54; 53 54 55] [53 54; 64 65] , ...
    [52; 63] [63; 67] [67; 58] , ...
    [59 58; 60 59] [59; 68] [57 58; 56 57] [57; 66] , ... 
];

landmark_names = {};
for a = 1 : M, landmark_names{a} = ['S' num2str(a-1)]; end;

%% Visualization 

if visualization
    i = 2;
    I = imread([FACES F{i}.imgPath]);
    [H, W, D] = size(I);
    P = F{i}.L68;

    figure(1); clf(1);
    imshow(I, []); hold on;
    for a = 1 : numel(edges(1, :)) 
        line([P(1, edges(1, a)) P(1, edges(2, a))], [P(2, edges(1, a)) P(2, edges(2, a))], 'Marker', '>', 'MarkerSize', 10, 'color', 'b', 'LineWidth', 2);
    end;
    plot(P(1, :), P(2, :), 'gx', 'MarkerSize', 6, 'LineWidth', 3);
    plot(P(1, root_comp), P(2, root_comp), 'mx', 'MarkerSize', 5, 'LineWidth', 3);
end;

%% Preprocess 

SS = ones(4, M);    % search spaces [min_x, min_y, max_x, max_y]
components = repmat(template_size, 1, M);
components(:, root_comp) = root_template_size;

create_xml_init('tmp.xml', M, edges-1, landmark_names, SS, components, bw, bw_margin, 'TMP');

flandmark = flandmark_class('tmp.xml');

ss = [inf(2, M); -inf(2, M)];

badidx = [];
missing_detection = [];
baddetection = [];

%% 

N = numel(F);

for i = 1 : N
    
    I = imread([FACES F{i}.imgPath]);
    try
        Ibw = rgb2gray(I);
    catch ex
        Ibw = I;
    end;
    gt = F{i}.L68;

    try
        bbox = int32(F{i}.fullbox);
    catch ex
        badidx(end+1) = i;
        missing_detection(end+1) = i;
        fprintf('%d Missing face box...\n', i);
        continue;
    end;
        
    % Filter out bad detections (too big or too small bbox, given the ground truth)
    overlap = getOverlapAABB(gt, reshape(bbox, 2, 4));
    if overlap < 0.1
        baddetection(end+1) = i;
        badidx(end+1) = i;
        fprintf('%d Detected bounding box seems to be odd compared to bbox of the ground truth annotation...\n', i);
        continue;
    end;
    
    [NF, GTNF] = flandmark.getNormalizedFrame(Ibw, bbox, gt);
    
    GTNF = double(GTNF-1);
    
    flag = true;
    for j = 1 : M
        if ( ((GTNF(1, j)-floor(components(1, j)/2)) < 0) || ((GTNF(2, j)-floor(components(2, j)/2)) < 0) || ...
             ((GTNF(1, j)+floor(components(1, j)/2)) >= bw(1)) || ((GTNF(2, j)+floor(components(2, j)/2)) >= bw(2)) )
            flag = false;
        end;
    end;
    
    if flag
        for j = 1 : M
            bb = [GTNF(1, j)-floor(components(1, j)/2), GTNF(2, j)-floor(components(2, j)/2), ...
                  GTNF(1, j)+floor(components(1, j)/2), GTNF(2, j)+floor(components(2, j)/2) ];
            ss(1, j) = min(ss(1, j), bb(1));
            ss(2, j) = min(ss(2, j), bb(2));
            ss(3, j) = max(ss(3, j), bb(3));
            ss(4, j) = max(ss(4, j), bb(4));
        end;
    else
        badidx(end+1) = i;
        fprintf('%d NOT PASSED...\n', i);
    end;
    
    fprintf('%d/%d\n', i, N);
    
    %%% visualization 
    if visualization
        figure(2); clf(2);
        subplot(1, 2, 1);
        imshow(I, []); hold on;
        plotbox_full(bbox, 'color', 'r');
        plotbox(gtbbox, 'color', 'g');
        plotbox(aabb, 'color', 'r');
        plot(gt(1, :), gt(2, :), 'gx', 'MarkerSize', 3, 'LineWidth', 2);
        text(gt(1, :), gt(2, :), gtnames, 'color', 'g', 'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'right');
        for a = 1 : numel(edges(1, :)) 
            line([gt(1, edges(1, a)+1) gt(1, edges(2, a)+1)], [gt(2, edges(1, a)+1) gt(2, edges(2, a)+1)], 'color', 'b'); 
        end;
        subplot(1, 2, 2);
        imshow(NF, []); hold on;
        plot(GTNF(1, :), GTNF(2, :), 'gx', 'MarkerSize', 3, 'LineWidth', 2);
        text(GTNF(1, :), GTNF(2, :), gtnames, 'color', 'g', 'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'right');
        for a = 1 : numel(edges(1, :)) 
            line([GTNF(1, edges(1, a)+1) GTNF(1, edges(2, a)+1)], [GTNF(2, edges(1, a)+1) GTNF(2, edges(2, a)+1)], 'color', 'b'); 
        end;
    %%%%%%%%%
    end;
end;

%% Statistics

% % bad examples independent on the settings
fprintf('bad examples independent on the settings:\n');
(numel(baddetection)+numel(missing_detection))
(numel(baddetection)+numel(missing_detection))/numel(F) * 100

% bad examples due to settings (bw, bw_margin, component size)
fprintf('bad examples due to settings (bw, bw_margin, component size):\n');
(numel(badidx) - (numel(baddetection)+numel(missing_detection)))
(numel(badidx) - (numel(baddetection)+numel(missing_detection)))/numel(F) * 100

% overall cut
fprintf('overall pruned:\n');
numel(badidx)
numel(badidx)/numel(F) * 100

%% Save DB with bad indices

save('../data/300W_WithBadIndices.mat', 'F', 'tstIdx', 'trnIdx', 'valIdx', 'badidx', 'baddetection', 'missing_detection');

%% Final adjust of search spaces

for j = 1 : M
    SS(1, j) = ss(1, j) + floor(components(1, j)/2);
    SS(2, j) = ss(2, j) + floor(components(2, j)/2);
    SS(3, j) = ss(3, j) - floor(components(1, j)/2);
    SS(4, j) = ss(4, j) - floor(components(2, j)/2);
end;

% Show size of search regions
(SS(3, :) - SS(1, :) + 1) .* (SS(4, :) - SS(2, :) + 1)

%% Create XML init file

sigma = 1.0;
create_xml_init('../model/SV_init.xml', M, edges-1, landmark_names, SS, components, bw, bw_margin, 'SINGLE_VIEW', sigma);

%% Timestamp

fprintf(1 ,'Finished on %s \n\n', datestr(now));
