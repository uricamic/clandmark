function [ score, psi, loss, data ] = loss_sv_flandmark( i, data, W )
%LOSS_SV_FLANDMARK Summary of this function goes here
%   Detailed explanation goes here
% 
% 2015-09-07, Michal Uricar

    % Update W
%     data.flandmark.setW(W);

    % update features
    data.featuresPool.setFeaturesRaw(1, data.NFfeatures{i});
    
    % compute loss 
    GT = data.GTs{i};
    if isfield(data, 'Lgt')
        data.flandmark.setTableLoss(data.Lgt{i});
    else
        data.flandmark.setTableLoss(compute_loss_sv(data.model, data.GTs{i}, data.kappas(i), 1));
    end;
    
    % loss augmented classification
    [y_hat, scores, ~, lossA] = data.flandmark.detect_base_optimizedFromPool(GT);

%     loss = sum(lossA)/data.options.M;
    loss = sum(lossA);
    score = loss + scores - W'*data.psi_gt(:, i);
    
    psi = data.flandmark.getPsi(y_hat) - data.psi_gt(:, i);
    
end

