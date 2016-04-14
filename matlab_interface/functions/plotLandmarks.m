function plotLandmarks( P, bbox, edges )
%PLOTLANDMARKS Summary of this function goes here
%   Detailed explanation goes here
% 
% 2015-07-14, Michal Uricar

    if numel(bbox) > 4
        plotbox_full(bbox, 'color', 'y');
    else 
        plotbox(bbox, 'color', 'y');
    end;
    
    if nargin > 2
        for a = 1 : numel(edges(1, :))
            line([P(1, edges(1, a)) P(1, edges(2, a))], [P(2, edges(1, a)) P(2, edges(2, a))], 'color', 'b');
        end;
    end;

    plot(P(1, :), P(2, :), 'r*');
    
end

