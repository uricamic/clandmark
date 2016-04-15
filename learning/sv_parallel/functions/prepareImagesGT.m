function [ Images, GTs, kappas ] = prepareImagesGT( flandmark, Annotation, image_path )
%PREPARETRAININGDATA Summary of this function goes here
%   Detailed explanation goes here
% 
% 2015-09-07, Michal Uricar

    N = numel(Annotation);
    Images = cell(N, 1);
    GTs = cell(N, 1);
    kappas = nan(N, 1);
    idx = 0;
    
    model.ss = flandmark.getNodesSearchSpaces();
    model.components = flandmark.getNodesWindowSizes();
    model.bw = flandmark.getBWsize();
    
    for i = 1 : N
        
        fname = Annotation{i}.imgPath;
        I = imread([image_path fname]);
        
        try
            Ibw = rgb2gray(I);
        catch ex
            Ibw = I;
        end;
        
        gt = Annotation{i}.L68;
        bbox = int32(Annotation{i}.fullbox);
        
        [Image, GT] = flandmark.getNormalizedFrame(Ibw, bbox, gt);
        
        % filter out examples with GT out of flandmark search spaces
        GTNF = GT-1;
        flag = isGTcompatibleWithModel(GTNF, model);
        
        fprintf('%d/%d processing %s... ', i, N, fname);
        
        if flag
            idx = idx + 1;
            GTs{idx} = GT;
            kappas(idx) = computeKappa(GT);
            Images{idx} = Image;
            fprintf(' done. \n');
        else
            fprintf(' not passed. \n');
        end;
        
    end;
    
    Images(idx+1:end) = [];
    GTs(idx+1:end) = [];
    kappas(idx+1:end) = [];

end

