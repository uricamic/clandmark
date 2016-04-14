%% createModelForJointMV.m
% 
% 
% 2016-01-14, Michal Uricar

clc;
close all;
clearvars;

%% Timestamp

fprintf(1 ,'Started on %s \n\n', datestr(now));

%% User settings

% path to clandmark root directory
CLANDMARK_PATH = '/path2clandmark/';

% path to directory with images 
FACES = '/path2faces/Faces/';

landmark_names = {'brow-ll', 'brow-lc', 'brow-lr', 'brow-rl', 'brow-rc', 'brow-rr', 'canthi-ll', ...
    'canthi-lr', 'nose-root', 'canthi-rl', 'canthi-rr', 'ear-l', 'nose-l', 'nose-tip', 'nose-r', ...
    'ear-r', 'mouth-corner-l', 'mouth-upper', 'mouth-lower', 'mouth-corner-r', 'chin'};

views = {'-profile', '-half-profile', 'frontal', 'half-profile', 'profile'};
visible_subset = {
  [1 2 3       7 8 9       12 13 14       17 18 19    21] % -profile
  [1 2 3 4 5 6 7 8 9 10 11 12 13 14       17 18 19 20 21] % -half-profile
  [1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21] %  frontal
  [1 2 3 4 5 6 7 8 9 10 11       14 15 16 17 18 19 20 21] %  half-profile
  [      4 5 6     9 10 11       14 15 16    18 19 20 21] %  profile
};
root_comp = {9, 14, 14, 12, 7}; % nose-root (id-14) in all views

edges = {
[8 8  8 5 5 10 10 11 12 4 1 2; 
 5 7 10 4 2 11  9 12  6 3 0 1]
[13 13 13 15 15 15 16 18 8 8 8 8 2 7 3  9 1 4;
  8 12 15 14 16 17 18 11 2 3 7 9 1 6 4 10 0 5]
[13 13 13 13 17 17 17 18 20 20 8 8 8 8 2 7 3  9 1 4;
  8 12 14 17 16 18 19 20 11 15 2 3 7 9 1 6 4 10 0 5]
[11 11 11 15 15 15 16 18 8 8 8 8 2 7 3  9 1 4;
  8 12 15 14 16 17 18 13 2 3 7 9 1 6 4 10 0 5]
[6 6 6 3 3  9  9 10 12 4 0 1;
 3 7 9 0 4 10 11 12  8 5 1 2]
};

% mirroring 
mirroring = true;

% model specific parameters
bw = [60; 60];              % normalized frame size (base window) [width, height]
bw_margin = [1.5; 1.5];     % extend normalized frame by multiple [w, h]
template_size = [9; 9];   % size of the patch for computing features (appearance model)
root_template_size = [15; 15];   % size of the patch of root_landmark
% root_comp = 31;     % root component is se to be the tip of the nose

visualization = false;

% NOTE: if you would like to change the graph configuration, please change
% the edges variable in the following section

%% Add paths

addpath('../functions/');
addpath([CLANDMARK_PATH 'matlab_interface/']);
addpath([CLANDMARK_PATH 'matlab_interface/mex/']);
addpath([CLANDMARK_PATH 'matlab_interface/functions/']);

%% Load annotation file 

load('../data/F_TRN.mat');

%% Preprocess

badidx = [];
missing_detection = [];
baddetection = [];
examples_cnts = cell(size(views));
for viewID = 1 : numel(views)
    examples_cnts{viewID} = 0;
end;

