function [ P, view ] = speedup_jointmv_detector( img, flandmark_pool, featuresPool, views, bbox )
%JOINTMV_DETECTOR_SPEEDUP Summary of this function goes here
%   Detailed explanation goes here

    if numel(flandmark_pool) ~= numel(views)
        error('Number of flandmark instances in the pool and discretized views must match.');
    end;

    PHIS = numel(flandmark_pool);
    
    D = cell(PHIS, 1);
    S = nan(PHIS, 1);
    
    % update features
    featuresPool.computeFromNF(flandmark_pool{1}.getNormalizedFrame(img, bbox)');
    for phi = 1 : PHIS
        [D{phi}, S(phi)] = flandmark_pool{phi}.detectOptimizedFromPool(bbox);
    end;
    
    [~, maxPhi] = max(S);

    P = D{maxPhi};
    view = views{maxPhi};

end

