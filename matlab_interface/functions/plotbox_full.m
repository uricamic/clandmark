function h=plotbox_full(box,varargin)
% PLOTBOX Plots box(es) to the current figure.
% 
% Synopsis:
%  h=plotbox(box)
%  h=plotbox(box,plot_arguments)
%
% Input:
%  box [4 x numOfBoxes] box coordinates when each column has the format
%    [top_left_col top_left_row bottom_right_col bottom_right_row]
%
%  plot_arguments [...] agruments passed to plot fuction.
%
% Output:
%  h [4 x numOfBoxes] hnadles to lines forming the boxes.
%
%
    
if nargin < 2
    line_style = {'b'};
else
    line_style= varargin;
end

current_ishold = ishold;
hold on;

if min(size(box)) == 1

    h = [0 0 0 0]';
    h(1)=plot([box(1) box(3)],[box(2) box(4)],line_style{:});
    h(2)=plot([box(3) box(5)],[box(4) box(6)],line_style{:});
    h(3)=plot([box(5) box(7)],[box(6) box(8)],line_style{:});
    h(4)=plot([box(7) box(1)],[box(8) box(2)],line_style{:});
else
    if size(box,1) ~= 4
        box = box';
    end
    
    h=[];
    for i=1:size(box,2)
        h = [h plotbox(box(:,i),line_style{:})];
    end
end

if ~current_ishold
    hold off;
end

%EOF


        
    
    