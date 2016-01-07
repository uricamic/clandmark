%% sv_parallel_learn_bmrmconstr.m
% 
% 
% 2015-09-07, Michal Uricar 
% 2016-01-06, Michal Uricar

clc;
close all;
clearvars;

%% Timestamp

fprintf(1,'Started on %s\n\n', datestr(now));

%% User Settings

CLANDMARK_PATH = '/path/to/clandmark/root/directory/';

% Opt.useCplex = 1;           % whether to use CPLEX for QP (otherwise libQP will be used)
Opt.useCplex = 0;           % whether to use CPLEX for QP (otherwise libQP will be used)
Opt.tolRel   = 1e-2;        % Stopping criterion 1e-2 is usually sufficient 
Opt.bufSize  = 1000;        % The more the better, but keep in mind the memory consumption! (this takes around 20GB)
Opt.saveProgress = true;    % Save progress
Opt.saveAfter = 250;        % Save progress after this number of iterations only

% precomputeLossTables = false;   % true if enough memory (on datagrid)
precomputeLossTables = true;

%% Add paths

addpath('./functions/');
addpath([CLANDMARK_PATH 'matlab_interface/']);
addpath([CLANDMARK_PATH 'matlab_interface/mex/']);
addpath([CLANDMARK_PATH 'matlab_interface/functions/']);
addpath([CLANDMARK_PATH 'learning/bmrm/']);
addpath([CLANDMARK_PATH 'learning/libqp/matlab/']);
% addpath('/CPLEX/'); 

%% Load data

load('./MAT/data_TRN_SPLIT_1.mat');

%% Init flandmark & featuresPool

fn = './model/SV_init.xml';

flandmark = flandmark_class(fn, true);

% Feature Pool 
bw = flandmark.getBWsize();
featuresPool = featuresPool_class(bw(1), bw(2));
featuresPool.addLBPSparseFeatures();
flandmark.setFeaturesPool(featuresPool.getHandle);

%% Pre-compute 

model.ss = flandmark.getNodesSearchSpaces();
model.components = flandmark.getNodesWindowSizes();
model.bw = flandmark.getBWsize();
model.M = flandmark.getLandmarksCount();
model.Woffsets = flandmark.getWoffsets();
model.nodeWdims = flandmark.getNodesDimensions();
model.edgeWdims = flandmark.getEdgesDimensions();
model.ndim = flandmark.getWdim();

clear flandmark;
clear featuresPool;

%% parpool

cluster = parcluster('local');
cluster.NumWorkers = 32;
parpool(cluster, cluster.NumWorkers);

%% TEST

N = numel(Images);

%% 

