%% create_TRN_and_VAL.m
% 
% 
% 2015-09-07, Michal Uricar

clc;
close all;
clearvars;

%% Timestamp 

fprintf(1,'Started on %s\n\n', datestr(now));

%% Select split

SPLIT = 1;

%% Prune & Save TRN

load('./300W_WithBadIndices.mat');

trn_indices = setdiff(trnIdx{SPLIT}, badidx);

F = F(trn_indices);

save(['../MAT/TRN_SPLIT_' num2str(SPLIT) '.mat'], 'F', 'trn_indices');

%% Prune & Save VAL

load('./300W_WithBadIndices.mat');

val_indices = setdiff(valIdx{SPLIT}, badidx);

F = F(val_indices);

save(['../MAT/VAL_SPLIT_' num2str(SPLIT) '.mat'], 'F', 'val_indices');

%% Timestamp 

fprintf(1,'Finished on %s\n\n', datestr(now));
