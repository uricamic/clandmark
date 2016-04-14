function [ P, stats, score, QG ] = flandmark_opt_sv_detector( Ibw, bbox, flandmark )
%FLANDMARK_SV_DETECTOR Summary of this function goes here
%   Detailed explanation goes here
% 
% 2015-07-14, Michal Uricar

    % check inputs
    %TODO

    [P, score, QG] = flandmark.detect_optimized(Ibw, bbox);
    stats = flandmark.getTimingsStats();
    
end