spmd
    data.N = N;
    data.GTs = codistributed(GTs');
    data.Images = codistributed(Images');
    data.kappas = codistributed(kappas');
    
    data.ndim = model.ndim;
    data.model = model;
    data.type = 'Trn';
    data.precomputeLossTables = precomputeLossTables;
    
    data.flandmark = flandmark_class(fn, true);
    data.featuresPool = featuresPool_class(model.bw(1), model.bw(2));
    data.featuresPool.addLBPSparseFeatures();
    data.flandmark.setFeaturesPool(data.featuresPool.getHandle);
    
    data.dist = codistributor('1d', 1);
    data.NFfeatures = codistributed.cell(N, 1, data.dist);
    data.g_indices = globalIndices(data.NFfeatures, 1);
    
    if precomputeLossTables
        data.Lgt = codistributed.cell(N, 1, data.dist);
    end;
    
    data.psi_gt = codistributed.cell(N, 1, data.dist);
    
end;

%% 

spmd
    LP_Images = getLocalPart(data.Images);
    LP_GTs = getLocalPart(data.GTs);
    LP_kappas = getLocalPart(data.kappas);
    
    LP_NFfeatures = getLocalPart(data.NFfeatures);
    dist = getCodistributor(data.NFfeatures);
    
    if precomputeLossTables
        LP_Lgt = getLocalPart(data.Lgt);
    end;
    
    LP_psi_gt = getLocalPart(data.psi_gt);

    for i = 1 : numel(LP_Images)
        % Precompute features
        data.featuresPool.computeFromNF(LP_Images{i}');
        LP_NFfeatures{i} = data.featuresPool.getFeaturesRaw(1);
        if ~isGTcompatibleWithModel(LP_GTs{i}-1, data.model)
            error('GT is not compatible with the model!!!\n');
        end;
        
        % Precompute psi vectors
        LP_psi_gt{i} = sparse(data.flandmark.getPsi_base_optimized(LP_GTs{i}));
        
        % Precompute loss tables
        if precomputeLossTables
            LP_Lgt{i} = compute_loss_sv(data.model, LP_GTs{i}, LP_kappas(i), 1);
        end;
        
    end;
    
    data.NFfeatures = codistributed.build(LP_NFfeatures, dist, 'noCommunication');
    if precomputeLossTables
        data.Lgt = codistributed.build(LP_Lgt, dist, 'noCommunication');
    end;
    data.psi_gt = codistributed.build(LP_psi_gt, dist, 'noCommunication');
end;

%% VAL data

load('./MAT/data_VAL_SPLIT_1.mat');

valdata.flandmark = flandmark_class(fn);

% Feature Pool 
valdata.featuresPool = featuresPool_class(model.bw(1), model.bw(2));
valdata.featuresPool.addLBPSparseFeatures();
valdata.flandmark.setFeaturesPool(valdata.featuresPool.getHandle);
valdata.GTs = VALGTs;
valdata.Images = VALImages;
valdata.kappas = VALkappas;
valdata.nImages = numel(VALImages);
valdata.ndim = valdata.flandmark.getWdim();
valdata.model = model;
valdata.type = 'Val';

lastPrint = 0;
fprintf('Precomputing VAL features...\n');
valdata.NFfeatures = cell(valdata.nImages, 1);
for i = 1 : valdata.nImages
    valdata.featuresPool.computeFromNF(valdata.Images{i}');
    valdata.NFfeatures{i} = valdata.featuresPool.getFeaturesRaw(1);
    
    % Print progress
    if (i >= lastPrint*valdata.nImages/10 || i==valdata.nImages)
        fprintf('%.0f%% ',100*i/valdata.nImages); 
        lastPrint = lastPrint + 1;
    end
end;
fprintf('\n');

%% BMRMCONSTR model selection 

lambdaRange = 10.^[3:-1:1];

% Prepare bounds and indices
edgeOffsets = model.Woffsets{2};

w3s = edgeOffsets+2;
w4s = edgeOffsets+3;

edgeIndices = [w3s w4s]';
edgeIndices = edgeIndices(:);

nConstr = numel(edgeIndices);
c = 1e-7;
A = zeros(nConstr, model.ndim);
for a = 1 : nConstr
    A(a, edgeIndices(a)) = -1;
end;
b = ones(nConstr, 1)*c;

fprintf('Call BMRMCONSTR...\n');

Ws = cell(numel(lambdaRange), 1);
Cpm = [];

for i = 1 : numel(lambdaRange)
    lambda = lambdaRange(i);
    
    fprintf('\n\nLambda = %f \n\n', lambda);
    
    Opt.outName = ['sv_W_PAR_BMRMCONSTR_lambda_' num2str(lambda) '.mat'];
    
    if ~exist(Opt.outName, 'file')
        [Ws{i}, Stat, Cpm] = bmrmconstr2(data, @risk_par_sv_flandmark, lambda, A, b, Cpm, Opt);
        W = Ws{i};
        save(Opt.outName, 'W', 'Stat', 'lambda', 'Opt', 'Cpm', '-v7.3');
    else
        fprintf('%s already exists. Loading and continue with next lambda...\n', fn);
        load(Opt.outName);
    end;
    
    % VAL Error
    fprintf('Compute VAL error...\n');
    ValOut = compute_err_sv_flandmark(W, valdata, valdata.flandmark, valdata.featuresPool);
    fprintf('%s: %.2f \n', ValOut{1}.name, ValOut{1}.value);
    
end;

%% Save

fprintf('Saving learned vector W...\n');
save('sv_W_PAR_BMRMCONSTR_lambdaRange.mat', 'Ws', 'Stat', 'lambdaRange', 'Opt');

%% Timestamp

fprintf(1,'Finished on %s\n\n', datestr(now));
