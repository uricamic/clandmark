function [ P, view ] = speedup_mv_detector( img, flandmark_pool, featuresPool, detection )
%CLANDMARK_MULTIVIEW_DETECTOR Summary of this function goes here
%   Detailed explanation goes here
    
    bbox = [detection.Position.BoundingBox.TopLeftCol;
          detection.Position.BoundingBox.TopLeftRow;
          detection.Position.BoundingBox.TopRightCol;
          detection.Position.BoundingBox.TopRightRow;
          detection.Position.BoundingBox.BotRightCol;
          detection.Position.BoundingBox.BotRightRow;
          detection.Position.BoundingBox.BotLeftCol;
          detection.Position.BoundingBox.BotLeftRow];
    
    bbox = int32(bbox');
    
    angles = detection.Angles;    
    yaw = angles(end);
    
    if (yaw > -110 && yaw <= -60)
        flandmark = flandmark_pool{1};
        view = '-profile';
    elseif (yaw > -60 && yaw <= -15)
        flandmark = flandmark_pool{2};
        view = '-half-profile';
    elseif (yaw > -15 && yaw <= 15)
        flandmark = flandmark_pool{3};
        view = 'frontal';
    elseif (yaw >= 15 && yaw < 60)
        flandmark = flandmark_pool{4};
        view = 'half-profile';
    elseif (yaw >= 60 && yaw < 110)
        flandmark = flandmark_pool{5};
        view = 'profile';
    else
        view = 'NaN';
        P = [];
        return;
    end;
        
    featuresPool.computeFromNF(flandmark.getNormalizedFrame(img, bbox)');
    P = flandmark.detectOptimizedFromPool(bbox);

end