for viewID = 1 : numel(views)
   
    
    %%
    M = numel(visible_subset{viewID});
    SS = ones(4, M);    % search spaces [min_x, min_y, max_x, max_y]
    components = repmat(template_size, 1, M);
    components(:, root_comp{viewID}) = root_template_size;

    create_xml_init(['tmp_' views{viewID} '.xml'], M, edges{viewID}, landmark_names(visible_subset{viewID}), SS, components, bw, bw_margin, ['TMP_' views{viewID}]);

    flandmark = flandmark_class(['tmp_' views{viewID} '.xml']);

    ss = [inf(2, M); -inf(2, M)];

    %% 
    
    N = numel(F);

    for i = 1 : N

        I = imread([FACES F{i}.imgPath]);
        try
            Ibw = rgb2gray(I);
        catch ex
            Ibw = I;
        end;
        
        if ~strcmp(views{viewID}, F{i}.view)
            continue;
        end;
        
        gt = F{i}.landmarks(:, visible_subset{viewID});

        try
            bbox = int32(F{i}.bbox);
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

        if mirroring
            NF_mirrored = fliplr(NF);
            GTNF_mirrored = GTNF;
            GTNF_mirrored(1, :) = size(NF, 1) - GTNF_mirrored(1, :) + 1;
            GTNF_mirrored = double(GTNF_mirrored-1);
        end;
        
        GTNF = double(GTNF-1);

%         flag = true;
%         for j = 1 : M
%             if ( ((GTNF(1, j)-floor(components(1, j)/2)) < 0) || ((GTNF(2, j)-floor(components(2, j)/2)) < 0) || ...
%                  ((GTNF(1, j)+floor(components(1, j)/2)) >= bw(1)) || ((GTNF(2, j)+floor(components(2, j)/2)) >= bw(2)) )
%                 flag = false;
%             end;
%         end;
        flag = true;
        GTNF(1, :) - 

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

        examples_cnts{viewID} = examples_cnts{viewID} + 1;
        
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
            for a = 1 : numel(edges{viewID}(1, :)) 
                line([gt(1, edges{viewID}(1, a)+1) gt(1, edges{viewID}(2, a)+1)], [gt(2, edges{viewID}(1, a)+1) gt(2, edges{viewID}(2, a)+1)], 'color', 'b'); 
            end;
            subplot(1, 2, 2);
            imshow(NF, []); hold on;
            plot(GTNF(1, :), GTNF(2, :), 'gx', 'MarkerSize', 3, 'LineWidth', 2);
            text(GTNF(1, :), GTNF(2, :), gtnames, 'color', 'g', 'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'right');
            for a = 1 : numel(edges{viewID}(1, :)) 
                line([GTNF(1, edges{viewID}(1, a)+1) GTNF(1, edges{viewID}(2, a)+1)], [GTNF(2, edges{viewID}(1, a)+1) GTNF(2, edges{viewID}(2, a)+1)], 'color', 'b'); 
            end;
        %%%%%%%%%
        end;
    end;
    
    %% Statistics
    
%     % bad examples independent on the settings
%     fprintf('bad examples independent on the settings:\n');
%     (numel(baddetection)+numel(missing_detection))
%     (numel(baddetection)+numel(missing_detection))/numel(F) * 100
% 
%     % bad examples due to settings (bw, bw_margin, component size)
%     fprintf('bad examples due to settings (bw, bw_margin, component size):\n');
%     (numel(badidx) - (numel(baddetection)+numel(missing_detection)))
%     (numel(badidx) - (numel(baddetection)+numel(missing_detection)))/numel(F) * 100
% 
%     % overall cut
%     fprintf('overall pruned:\n');
%     numel(badidx)
%     numel(badidx)/numel(F) * 100
    
    % overall examples
    fprintf('overall examples for training: %d \n', examples_cnts{viewID});

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

    sigma = 0.4;
    create_xml_init(['../model/withMirroring/JOINT_MV_SPLIT_1_' upper(views{viewID}) '_init.xml'], M, edges{viewID}, landmark_names(visible_subset{viewID}), SS, components, bw, bw_margin, ['JOINT_MV_SPLIT_1_' upper(views{viewID})], sigma);

end;

%% Save DB with bad indices

save(['../data/F_TRN_WithBadIndices.mat'], 'F', 'badidx', 'baddetection', 'missing_detection');

%% Timestamp

fprintf(1 ,'Finished on %s \n\n', datestr(now));
