function [W,stat]= bmrm(data, risk, lambda, options)
% BMRM Bundle Method for regularized Risk Minimization.
%
% Synopsis:
%  [W,stat]= bmrm(data,risk,lambda)
%  [W,stat]= bmrm(data,risk,lambda,options)
%
% Description:
%  Bundle Method for regularized Risk Minimization (BMRM) is an algorithm 
%  for minimization of function
%
%    F(W) = 0.5*lambda*W'*W + R(data,W)
%
%  where R(data,W) is an arbitrary convex function of W. BMRM requires 
%  a function which evaluates R and computes its subgradient at W. 
%  A handle to this function is expected as the second input argument
%  "risk". The calling syntax is
%
%    [Fval,subgrad,data] = risk(data)
%    [Fval,subgrad,data] = risk(data,W)
%
%  which returns function value and subgradient of R(data,W) evaluated 
%  at W. Calling risk(data) without the argument W assumes that 
%  W equals zero vector (this is used to by BMRM to get dimension of
%  parameter vector). The argument data is passed to allow the risk 
%  function to store some auxciliary result if needed. 
%  
% Reference: 
%    Teo et al.: A Scalable Modular Convex Solver for Regularized Risk 
%    Minimization, KDD, 2007
%  
% Inputs:
%  data ... arbitrary data type.
%  risk ... handle of the risk function.
%  lambda [1x1] Regularization parameter.
%
%  options [struct] 
%   .TolRel [1x1] Relative tolerance (default 1e-3). Halt optimization
%      if Fp-Fd <= TolRel*Fp holds.
%   .TolAbs [1x1] Absolute tolerance (default 0). Halt if Fp-Fd <= TolAbs
%      where Fp = F(W) and Fd is a lower bound of F(W_optimal). 
%   .MaxIter [1x1] Maximal number of iterations (default inf). Halt 
%      optimization if nIter >= MaxIter .
%   .verb [1x1] if 1 print progress status (default 0).
%   .BufSize [1x1] Size of cutting plane buffer in MB (default 100).
%   .CleanICP [logical] Flag, which turns on/off the inactiveCP removal
%   .CleanAfter [1x1] Number of iterations after which an inactiveCP will
%      be removed.
% 
% Outputs:
%  W [nDim x 1] Solution vector.
%  stat [struct] 
%   .Fp [1x1] Primal objective value.
%   .Fd [1x1] Reduced (dual) objective value.
%   .nIter [1x1] Number of iterations.
%   .hist [struct]
%       .Fp [1 x nIter] Primal objective values
%       .Fd [1 x nIter] Reduced (dual) objective values
%       .R [1 x nIter]  Risk values
%       .runtime [1 x nIter] Time spent on individual iterations
%       .wdist [1 x nIter] Euclidean distance of consecutive vectors W
%   .timings [struct]
%       .r [1 x nIter] Time spent on risk computation
%       .subgrad [1 x nIter] Time spent on subgradient computation
%       .hessian [1 x nIter] Time spent on Hessian matrix computation
%       .qp [1 x nIter] Time spent on computation of QP task
%       .w [1 x nIter] Time spent on computation of W
%       .overall [1 x nIter] Time spent on whole iteration

% 2010-04-12, Vojtech Franc
% 2012-27-01, Michal Uricar, inactiveCP removal
    
t0 = cputime;
% tic
% clck0_ = clock;

if nargin < 4, options = []; end
if ~isfield(options,'TolRel'), options.TolRel = 1e-3; end
if ~isfield(options,'TolAbs'), options.TolAbs = 0; end
if ~isfield(options,'MaxIter'), options.MaxIter = inf; end
if ~isfield(options,'verb'), options.verb = 0; end
if ~isfield(options,'BufSize'), options.BufSize = 100; end
if ~isfield(options,'BufSizeCP'), options.BufSizeCP = 0; end
if ~isfield(options,'CleanBuffer'), options.CleanBuffer = inf; end

% inactiveCP removal options fields
if ~isfield(options, 'CleanICP'), opts.CleanICP = false; end;
if ~isfield(options, 'CleanAfter'), opts.CleanAfter = 10; end;

if isfield(options, 'initW')
    hasInitW = true;
else
    hasInitW = false;
end;

% rtime = 0; 
% subgradtime = 0;
% qptime = 0; 
% hesstime = 0; 
% wtime = 0;

% clck0_ = clock;
%-------------------------------------------------------

%% Risk and subgradient computation
tmp_time = cputime;
% [R, subgrad, data, risktim] = risk(data);
[R, subgrad, data] = risk(data);
% rtime = rtime + risktim.rtime;
% subgradtime = subgradtime + risktim.subgradtime;
risktime1 = cputime-tmp_time;

