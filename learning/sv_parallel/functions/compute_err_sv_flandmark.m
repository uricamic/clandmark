function [ Output ] = compute_err_sv_flandmark( W, data, flandmark, featuresPool )
%COMPUTE_ERR_SV_FLANDMARK Summary of this function goes here
%   Detailed explanation goes here
% 
% 2015-09-07, Michal Uricar

    N = data.nImages;
    M = flandmark.getLandmarksCount();
    
    flandmark.setW(W);

    TrnE = inf(N, 1);
    
    lastPrint = 0;
    for i = 1 : N
        
        GT = data.GTs{i};
        
        featuresPool.setFeaturesRaw(1, data.NFfeatures{i});
        P = flandmark.detect_base_optimizedFromPool();
        
        TrnE(i) = 100 * sum(1/M * data.kappas(i) * sqrt(sum((P-GT).^2)));
        
        % Print progress
        if (i >= lastPrint*N/10 || i== N)
            fprintf('%.0f%% ',100*i/N); 
            lastPrint = lastPrint + 1;
        end
        
    end;
    fprintf('\n');
    
    Output{1}.name = [data.type 'E'];
    Output{1}.value = sum(TrnE)/N;

end

