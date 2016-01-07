function [W, S, C]= bmrmconstr2(Data,risk,lambda, Aineq, bineq, C, Opt)
% BMRMCONSTR Bundle Method for regularized Risk Minimization.
%
% Synopsis: 
%  [W,Stat,Cpm] = bmrmconstr( Data, risk, lambda, Aineq, bineq, )
%  [W,Stat,Cpm] = bmrmconstr( Data, risk, lambda, Aineq, bineq, Cpm )
%  [W,Stat,Cpm] = bmrmconstr( Data, risk, lambda, Aineq, bineq, Cpm, Options )
%
% Description:
%  This function minimizes a quadraticaly regularized convex risk, i.e.:
%
%    Fp(W) = 0.5*lambda*W'*W + risk( Data, W )
%
%  subject to
%
%    Aineq*W >= bineq
%
%  where risk(data,W) is an arbitrary convex function of W. BMRMCONSTR requires 
%  a function to evaluate R and its subgradient at W. This function is 
%  expected as the second argument "risk". The calling syntax is
%
%    [ Fval, subgrad ] = risk( Data)
%    [ Fval, subgrad ] = risk( Data, W )
%
%  which returns function value and subgradient evaluated at W. 
%  Calling risk( Data) without the argument W assumes that W equals to 
%  zero vector which is used by BMRMCONSTR to get dimension of parameter 
%  vector. 
%
%  This algorithm is a slight modification of Teo et al.: A Scalable Modular 
%  Convex Solver for Regularized Risk Minimization, KDD, 2007
%  
% Inputs:
%  Data   [anything]       Data.
%  risk   [function]       Risk function (function pointer).
%  lambda [1x1]            Regularization parameter.
%  Aineq  [nConstr x nDim] linear constraitns
%  bineq  [nConstr x 1]
%  Cpm    [struct]         Initial cutting plane model (default []).
%
%  Opt [struct] 
%   .tolRel  [1x1]  Relative tolerance (default 1e-3). The solver halts 
%                   if Fp-Fd <= tolRel*Fp holds where Fd is the lower bound
%                   of Fp(W_optimal).
%   .tolAbs  [1x1]  Absolute tolerance (default 0). The solver halts 
%                   if Fp-Fd <= tolAbs.
%   .maxIter [1x1]  Maximal number of iterations (default inf). The solver 
%                   halts if nIter >= maxIter .
%   .bufSize [1x1]  Allocate memory for bufSize cutting planes (default 500).
%   .useCplex [1x1] If 1 use CPLEXQP solver otherwise LIBQP_CPLX is used 
%                   (default 0). 
%   .verb    [1x1]  if 1 print progress status (default 1).
% 
% Outputs:
%  W [nDim x 1] Solution vector.
%  Stat [struct] 
%   .Fp    [1x1] Primal objective value.
%   .Fd    [1x1] Reduced (dual) objective value.
%   .nIter [1x1] Number of iterations.
%
%

% 2015-07-12, MU (time measurements that works with threads)
% 2015-06-25, Michal Uricar (bufSize fix + store intermediate solution after each iteration)
% 2015-06-20, VF
% 2010-04-12, Vojtech Franc
    
    %%
%     startTime = cputime;
    startTime = clock;

    % default options
    if nargin < 7, Opt = []; end
    if ~isfield(Opt,'tolRel'),    Opt.tolRel   = 1e-3; end
    if ~isfield(Opt,'tolAbs'),    Opt.tolAbs   = 0; end
    if ~isfield(Opt,'maxIter'),   Opt.maxIter  = inf; end
    if ~isfield(Opt,'verb'),      Opt.verb     = 1; end
    if ~isfield(Opt,'bufSize'),   Opt.bufSize  = 500; end
    if ~isfield(Opt,'useCplex'),  Opt.useCplex = 0; end
    
    if ~isfield(Opt,'saveProgress'),Opt.saveProgress = true; end
    if ~isfield(Opt,'outName'),     Opt.outName      = 'W_CONSTRBMRM.mat'; end
    if ~isfield(Opt,'saveAfter'),   Opt.saveAfter    = 10; end

    %% "S"state variables
    S          = [];
    S.Fp       = nan;
    S.Fd       = nan;
    S.risk     = nan;
    S.nIter    = 0;
    S.nP       = 0;     % number of primal variables
    S.nD       = 0;     % number of dual variables
    S.exitflag = -1;
    S.timing.qpsolver = 0;
    S.timing.risk     = 0;
    S.timing.hessian  = 0;
    S.timing.wtime    = 0;
