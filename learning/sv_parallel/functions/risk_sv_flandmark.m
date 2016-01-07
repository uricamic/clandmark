function [ R, subgrad, data ] = risk_sv_flandmark( data, W )
%RISK_SV_FLANDMARK Summary of this function goes here
%   Detailed explanation goes here
% 
% 2015-09-07, Michal Uricar

    if nargin < 2
        W = zeros(data.ndim, 1);
    end;

    data.flandmark.setW(W);
    
    N = numel(data.Images);
    
    r = zeros(N, 1);
    subgrad = zeros(numel(W), 1);

    lastPrint = 0;
    
    for i = 1 : N
        [r(i), psi] = loss_sv_flandmark(i, data, W);
        subgrad = subgrad + psi;
        
        % Print progress
        if (i >= lastPrint*N/10 || i==N)
            fprintf('%.0f%% ',100*i/N); 
            lastPrint = lastPrint + 1;
        end
    end;

    R = sum(r) / N;
    subgrad = sparse(subgrad / data.nImages);
    
    % Check concavity of wg (distance transform)
    Woffsets = data.flandmark.getWoffsets();
    dt_enabled = zeros(numel(Woffsets{2}), 1);
    for a = 1 : numel(Woffsets{2})
        w_g = W(Woffsets{2}(a):Woffsets{2}(a)+3);
        if (w_g(3) < 0 && -w_g(3)*w_g(4) < 0)
            dt_enabled(a) = true;
        end;
    end;
    clear Woffsets;
    fprintf(' DT enabled ');
    fprintf('%d ', dt_enabled);
    fprintf('\n');
    
end