%% if subgradient at w=0 is also zero vector then the optimum solution 
% is w=0 and the optimal value is R(0).
if all(subgrad==0)
    W = zeros(size(subgrad));
    stat = [];
    stat.Fp = R;
    stat.Fd = R;
    stat.nIter = 0;
    stat.hist.Fp = R;
    stat.hist.Fd = R;
    stat.hist.R = R;
    stat.hist.qptime = 0;
    stat.hist.risktime = risktime1;
    stat.hist.hessiantime = 0;
    stat.hist.innerlooptime = 0;
    stat.hist.wtime = 0;
    stat.hist.runtime = cputime-t0;
    return;
end

% get paramater space dimension
nDim = length(subgrad);

% computes number of cutting planes from given mega bytes
nCutPlanes = (-nDim*8 + sqrt((nDim*8)^2 +4*8*1024^2 * options.BufSize))/16;
options.BufSize = round(nCutPlanes);

if options.BufSize < options.BufSizeCP 
    options.BufSize = options.BufSizeCP;
end

% inital solution
% W = zeros(nDim,1);
if hasInitW
    W = options.initW;
else
    W = zeros(nDim,1);
end;

prevw = W;

if ~issparse(subgrad)
    A = zeros(nDim,options.BufSize);
else
    A = sparse(nDim,options.BufSize);
end
b = zeros(options.BufSize,1);
H = zeros(options.BufSize,options.BufSize);

A(:,1) = subgrad;
b(1) = R;
beta = [];

nIter = 0;
nCP = 0;
exitflag= -1;

CPcounter = [];

% alloc buffers for meassured statistics
hist_Fd = zeros(1, options.BufSize+1);
hist_Fp = zeros(1, options.BufSize+1);
hist_R = zeros(1, options.BufSize+1);
hist_runtime = zeros(1, options.BufSize+1);
hist_risktime = zeros(options.BufSize+1,1);
hist_qptime = zeros(options.BufSize+1,1);
hist_hessiantime = zeros(options.BufSize+1,1);
hist_innerlooptime = zeros(options.BufSize+1,1);
hist_wtime = zeros(options.BufSize+1,1);

hist_risktime(1) = risktime1;
hist_runtime(1) = cputime-t0;
hist_Fd(1) = -inf;
hist_Fp(1) = R+0.5*lambda*norm(W)^2;
hist_R(1) = R;

% clck1_ = clock;
% hist_runtime(1) = etime(clck1_, clck0_);

% timing stats
% hist_rtime(1) = rtime;
% hist_subgradtime(1) = subgradtime;
% hist_qptime(1) = qptime;
% hist_hesstime(1) = hesstime;
% hist_wtime(1) = wtime;

% wdists stats
hist_wdists(1) = norm(prevw - W);

if options.verb
    fprintf('%4d: tim=%.3f, Fp=%f, Fd=%f, R=%f\n', ...
        nIter, hist_runtime(1), hist_Fp(1), hist_Fd(1), hist_R(1));
end

while exitflag == -1
    iterstart_time = cputime;
    
%     clck0 = clock;
    nIter = nIter + 1;
    nCP = nCP + 1;
    
    %% Hessian computation
