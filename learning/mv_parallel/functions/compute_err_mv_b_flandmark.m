function [ Output ] = compute_err_mv_b_flandmark( W, data, flandmark_pool, featuresPool )
%COMPUTE_ERR_MV_FLANDMARK Summary of this function goes here
%   Detailed explanation goes here

    N = data.nImages;
    PHIS = data.PHIS;
    
    for phi = 1 : PHIS
        from = data.options{phi}.from;
        to = data.options{phi}.to;
        flandmark_pool{phi}.setW(W(from:to));
    end;

    biasFrom = data.options{end}.to+1;
    if biasFrom > numel(W)
        biasW = zeros(PHIS, 1);
    else
        biasW = W(biasFrom:end);
    end;
    
%     E = inf(N, 1);
    E = 0;
    correctYaw = false(data.nImages, 1);
    ErrOnCorrYaw = inf(data.nImages, 1);
    y = cell(PHIS, 1);
    S = nan(PHIS, 1);
    
    lastPrint = 0;
    for i = 1 : N
        
        GT = data.GTs{i};
        
        featuresPool.setFeaturesRaw(1, data.NFfeatures{i});
%         P = flandmark.detect_base_optimizedFromPool();
        for phi = 1 : PHIS
            [y{phi}, S(phi)] = flandmark_pool{phi}.detect_base_optimizedFromPool();
            if strcmp(data.options{phi}.view, data.Yaws{i})
                GT = data.GTs{i};
                GTphi = phi;
            end;
        end;
        S = S + biasW;
        [~, maxPhi] = max(S);
        P = y{maxPhi};
        
%         E(i) = 100 * sum(1/M * data.kappas(i) * sqrt(sum((P-GT).^2)));
        if (GTphi == maxPhi)
            ErrOnCorrYaw(i) = 100 * sum(1/numel(P(1, :)) * data.kappas(i) * sqrt(sum((P-GT).^2)));
            E = E + ErrOnCorrYaw(i);
            correctYaw(i) = true;
        else
            ErrOnCorrYaw(i) = inf;
            E = E + 100;
            correctYaw(i) = false;
        end;
        
        % Print progress
        if (i >= lastPrint*N/10 || i== N)
            fprintf('%.0f%% ',100*i/N); 
            lastPrint = lastPrint + 1;
        end
        
    end;
    fprintf('\n');
    
    Output{1}.name = [data.type 'E'];
    Output{1}.value = E/N;
    
    Output{2}.name = [data.type 'YawMissPerc'];
    Output{2}.value = sum(~correctYaw)/N*100;
    
    Output{3}.name = [data.type 'ErrOnCorrYaw'];
    Output{3}.value = mean(ErrOnCorrYaw(correctYaw));
    

end

