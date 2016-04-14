function [ relative_overlap, gtbbox ] = getOverlapAABB( gt, p )
%GETOVERLAPAABB Summary of this function goes here
%   Detailed explanation goes here

    gtbbox = double([min(gt(1, :)) min(gt(2, :)) max(gt(1, :)) max(gt(2, :))]);
    gtrect = double([gtbbox(1) gtbbox(2) gtbbox(3)-gtbbox(1)+1 gtbbox(4)-gtbbox(2)+1]);

    aabb = double([min(p(1, :)) min(p(2, :)) max(p(1, :)) max(p(2, :))]);
    aabbrect = double([aabb(1) aabb(2) aabb(3)-aabb(1)+1 aabb(4)-aabb(2)+1]);
    
    unionP = [reshape(gtbbox, 2, 2), reshape(aabb, 2, 2)];
    unionCoords = double([min(unionP(1, :)) min(unionP(2, :)) max(unionP(1, :)) max(unionP(2, :))]);
    
    intersectArea = rectint(gtrect, aabbrect);
    unionArea=(unionCoords(3)-unionCoords(1)+1)*(unionCoords(4)-unionCoords(2)+1);
    relative_overlap = intersectArea/unionArea;

end

