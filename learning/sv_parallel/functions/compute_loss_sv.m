function [ L ] = compute_loss_sv( model, GT, kappa, bound )
%COMPUTE_LOSS_SV Summary of this function goes here
%   Detailed explanation goes here
% 
% 2015-09-07, Michal Uricar

    L = cell(1, model.M);
    
    for comp = 1 : model.M
        % size of component's bbox
        siz = [model.ss(4, comp)-model.ss(2, comp)+1; model.ss(3, comp)-model.ss(1, comp)+1];
        % offset (top left corner of component's bbox)
        offset = model.ss(1:2, comp);

        cor_sqr = zeros(2, siz(2)*siz(1));

        for i = 1 : siz(2)
            % x coordinates of (S_i - S_i*)^2
            cor_sqr(1, (i-1)*siz(1)+1:(i*siz(1))) = (i-1+offset(1) - GT(1, comp)).^2;
            % y coordinates of (S_i - S_i*)^2
            cor_sqr(2, (i-1)*siz(1)+1:(i*siz(1))) = (offset(2) - GT(2, comp):offset(2)+siz(1)-1 - GT(2, comp)).^2;
        end;
        % L(S, S*) = 1/M \sum_{i=0}^{M-1} || S_i - S_i* ||
        ell = bound*kappa/model.M * sqrt( cor_sqr(1,:) + cor_sqr(2,:) );
        
        L{comp} = bound*ones(model.bw(2), model.bw(1));
        L{comp}(model.ss(2, comp)+1:model.ss(4, comp)+1, model.ss(1, comp)+1:model.ss(3, comp)+1) = reshape(ell, siz');
    end;

end