%     S.timing.runtime  = cputime-startTime;
    S.timing.runtime  = etime(clock, startTime);

    %% use no constraints by default
    if nargin < 5
        Aineq = [];
        bineq = [];
    end

    %% start from given cutting plane model or if not given allocate memory for (NOTE: Here should be possibility to enlarge the capacity of cutting plane model if BufSize allows)
    % a new one and compute the first CP at W=0
    if nargin >= 6 & ~isempty( C )

        S.nP = size( C.A, 1);
        S.nD = C.nCp + C.nConstr;

    else

        %% 
        S.nIter    = 1;

        %% compute risk and its subgradient at zero
%         t0               = cputime;
        t0               = clock;
        [S.risk,subgrad] = risk( Data );
%         S.timing.risk    = cputime-t0;
        S.timing.risk    = etime(clock, t0);
        S.Fp             = S.risk;
        S.nP             = length(subgrad);

        %% check if subgradient at W=0 is a zero vector in which case the 
        % solution is trivial, i.e. W = 0; 
        if all( subgrad ==0 ), 
            S.exitflag = 1;
            W          = zeros( S.nP, 1 ); 
            return; 
        end

        %% get number of constraints
        if isempty( Aineq )
            C.nConstr = 0;
        else
            C.nConstr = size( Aineq,1);
            if size( Aineq, 2 ) ~= S.nP | length( bineq ) ~= C.nConstr
                error('Arguments Aineq and bineq have wrong dimensions.');
            end
            
            % adjust bufSize appropriately 
            Opt.bufSize = Opt.bufSize + C.nConstr;
        end

        %% allocate memory for cutting planes and for hessian
        if issparse(subgrad), 
            C.A = sparse(S.nP, Opt.bufSize);
        else
            C.A = zeros(S.nP, Opt.bufSize);
        end
        C.b = zeros( Opt.bufSize, 1);
        C.H = zeros( Opt.bufSize, Opt.bufSize);

        if C.nConstr > 0
%             C.A(:,1:C.nConstr) = Aineq';
            C.A(:,1:C.nConstr) = -Aineq';
            C.b(1:C.nConstr)   = bineq;
        end
        C.A(:,C.nConstr+1) = subgrad;
        C.b(C.nConstr+1)   = S.risk         % - W'*subgrad;
        C.nCp              = 1;

        S.nD               = C.nCp + C.nConstr;
%         t0                 = cputime;
        t0                 = clock;
%         C.H(1:S.nD,1:S.nD) = C.A(:,1:S.nD)'*C.A(:,1:S.nD);      % slow as hell
%         C.H(1:S.nD,1:S.nD) = multATB(C.A, C.A, S.nD, S.nD, S.np, 1.0, 0); %C.A(:,1:S.nD)'*C.A(:,1:S.nD);
        C.H(1:S.nD, 1:S.nD) = mxMultATB(C.A, C.A, S.nD, S.nD, S.nP, 1.0, 0);
%         S.timing.hessian   = cputime - t0;
        S.timing.hessian   = etime(clock, t0);
    end

    %%
    if Opt.verb
        fprintf('Cutting plane buffer : %d (%fMB)\n', Opt.bufSize, Opt.bufSize*S.nP*8/1024^2);
        fprintf('Hessian buffer       : %f MB\n'    , Opt.bufSize^2 * 8/1024^2);
        fprintf('Number of constraints: %d\n'       , C.nConstr );
        fprintf('Number of weights    : %d\n'       , S.nP );

        fprintf('%4d: tim=%.3f, Fp=%f, Fd=%f, R=%f\n', ...
                S.nIter, S.timing.runtime(end), S.Fp(end), S.Fd(end), S.risk(end) );
    end


    %% main loop
    alpha = [];
    while S.exitflag == -1

        %%
        S.nIter = S.nIter + 1;

        %% solve reduced problem
