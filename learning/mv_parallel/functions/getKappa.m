function [ kappa ] = getKappa( GT )
%GETKAPPA_FRONTAL Summary of this function goes here
%   Detailed explanation goes here

    GT = double(GT);

    leftEye = ( GT(:, 7) + GT(:, 8 ) ) / 2;
    rightEye = ( GT(:, 10) + GT(:, 11) ) / 2;
    mouth = ( GT(:, 18) + GT(:, 19) ) / 2;
% %     nose = GT(:, 14);
% 
%     % facebox size
%     S = 0.5*(leftEye+rightEye);
%     kappa = 1/(2.7*( norm(leftEye-rightEye) + norm(S-mouth)*1.12 )*0.5);

    S = 0.5*(leftEye+rightEye);
    kappa = 1/norm(S-mouth); 

end

