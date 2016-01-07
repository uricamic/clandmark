function [ kappa ] = computeKappa( GT )
%COMPUTEKAPPA Summary of this function goes here
%   Detailed explanation goes here
% 
% 2015-09-07, Michal Uricar

    GT = double(GT);
    interocular_distance = norm(GT(:, 37)-GT(:, 46));
    kappa = 1/interocular_distance;

end

