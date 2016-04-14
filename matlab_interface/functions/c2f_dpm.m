function [ P, refined_bbox, Stats ] = c2f_dpm( Ibw, bbox1, cdpm_flandmark, fdpm_flandmark )
%C2F_DPM Summary of this function goes here
%   Detailed explanation goes here

    % Detect landmarks - Coarse
    [Pcoarse, Stats1, score1, QG1] = flandmark_opt_sv_detector(Ibw, int32(bbox1), cdpm_flandmark);
    
    % construct ideal bbox from GT
    bbox2 = getUpdatedBBOX(Pcoarse);
    
    % Detect landmarks - Fine
    [P, Stats2, score2, QG2] = flandmark_opt_sv_detector(Ibw, int32(bbox2(:)), fdpm_flandmark);

    Stats = [];
    Stats.CDPM = Stats1;
    Stats.FDPM = Stats2;
    Stats.score_CDPM = score1;
    Stats.score_FDPM = score2;
    Stats.QG_CDPM = QG1;
    Stats.QG_FDPM = QG2;
    
    refined_bbox = getUpdatedBBOX(P);
    
end

