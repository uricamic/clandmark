function [ bbox, full_bb1 ] = getUpdatedBBOX( P )
%GETUPDATEDBBOX Summary of this function goes here
%   Detailed explanation goes here

    left_eye = [43, 44, 45, 46, 47, 48];
    right_eye = [37, 38, 39, 40, 41, 42];
    eyes = [right_eye, [28], left_eye];
    mouth = [49:68];
    centerline = [28, 29, 30, 31, 34, 52, 63, 67, 58, 9];
    
    f2 = lsbestline(P(1, eyes)', P(2, eyes)');
    
    LeftEye = [mean(P(1, left_eye)); mean(P(2, left_eye))];
    RightEye = [mean(P(1, right_eye)); mean(P(2, right_eye))];
    Mouth = [mean(P(1, mouth)); mean(P(2, mouth))];

    d = norm(LeftEye-RightEye);
    NP = 0.5*(LeftEye+RightEye);
%     CP = 0.6*NP+0.4*Mouth;
%     CP = [mean(P(1, :)); mean(P(2, :)) ];
    CP = [mean(P(1, 18:end)); mean(P(2, 18:end)) ];
    
    enlarge_coeff = 2.7;
    bb_origin = [-d/2, d/2, d/2, -d/2; -d/2, -d/2, d/2, d/2];
    % full_bb1 = [CP(1)-d/2, CP(1)+d/2, CP(1)+d/2, CP(1)-d/2; CP(2)-d/2, CP(2)-d/2, CP(2)+d/2, CP(2)+d/2];
    full_bb1 = bb_origin + repmat(CP, 1, 4);
    bb_origin = bb_origin*enlarge_coeff;

    X1 = [0; 0];
    X2 = [1; 0];
    vX = X2 - X1;
    A1 = [X1(1); f2(1)+f2(2)*X1(1)];
    A2 = [X2(1); f2(1)+f2(2)*X2(1)];
    vA = A2 - A1;
    ph = atan2(vA(2), vA(1));
%     phi = ph*180/pi;
    RotMat = [cos(ph) -sin(ph) 0; sin(ph) cos(ph) 0; 0 0 1];
    full_bb2 = RotMat*[bb_origin; ones(1, 4)];
    bbox = full_bb2(1:2, :)+repmat(CP, 1, 4);
    
end

