function [ R, subgrad, data ] = risk_mv_b( data, W )
%RISK_MV Summary of this function goes here
%   Detailed explanation goes here
% 
% 2015-07-14, Michal Uricar

    createW = false;
    if nargin < 2
        createW = true;
    end;
    
        
    subgrad = sparse(data.ndim, 1);
    R = 0;

    if createW
        fprintf('W not provided, creating zeros matrix \n');
        W = zeros(data.ndim, 1);
    end;

    % update W
    for phi = 1 : data.PHIS
        from = data.options{phi}.from;
        to = data.options{phi}.to;
        data.flandmark_pool{phi}.setW(W(from:to));
    end;

    % Check concavity of wg (distance transform)
    showInfo = false;
%          fprintf('\n');
    for phi = 1 : data.PHIS
        w_g = data.flandmark_pool{phi}.getWedges();
        dt_enabled = zeros(numel(w_g), 1);
        for a = 1 : numel(w_g)
            if (w_g{a}(3) < 0 && -w_g{a}(3)*w_g{a}(4) < 0)
                dt_enabled(a) = true;
            else
                showInfo = true;
            end;
        end;
        if showInfo
            fprintf('\n{%d} DT enabled ', phi);
            fprintf('%d ', dt_enabled);
            fprintf('\n');
        end;
    end;

%     LP_NFfeatures = getLocalPart(data.NFfeatures);
%     LP_Yaws = getLocalPart(data.Yaws);
%     LP_GTs = getLocalPart(data.GTs);
%     LP_kappas = getLocalPart(data.kappas);
%     LP_psi_gt = getLocalPart(data.psi_gt);
%     if data.precomputeLossTables
%         LP_Lgt = getLocalPart(data.Lgt);
%     end;

    N = numel(data.NFfeatures);

    for i = 1 : numel(data.NFfeatures)
        if data.precomputeLossTables
            [r, psi] = loss_mv_b_flandmark(data.options, data.NFfeatures{i}, data.Yaws{i}, data.GTs{i}, data.Lgt{i}, data.psi_gt{i}, data.model, data.featuresPool, data.flandmark_pool, W);
        else
            [r, psi] = loss_mv_b_flandmark(data.options, data.NFfeatures{i}, data.Yaws{i}, data.GTs{i}, data.kappas(i), data.psi_gt{i}, data.model, data.featuresPool, data.flandmark_pool, W);
        end;
        subgrad = subgrad + psi;
        R = R + r;
    end;
        
    
%     N = sum([N{:}]);
%     R = sum([R{:}])/N;
% %      subgrad = sparse(sum([subgrad{:}], 2)/N);
%     subgrad = sum([subgrad{:}], 2)/N;

    R = R/N;
    subgrad = subgrad/N;

end

