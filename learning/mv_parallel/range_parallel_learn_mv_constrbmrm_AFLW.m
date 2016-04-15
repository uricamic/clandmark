%% parallel_learn_mv_constrbmrm_full_AFLW.m
% 
% 
% 2016-01-15, Michal Uricar 

clc;
close all;
clearvars;

%% Timestamp

fprintf(1,'Started on %s\n\n', datestr(now));

%% User Settings

CLANDMARK_PATH = '/path/to/clandmark/root/directory/';

%  Opt.useCplex = 1;           % whether to use CPLEX for QP (otherwise libQP will be used)
Opt.useCplex = 0;           % whether to use CPLEX for QP (otherwise libQP will be used)
Opt.tolRel   = 1e-2;        % Stopping criterion 1e-2 is usually sufficient 
Opt.bufSize  = 1000;        % The more the better, but keep in mind the memory consumption! (this takes around XX GB)
Opt.saveProgress = true;    % Save progress
Opt.saveAfter = 300;        % Save progress after this number of iterations only

lambdaRange = 10.^[2:-1:-2];	% lambda range for model selection 

precomputeLossTables = false;   % true if enough memory (on datagrid)
% precomputeLossTables = true;

NUM_THREADS = 30;           % number of cores to use 
%  NUM_THREADS = 6;

%% Add paths

addpath('./functions/');

addpath([CLANDMARK_PATH 'matlab_interface/']);
addpath([CLANDMARK_PATH 'matlab_interface/mex/']);
addpath([CLANDMARK_PATH 'matlab_interface/functions/']);
addpath([CLANDMARK_PATH 'learning/bmrm/']);
addpath([CLANDMARK_PATH 'learning/libqp/matlab/']);

addpath('/media/CPLEX/');
addpath('/local/xfrancv/CPLEX_Studio125/cplex/matlab/');
addpath('/datagrid/personal/uricamic/2015-07-12_300W_Learning/CPLEX/');

%% Load data

load('./MAT/data_TRN_SPLIT_1.mat');

% helpers (such as visible subsets, PHIS, views)
load('./MAT/A.mat');

%% Init flandmarkPool

flandmark_pool = cell(PHIS, 1);

FLANDMARK_INIT_FILE = {
    './model/JOINT_MV_SPLIT_1_-PROFILE_init.xml'
    './model/JOINT_MV_SPLIT_1_-HALF-PROFILE_init.xml'
    './model/JOINT_MV_SPLIT_1_FRONTAL_init.xml'
    './model/JOINT_MV_SPLIT_1_HALF-PROFILE_init.xml'
    './model/JOINT_MV_SPLIT_1_PROFILE_init.xml'
};

for phi = 1 : numel(FLANDMARK_INIT_FILE)
    flandmark_pool{phi} = flandmark_class(FLANDMARK_INIT_FILE{phi}, true);
end;

% Feature Pool 
bw = flandmark_pool{1}.getBWsize();
featuresPool = featuresPool_class(bw(1), bw(2));
featuresPool.addLBPSparseFeatures();
for phi = 1 : PHIS
    flandmark_pool{phi}.setFeaturesPool(featuresPool.getHandle);
end;

%% built options and model

options = cell(PHIS, 1);

from = 0;
for phi = 1 : PHIS
    options{phi}.S = flandmark_pool{phi}.getNodesSearchSpaces()+1;
    options{phi}.bw = flandmark_pool{phi}.getBWsize();
    options{phi}.view = A.views{phi};
    options{phi}.M = flandmark_pool{phi}.getLandmarksCount();
    options{phi}.from = from+1;
    options{phi}.dim = flandmark_pool{phi}.getWdim();
    options{phi}.to = from + options{phi}.dim;
    from = options{phi}.to;
    [options{phi}.Woffsets] = flandmark_pool{phi}.getWoffsets();
    options{phi}.nodeWdims = flandmark_pool{phi}.getNodesDimensions();
    options{phi}.edgeWdims = flandmark_pool{phi}.getEdgesDimensions();
end;

ndim = options{end}.to;

