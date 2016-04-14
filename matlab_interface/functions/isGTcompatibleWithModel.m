function [ out ] = isGTcompatibleWithModel( GT, model )
%VALIDATEGTWITHMODEL Summary of this function goes here
%   Detailed explanation goes here

    if size(GT, 2) ~= size(model.ss, 2) || size(GT, 2) ~= size(model.components, 2)
        error('GT and model are not compatible!\n');
    end;
        
    out = true;
    for j = 1 : size(GT, 2)
            
        if ( (GT(1, j) < model.ss(1, j)) || (GT(2, j) < model.ss(2, j)) || ...
             (GT(1, j) > model.ss(3, j)) || (GT(2, j) > model.ss(4, j)) )
            out = false;
            return;
        end;
        
    end;

end
