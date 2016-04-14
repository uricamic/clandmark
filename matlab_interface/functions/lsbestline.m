function [ f ] = lsbestline( x, y )
%LSBESTLINE Summary of this function goes here
%   Detailed explanation goes here

    f = nan(2, 1);

    mx = mean(x);
    my = mean(y);
    f(2) = ((x-mx)'*(y-my))/sum((x-mx).^2);
    f(1) = my - f(2)*mx;

end