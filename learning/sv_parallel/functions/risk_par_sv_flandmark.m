function [ R, subgrad, data ] = risk_par_sv_flandmark( data, W )
%RISK_PAR_SV_FLANDMARK Summary of this function goes here
%   Detailed explanation goes here
% 
% 2015-09-07, Michal Uricar

    createW = false;
    if nargin < 2
        createW = true;
    end;

    spmd
        subgrad = sparse(data.ndim, 1);
        R = 0;
        
        if createW
            fprintf('W not provided, creating zeros matrix \n');
            W = zeros(data.ndim, 1);
        end;
        
        data.flandmark.setW(W);
        
        % Check concavity of wg (distance transform)
        showInfo = false;
        Woffsets = data.flandmark.getWoffsets();
        dt_enabled = zeros(numel(Woffsets{2}), 1);
        for a = 1 : numel(Woffsets{2})
            w_g = W(Woffsets{2}(a):Woffsets{2}(a)+3);
            if (w_g(3) < 0 && -w_g(3)*w_g(4) < 0)
                dt_enabled(a) = true;
            else
                showInfo = true;
            end;
        end;
        if showInfo
            fprintf('Worker %d: DT enabled ', labindex);
            fprintf('%d ', dt_enabled);     
            fprintf('\n');  
        end;
            
        LP_NFfeatures = getLocalPart(data.NFfeatures);
        
        LP_GTs = getLocalPart(data.GTs);
        LP_kappas = getLocalPart(data.kappas);
        LP_psi_gt = getLocalPart(data.psi_gt);
        
        if data.precomputeLossTables
            LP_Lgt = getLocalPart(data.Lgt);
        end;
        
        N = numel(LP_NFfeatures);
        
        
        for i = 1 : numel(LP_NFfeatures)
            if data.precomputeLossTables
                [r, psi] = loss_par_sv_flandmark(LP_NFfeatures{i}, LP_GTs{i}, LP_Lgt{i}, LP_psi_gt{i}, data.model, data.featuresPool, data.flandmark, W);
            else
                [r, psi] = loss_par_sv_flandmark(LP_NFfeatures{i}, LP_GTs{i}, LP_kappas(i), LP_psi_gt{i}, data.model, data.featuresPool, data.flandmark, W);
            end;
            subgrad = subgrad + psi;
            R = R + r;
        end;

    end;
    
    N = sum([N{:}]);
    R = sum([R{:}])/N;
%     subgrad = sparse(sum([subgrad{:}], 2)/N);
    subgrad = sum([subgrad{:}], 2)/N;
    
end

