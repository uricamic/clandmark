function [ score, psi, loss ] = loss_mv_b_flandmark( options, NFfeatures, Yaws, GTs, tmp, psi_gt, models, featuresPool, flandmark_pool, W )
%LOSS_MV_FLANDMARK Summary of this function goes here
%   Detailed explanation goes here

    PHIS = numel(flandmark_pool);
    
    y_hats = cell(PHIS, 1);
    scores = nan(PHIS, 1);
    losses = nan(PHIS, 1);
    
    GTphi = 0;
    
    biasFrom = options{end}.to+1;
    biasW = W(biasFrom:end);
    
    % speedup version - set features to featuresPool
    featuresPool.setFeaturesRaw(1, NFfeatures);
    
    for phi = 1 : PHIS
                
        if strcmp(options{phi}.view, Yaws)
%             GT = data.GTs{i};
            GT = GTs;
            GTphi = phi;
            if isa(tmp, 'double')
                L = compute_table_loss_mv(models{phi}, GTs, tmp, 1);
            else
                L = tmp;
            end;
        else
            GT = zeros(2, options{phi}.M);
            L = models{phi}.L;
        end;

        flandmark_pool{phi}.setTableLoss(L);
        
        [y_hats{phi}, scores(phi), ~, lossA] = flandmark_pool{phi}.detect_base_optimizedFromPool(GT);
        
        losses(phi) = sum(lossA);
        
        if phi~=GTphi
            losses(phi) = losses(phi)/options{phi}.M;
        end;
        
    end;
    
    f = scores + losses + biasW;
    
    [maximum, maxidx] = max(f);
    
    from = options{GTphi}.from;
    to = options{GTphi}.to;
    score = maximum - W(from:to)'*flandmark_pool{GTphi}.getPsi(GTs) - biasW(GTphi);

    loss = losses(maxidx);
        
    from = options{maxidx}.from;
    to = options{maxidx}.to;
    psi_i = zeros(numel(W), 1);
    psi_i(from:to) = flandmark_pool{maxidx}.getPsi(y_hats{maxidx});
    psi_i(biasFrom-1+maxidx) = 1;
    psi = psi_i - psi_gt;

end