%         t0 = cputime;
        t0 = clock;

        % if b is zero vector then the optimum is also zero vector => QP
        % doesn't need to be called
        if all( C.b(1:S.nD) == 0 )
            alpha = zeros( S.nD,1);
            Fd    = 0;
        else

            if Opt.useCplex
                Aeq           = [zeros(1,C.nConstr) ones(1,C.nCp)];
                beq           = 1/sqrt(lambda);
                lb            = zeros( S.nD, 1 );
                f             = -C.b(1:S.nD)*sqrt(lambda);
%                 f             = C.b(1:S.nD)*(-sqrt(lambda));
                [alpha, fval] = cplexqp( C.H(1:S.nD,1:S.nD), f, [], [], Aeq, beq, lb );
                Fd            = -fval;
            else
                
                f            = -C.b(1:S.nD)*sqrt(lambda);
%                 f            = C.b(1:S.nD)*(-sqrt(lambda));
                b            = [1e12*ones(1,C.nConstr) 1/sqrt(lambda)];
                I            = [1:C.nConstr (C.nConstr+1)*ones(1,C.nCp)];
                E            = [ones(1,C.nConstr) 0];
                
                if isempty(alpha)
                    [alpha,Stat] = libqp_splx( C.H(1:S.nD,1:S.nD), f, b, I, E );
                else
                    [alpha,Stat] = libqp_splx( C.H(1:S.nD,1:S.nD), f, b, I, E, [alpha ; 0]);
                end
                Fd           = -Stat.QP;
            end
        end
%         S.timing.qpsolver(S.nIter) = cputime-t0;
        S.timing.qpsolver(S.nIter) = etime(clock, t0);
        S.nnzD = sum(alpha > 0);

        %% update primal solution
%         t0                      = cputime;
        t0                      = clock;
%         W                       = -C.A(:,1:S.nD)*(alpha/sqrt(lambda));
%         W                       = C.A(:,1:S.nD)*(-alpha/sqrt(lambda));      % slow as hell
%         W                       = multAx(-1/sqrt(lambda), C.A, S.nP, S.nD, alpha, 0);
        W                       = mxMultAx(-1/sqrt(lambda), C.A, S.nP, S.nD, alpha, 0);
%         S.timing.wtime(S.nIter) = cputime - t0;
        S.timing.wtime(S.nIter) = etime(clock, t0);


        %% compute value and subgradient of risk at W 
%         t0                      = cputime;
        t0                      = clock;
        [R, subgrad]            = risk( Data, W);
