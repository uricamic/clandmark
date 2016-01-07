function [ score, psi, loss ] = loss_par_sv_flandmark( NFfeatures, GT, tmp, psi_gt, model, featuresPool, flandmark, W )
%LOSS_PAR_SV_FLANDMARK Summary of this function goes here
%   Detailed explanation goes here
% 
% 2015-09-07, Michal Uricar

%     % Update W
%     data.flandmark.setW(W);

    % update features
    featuresPool.setFeaturesRaw(1, NFfeatures);
    
    % compute loss  (tmp is either kappa or loss_table)
    if isa(tmp, 'double')
        flandmark.setTableLoss(compute_loss_sv(model, GT, tmp, 1));
    else
        flandmark.setTableLoss(tmp);
    end;
    
    % loss augmented classification
    [y_hat, scores, ~, lossA] = flandmark.detect_base_optimizedFromPool(GT);

    loss = sum(lossA);
    score = loss + scores - W'*psi_gt;
    
    psi = flandmark.getPsi(y_hat) - psi_gt;
    
end