%     clock_s = clock;
    tmp_time = cputime;
    if nCP > 1,
        H(1:nCP-1,nCP) = full(A(:,1:nCP-1)'*A(:,nCP))/lambda;
        H(nCP,1:nCP-1) = H(1:nCP-1,nCP)';
    end
    H(nCP,nCP) = full(A(:,nCP)'*A(:,nCP))/lambda;
%     clock_f = clock;
%     hesstime = etime(clock_f, clock_s);
    hist_hessiantime(nIter+1) = cputime-tmp_time;
        
    %% QP solver
%     clock_s = clock;
    % solve reduced problem
    tmp_time = cputime;
    if all(b(1:nCP)==0)
        % if b is zero vector then the optimum is also zero vector => QP
        % doesn't need to be called
        beta = zeros(nCP,1);
        stat.QP = 0;
    else
        [beta, stat] = libqp_splx(H(1:nCP,1:nCP),-b(1:nCP),1,ones(1,nCP),1,[beta;0]);
    end;
    hist_qptime(nIter+1) = cputime-tmp_time;
%     clock_f = clock;
%     qptime = etime(clock_f, clock_s);

    % update CPCounter - add one unused CPs and reset used CPs
    CPcounter = [CPcounter+1; 1];
    CPcounter(beta > 0) = 0;

    %% W update
%     clock_s = clock;
    tmp_time = cputime;
    W = -A(:,1:nCP)*beta/lambda;
    hist_wtime(nIter+1) = cputime - tmp_time;
%     clock_f = clock;
%     wtime = etime(clock_f, clock_s);

    nzA = sum(beta > 0);
        
    %% Risk ans dubgradient computation
%     rtime = 0; subgradtime = 0;
    tmp_time = cputime;
%     [R, subgrad, data, risktim] = risk(data,W);
    [R, subgrad, data] = risk(data,W);
    hist_risktime(nIter+1) = cputime-tmp_time;
%     rtime = rtime + risktim.rtime;
%     subgradtime = subgradtime + risktim.subgradtime;
        
    A(:,nCP+1) = subgrad;
    b(nCP+1) = R - A(:,nCP+1)'*W;
    
    Fp = R+0.5*lambda*norm(W)^2;
    Fd = -stat.QP;
                  
    if Fp-Fd<= options.TolRel*abs(Fp)
        exitflag= 1;
    elseif Fp-Fd <= options.TolAbs
        exitflag= 2;    
    elseif nIter >= options.MaxIter
        exitflag= 0;
    end 
    
%     clck1 = clock;
%     hist_runtime(nIter+1) = etime(clck1, clck0);
    hist_runtime(nIter+1) = cputime-t0;
    hist_Fp(nIter+1) = Fp;
    hist_Fd(nIter+1) = Fd;
    hist_R(nIter+1) = R;
    
    % timing stats
%     hist_rtime(nIter+1) = rtime;
%     hist_subgradtime(nIter+1) = subgradtime;
%     hist_qptime(nIter+1) = qptime;
%     hist_hesstime(nIter+1) = hesstime;
%     hist_wtime(nIter+1) = wtime;
    % wdists stats
    wdist = norm(prevw - W);
    hist_wdists(nIter+1) = wdist;

    %% keep w_t and change alpha
    prevw = W;
    
    %%
    
    hist_innerlooptime(nIter+1) = cputime-iterstart_time;
    
    if mod(nIter,options.verb) == 0 || exitflag ~= -1
        fprintf('%4d: tim=%.3f, Fp=%f, Fd=%f, (Fp-Fd)=%f, (Fp-Fd)/Fp=%f, R=%f, nCP=%d, nzA=%d, timinner=%.3f, timrisk=%.3f, timw=%.3f, timqp=%.3f, timhes=%.3f\n', ...
                nIter, hist_runtime(nIter+1), Fp, Fd, Fp-Fd,(Fp-Fd)/Fp, R, nCP, nzA, hist_innerlooptime(nIter+1), hist_risktime(nIter+1), hist_wtime(nIter+1), hist_qptime(nIter+1),hist_hessiantime(nIter+1));
    end
    
    % Save W (to have at least partial solution)
%     save([data.OUTPUT_DIR 'W_lambda_' num2str(lambda) '.mat'], 'W');
    if isfield(data, 'outWname')
        save(['_progress_' data.outWname], 'W');
    else
        save(['_progress_W_lambda_' num2str(lambda) '.mat'], 'W');
    end;
    
    % clean buffer
    if mod(nIter,options.CleanBuffer) == 0 && nzA < nCP
        old_nCP = nCP;
        idx = find(beta > 0);
        nCP = length(idx);

        beta = beta(idx);
        H(1:nCP,1:nCP) = H(idx,idx);
        A(:,1:nCP) = A(:,idx);        
        b(1:nCP) = b(idx);        
        
        fprintf('done. (old_nCP=%d, new_nCP=%d) \n',old_nCP, nCP);
    end
    
    % inactiveCP removal
    if options.CleanICP
        iCP = find(CPcounter>=options.CleanAfter);
        beta(iCP) = [];
        CPcounter(iCP) = [];
        A(:, iCP) = [];
        b(iCP) = [];
        nCP = nCP - length(iCP);
        H(:, iCP) = []; H(iCP, :) = [];
    end;
    
end

stat = [];
stat.Fp = Fp;
stat.Fd = Fd;
stat.nIter = nIter;
stat.hist.Fp = hist_Fp(1:nIter+1);
stat.hist.Fd = hist_Fd(1:nIter+1);
stat.hist.R = hist_R(1:nIter+1);
stat.hist.runtime = hist_runtime(1:nIter+1);

stat.hist.qptime = hist_qptime(1:nIter+1);
stat.hist.risktime = hist_risktime(1:nIter+1);
stat.hist.hessiantime = hist_hessiantime(1:nIter+1);
stat.hist.innerlooptime = hist_innerlooptime(1:nIter+1);
stat.hist.wtime = hist_wtime(1:nIter+1);

% timing stats
% stat.timings.r = hist_rtime(1:nIter+1);
% stat.timings.subgrad = hist_subgradtime(1:nIter+1);
% stat.timings.hessian = hist_hesstime(1:nIter+1);
% stat.timings.qp = hist_qptime(1:nIter+1);
% stat.timings.w = hist_wtime(1:nIter+1);
% stat.timings.overall = hist_runtime(1:nIter+1);

% euclidean distance of consecutive w_{t-1} and w_t
stat.hist.wdist = hist_wdists(1:nIter+1);

return;