model = cell(PHIS, 1);
for phi = 1 : PHIS
    model{phi}.ss = flandmark_pool{phi}.getNodesSearchSpaces();
    model{phi}.components = flandmark_pool{phi}.getNodesWindowSizes();
    model{phi}.bw = flandmark_pool{phi}.getBWsize();
    model{phi}.M = flandmark_pool{phi}.getLandmarksCount();
    model{phi}.Woffsets = flandmark_pool{phi}.getWoffsets();
    model{phi}.nodeWdims = flandmark_pool{phi}.getNodesDimensions();
    model{phi}.edgeWdims = flandmark_pool{phi}.getEdgesDimensions();
    model{phi}.view = A.views{phi};
    model{phi}.L = repmat({ones(model{phi}.bw')}, 1, model{phi}.M);
end;

% clean temporary created flandmark_pool and featuresPool
clear flandmark_pool;
clear featuresPool;

%% parpool

cluster = parcluster('local');
cluster.NumWorkers = NUM_THREADS;
parpool(cluster, cluster.NumWorkers);

%% prepare data (parallel)

N = numel(Images);

spmd
    data.N = N;
    data.PHIS = PHIS;
    data.GTs = codistributed(GTs');
    data.Images = codistributed(Images');
    data.Yaws = codistributed(Yaws');
    data.kappas = codistributed(kappas');
    data.ndim = ndim;
    data.precomputeLossTables = precomputeLossTables;
    data.model = model;
    data.options = options;
    data.type = 'Trn';

    % featuresPool
    data.featuresPool = featuresPool_class(bw(1), bw(2));
    data.featuresPool.addLBPSparseFeatures();

    % flandmark 
    data.flandmark_pool = cell(PHIS, 1);
    for phi = 1 : numel(FLANDMARK_INIT_FILE)
        data.flandmark_pool{phi} = flandmark_class(FLANDMARK_INIT_FILE{phi}, true);
        data.flandmark_pool{phi}.setFeaturesPool(data.featuresPool.getHandle);
    end;
    
    data.dist = codistributor('1d', 1);
    data.NFfeatures = codistributed.cell(N, 1, data.dist);
    data.g_indices = globalIndices(data.NFfeatures, 1);
    data.Lgt = codistributed.cell(N, 1, data.dist);
    data.psi_gt = codistributed.cell(N, 1, data.dist);
end;
    
%% pre-compute features and loss functions 
spmd
    
    LP_Images = getLocalPart(data.Images);
    LP_GTs = getLocalPart(data.GTs);
    LP_kappas = getLocalPart(data.kappas);
    LP_Yaws = getLocalPart(data.Yaws);
    
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
        for phi = 1 : PHIS
            if strcmp(LP_Yaws(i), data.options{phi}.view)
                if ~isGTcompatibleWithModel(LP_GTs{i}-1, data.model{phi})
                    error('GT is not compatible with the model!!!\n');
                end;
                data.featuresPool.setFeaturesRaw(1, LP_NFfeatures{i});
                % pre-compute psi vector
                from = data.options{phi}.from;
                to = data.options{phi}.to;
                psi_gt = data.flandmark_pool{phi}.getPsi_base_optimized(LP_GTs{i});
                LP_psi_gt{i} = sparse(data.ndim, 1);
                LP_psi_gt{i}(from:to) = psi_gt;
                % pre-compute loss tables
                if precomputeLossTables
                    LP_Lgt{i} = compute_table_loss_mv(model{phi}, LP_GTs{i}, LP_kappas(i), 1);
                end;
            end;
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

valdata.flandmark_pool = cell(PHIS, 1);
for phi = 1 : numel(FLANDMARK_INIT_FILE)
    valdata.flandmark_pool{phi} = flandmark_class(FLANDMARK_INIT_FILE{phi});
end;

% Feature Pool 
valdata.featuresPool = featuresPool_class(model{1}.bw(1), model{1}.bw(2));
valdata.featuresPool.addLBPSparseFeatures();
for phi = 1 : PHIS
    valdata.flandmark_pool{phi}.setFeaturesPool(valdata.featuresPool.getHandle);
end;
valdata.GTs = VALGTs;
valdata.Images = VALImages;
valdata.Yaws = VALYaws;
valdata.kappas = VALkappas;
valdata.PHIS = PHIS;
valdata.options = options;
valdata.nImages = numel(VALImages);
valdata.ndim = valdata.options{end}.to;
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

% Prepare bounds and indices
edgeOffsets = {};
for a = 1 : numel(options)
    edgeOffsets{end+1} = options{a}.Woffsets{2};
end;

w3s = [];
w4s = [];
for a = 1 : numel(edgeOffsets)
    w3s = [w3s; options{a}.from+edgeOffsets{a}+2-1];
    w4s = [w4s; options{a}.from+edgeOffsets{a}+3-1];
end;

edgeIndices = [w3s w4s]';
edgeIndices = edgeIndices(:);

nConstr = numel(edgeIndices);
c = 1e-7;
A = zeros(nConstr, ndim);
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
    
    Opt.outName = ['W_BMRMCONSTR_lambda_' num2str(lambda) '.mat'];
    
    if ~exist(Opt.outName, 'file')
        
        %TODO: check for the working file
        if exist(['__progress__' Opt.outName], 'file')
            fprintf('Working file %s found, starting from that point...\n', ['__progress__' Opt.outName]);
            load(['__progress__' Opt.outName]);
            [Ws{i}, Stat, Cpm] = bmrmconstr2(data, @risk_par_mv, lambda, A, b, C, Opt);
        else 
            [Ws{i}, Stat, Cpm] = bmrmconstr2(data, @risk_par_mv, lambda, A, b, Cpm, Opt);
        end;
        W = Ws{i};
        save(Opt.outName, 'W', 'Stat', 'lambda', 'Opt', 'Cpm', '-v7.3');
    else
        fprintf('%s already exists. Loading and continue with next lambda...\n', Opt.outName);
        load(Opt.outName);
        Ws{i} = W;
    end;
    
    % VAL Error
    fprintf('Compute VAL error...\n');
    ValOut = compute_err_mv_flandmark(W, valdata, valdata.flandmark_pool, valdata.featuresPool);
    fprintf('%s: %.2f; %s: %.2f; %s: %.2f \n', ValOut{1}.name, ValOut{1}.value, ValOut{2}.name, ValOut{2}.value, ValOut{3}.name, ValOut{3}.value);
    
end;

%% Save

fprintf('Saving learned vector W...\n');
save('W_BMRMCONSTR_lambdaRange.mat', 'Ws', 'Stat', 'lambdaRange', 'Opt', '-v7.3');

%% Timestamp

fprintf(1,'Finished on %s\n\n', datestr(now));