%         S.timing.risk( S.nIter) = cputime-t0;
        S.timing.risk( S.nIter) = etime(clock, t0);
        
        %% evaluate primal objective
        Fp = R + 0.5*lambda*norm(W)^2;
        
        %% keep number of cutting planes <= bufSize - 1
        bufSize = size( C.A, 2 );
        if C.nCp + C.nConstr == bufSize
            
            zeroAlpha = 1e-8;
            idx1      = find( alpha' > zeroAlpha | [1:bufSize] <= C.nConstr );            
            nActCp    = length( idx1 );
            
            if nActCp == bufSize 
                %% create one free slot by aggregating two last added CPs                
                idx1 = [1:C.nConstr+1 C.nConstr+3:bufSize];
                
                a1  = alpha(C.nConstr+1);
                a2  = alpha(C.nConstr+2);
                C.A(:,C.nConstr+1) = (a1*C.A(:,C.nConstr+1) + a2*C.A(:, C.nConstr+2))/(a1+a2);
                C.b(C.nConstr+1)   = (a1*C.b(C.nConstr+1) + a2*C.b(C.nConstr+2))/(a1+a2);
                
                C.H(:,C.nConstr+1) = C.A(:,C.nConstr+1)'*C.A;
                C.H(C.nConstr+1,:) = C.H(:,C.nConstr+1)';
                
                alpha(C.nConstr+1) = a1+a2;
                
                nActCp = nActCp - 1;
            end            
            
            idx2           = [1:nActCp];
            C.A(:, idx2)   = C.A(:, idx1);
            C.b(idx2)      = C.b(idx1);
            C.H(idx2,idx2) = C.H(idx1,idx1);
                      
            C.nCp          = nActCp - C.nConstr;
            S.nD           = C.nCp + C.nConstr;                

            alpha(idx2)    = alpha(idx1);
            alpha          = alpha(1:S.nD);
            
        end
                
        %% update CP buffer and Hessian 
        C.nCp = C.nCp + 1;
        S.nD  = S.nD + 1;

        C.A(:,S.nD) = subgrad;
        C.b(S.nD)   = R - C.A(:,S.nD)'*W;               % fast
%         C.b(S.nD)   = R + (C.A(:,S.nD)'*(-W));

%         t0                        = cputime;
        t0                        = clock;
        if issparse(subgrad)
            C.H(1:S.nD,S.nD)      = full( C.A(:,1:S.nD)'*C.A(:,S.nD));
        else
%             C.H(1:S.nD,S.nD)      = C.A(:,1:S.nD)'*C.A(:,S.nD);            % slow as hell
            C.H(1:S.nD, S.nD)     = mxMultATx(1, C.A, S.nP, S.nD, C.A(:, S.nD), 0);
        end
        C.H(S.nD,1:S.nD-1)        = C.H(1:S.nD-1,S.nD)';
%         S.timing.hessian(S.nIter) = cputime-t0;
        S.timing.hessian(S.nIter) = etime(clock, t0);

        %% eval stopping conditions
        if Fp-Fd <= Opt.tolRel*abs(Fp) | Fp-Fd <= Opt.tolAbs
            S.exitflag = 1;
        elseif S.nIter >= Opt.maxIter
            S.exitflag = 0;
        end 

        %%
        S.Fp(S.nIter)    = Fp;
        S.Fd(S.nIter)    = Fd;
        S.risk(S.nIter)  = R;
%         S.timing.runtime(S.nIter) = cputime-startTime;
        S.timing.runtime(S.nIter) = etime(clock, startTime);

        %%
        if (mod(S.nIter, Opt.verb) == 0 || exitflag ~= -1) & Opt.verb ~= 0
        fprintf(['%4d: tim=%.3f, Fp=%f, Fd=%f, (Fp-Fd)=%f, (Fp-Fd)/Fp=%f, ' ...
                 'R=%f, nCp=%3d, nDualVar=%3d, nzDual=%3d, timrisk=%f, timw=%f, timqp=%f, timhes=%f\n'], ...
                S.nIter, S.timing.runtime(end), ...
                Fp, Fd, Fp-Fd,(Fp-Fd)/Fp, R, C.nCp, S.nD, S.nnzD, ...
                S.timing.risk(end),     S.timing.wtime(end), ...
                S.timing.qpsolver(end), S.timing.hessian(end));
        end

        
        %% save intermediate solution 
        if (Opt.saveProgress && mod(S.nIter, Opt.saveAfter) == 0)
            save(['__progress__' Opt.outName], 'W', 'S', 'C', '-v7.3'); % -v7.3 switch for large sized MAT-files
        end;
        
    end

    %%
    if Opt.verb
        fprintf('Accumulated times\n');
        fprintf('risk time       : %f\n', sum( S.timing.risk     ));
        fprintf('qptime          : %f\n', sum( S.timing.qpsolver ));
        fprintf('hessian time    : %f\n', sum( S.timing.hessian  ));
        fprintf('w time          : %f\n', sum( S.timing.wtime    ));
        fprintf('total runtime   : %f \n', S.timing.runtime(end) );
    end

return;
