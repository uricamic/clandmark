function [ Images, GTs, Yaws, kappas ] = jointmv_prepareImagesGT( flandmark, models, A, F, image_path, mirroring )
%PREPARETRAININGDATA Summary of this function goes here
%   Detailed explanation goes here

    if nargin < 6
	mirroring = false;
	A.viewsMirrored = flip(A.views, 1);
    end;

    N = numel(F);
    Images = cell(N, 1);
    GTs = cell(N, 1);
    Yaws = cell(N, 1);
    kappas = nan(N, 1);
    idx = 0;
        
    for i = 1 : N
        
        fname = F{i}.imgPath;
        I = imread([image_path fname]);
        try
            Ibw = rgb2gray(I);
        catch ex
            Ibw = I;
        end;
        
        gt = F{i}.landmarks;
        gtphi = find(strcmp(F{i}.view, A.views));
        if mirroring
	  gtphiMirrored = find(strcmp(A.viewsMirrored(gtphi), A.views)); 
        end;
        gtyaw = F{i}.view;
        bbox = int32(F{i}.bbox);
        
        % all detectors in the pool have the same parameters to get the
        % normalized frame => use frontal detector here, which transforms
        % all landmarks
        [NF, GT] = flandmark.getNormalizedFrame(Ibw, bbox, gt);
        
        % filter out examples with GT out of flandmark search spaces
        GTNF = GT-1;
        flag = isGTcompatibleWithModel(GTNF(:, A.visible_subset{gtphi}), models{gtphi});
        
        fprintf('%d/%d processing %s... ', i, N, fname);
        
%         %%% DEBUG - VISUALIZATION
%         figure(1); clf(1); 
%         subplot(121);
%         imshow(I, []); hold on;
%         plot(gt(1, A.visible_subset{gtphi}), gt(2, A.visible_subset{gtphi}), 'gx');
%         subplot(122);
%         imshow(Image, []); hold on;
%         plot(GT(1, A.visible_subset{gtphi}), GT(2, A.visible_subset{gtphi}), 'gx');
%         %%%%%%%%%%%%%%%%%%%%%%%%%
        
        if flag
            idx = idx + 1;
            GTs{idx} = GT(:, A.visible_subset{gtphi});
            kappas(idx) = 1/norm(double(GT(:, 9))-double(GT(:, 21)));   % normalize to vertical face size
            Images{idx} = NF;
            Yaws{idx} = gtyaw;
            fprintf(' done. \n');
        else
            fprintf(' not passed. \n');
        end;
        
        % mirroring
        if mirroring
	    NF_mirrored = fliplr(NF);
            GT_mirrored = GT;
            GT_mirrored(1, :) = size(NF, 1) - GT_mirrored(1, :) + 1;
            
            GTNF_mirrored = GT_mirrored-1;
            flag = isGTcompatibleWithModel(GTNF_mirrored(:, A.visible_subset{gtphiMirrored}), models{gtphiMirrored});
            if flag
	      idx = idx + 1;
	      GTs{idx} = GT_mirrored(:, A.visible_subset{gtphiMirrored});
	      kappas(idx) = 1/norm(double(GT_mirrored(:, 9))-double(GT_mirrored(:, 21)));   % normalize to vertical face size
	      Images{idx} = NF;
	      Yaws{idx} = gtyaw;
	      fprintf(' mirrored done. \n');
	    end;
        end;
        
    end;
    
    Images(idx+1:end) = [];
    GTs(idx+1:end) = [];
    Yaws(idx+1:end) = [];
    kappas(idx+1:end) = [];

end

