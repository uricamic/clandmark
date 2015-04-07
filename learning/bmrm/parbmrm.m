function [ w, stat ] = parbmrm( data, risk, lambda, opts )
%PARBMRM Summary of this function goes here
%   Detailed explanation goes here
% 
% 03-01-12, timing statistics: rtime, subgradtime, qptime, hesstime, wtime
% 12-01-12, save euclidean distance of w_t and w_{t-1}

    if nargin<4, opts = struct(); end;
    if ~isfield(opts, 'TolRel'), opts.TolRel = 1e-3; end;
    if ~isfield(opts, 'TolAbs'), opts.TolAbs = 0; end;
    if ~isfield(opts, 'MaxIter'), opts.MaxIter = Inf; end;
    if ~isfield(opts, 'verb'), opts.verb = 0; end;
    if ~isfield(opts,'MaxMemory'), opts.MaxMemory = Inf; end
    if ~isfield(opts,'CleanBuffer'), opts.CleanBuffer = inf; end
%    if ~isfield(opts,'BufSize'), o         0pts.BufSize = 100; end
    
    if ~isfield(opts, 'CleanICP'), opts.CleanICP = false; end;
    if ~isfield(opts, 'CleanAfter'), opts.CleanAfter = 10; end;

    if ~iscell(data), data = {data}; end;
    nThreads = length(data);

    rtime = 0; 
    subgradtime = 0;
    qptime = 0; 
    hesstime = 0; 
    wtime = 0;
    
    clck0_ = clock;
    
    % constants
    exitflag = -1;
    
    % get space dimension
    [R, SG] = risk(data{1});
    
    wDim  = length(SG);
    w = zeros(wDim, 1, 'double');
    prevw = w;
    
    t = 0;
    Rt = zeros(1, nThreads);
    SGt = zeros(wDim, nThreads);
    
    %% Risk and subgradient computation
    parfor p = 1:nThreads
        [Rt(p), SGt(:,p), ldata, risktim] = risk(data{p}, w);
        data{p} = ldata;
        rtime = rtime + risktim.rtime;
        subgradtime = subgradtime + risktim.subgradtime;
    end;
    
    hR = [];
    hSG = [];
    hw = [];
    I = [];
    f = [];
    alpha = [];
    nCP = 0;
        
    CPcounter = [];
    
    hist_Fd(1) = -inf;
    hist_Fp(1) = R+0.5*lambda*norm(w)^2;
    hist_R(1) = R;
    clck1_ = clock;
    hist_runtime(1) = etime(clck1_, clck0_);
    
    % timing stats
    hist_rtime(1) = rtime;
    hist_subgradtime(1) = subgradtime;
    hist_qptime(1) = qptime;
    hist_hesstime(1) = hesstime;
    hist_wtime(1) = wtime;
    
    % wdists stats
    hist_wdists(1) = norm(prevw - w);
    
    fprintf('%4d: Starting BRMR in %d threads\n', t, nThreads);
    
    while exitflag == -1;
        %tic;
        clck0 = clock;    
        t = t + 1;
        
        %% Hessian computation
        clock_s = clock;
        nCP = nCP + nThreads;
        hR = [hR Rt'];
        for i = 1:nThreads
            hSG{end+1} = SGt(:,i);
        end;
%        hSG = [hSG SGt];
        hw = [hw w];
        f = [f (w'*SGt - Rt)];
        if t == 1
            H = (SGt'*SGt)/lambda;
        else
            tmp = zeros(nThreads, nCP);
            for i = 1:nCP
                tmp(:,i) = SGt'*hSG{i}/lambda;
            end;
            H = [H tmp(:,1:end-nThreads)'];
            H = [H; tmp];
        end;
        clock_f = clock;
        hesstime = etime(clock_f, clock_s);
        
        %% QP solver
        clock_s = clock;
        I = [I 1:nThreads];
        alpha = [alpha; zeros(nThreads, 1)];
        [alpha, stat] = libqp_splx(H, f, ones(1, nThreads), I, ones(1, nThreads), alpha);
        clock_f = clock;
        qptime = etime(clock_f, clock_s);
        
        % update CPCounter - add one unused CPs and reset used CPs
        CPcounter = [CPcounter+1; ones(nThreads, 1)];
        CPcounter(alpha > 0) = 0;
        
        %% W update
        clock_s = clock;
        w = zeros(size(SGt,1),1);
        for i = 1:nCP
            w = w - hSG{i}*alpha(i)/lambda;
        end;
        clock_f = clock;
        wtime = etime(clock_f, clock_s);

        %% Risk ans dubgradient computation
        rtime = 0; subgradtime = 0;
        Rt = zeros(1, nThreads);
        SGt = zeros(wDim, nThreads);
        parfor p = 1:nThreads
            [Rt(p) SGt(:,p) ldata, risktim] = risk(data{p}, w);
            data{p} = ldata;
            rtime = rtime + risktim.rtime;
            subgradtime = subgradtime + risktim.subgradtime;
        end;
        
        %%
        
        Fp = lambda/2*norm(w)^2+sum(Rt);
        Fd = -stat.QP;    

        if Fp-Fd<= opts.TolRel*abs(Fp)
            exitflag= 1;
        elseif Fp-Fd <= opts.TolAbs
            exitflag= 2;    
        elseif t >= opts.MaxIter
            exitflag= 0;
        end         

        tmp = whos('H');
        mem = tmp.bytes;
        tmp = whos('hSG');
        mem = mem + tmp.bytes;
        tmp = whos('hR');
        mem = mem + tmp.bytes;
        
        %hist_runtime(t+1) = toc;
        clck1 = clock;
        hist_runtime(t+1) = etime(clck1, clck0);
        hist_Fp(t+1) = Fp;
        hist_Fd(t+1) = Fd;
        hist_R(t+1) = sum(Rt);
        
        % timing stats
        hist_rtime(t+1) = rtime;
        hist_subgradtime(t+1) = subgradtime;
        hist_qptime(t+1) = qptime;
        hist_hesstime(t+1) = hesstime;
        hist_wtime(t+1) = wtime;
        % wdists stats
        wdist = norm(prevw - w);
        hist_wdists(t+1) = wdist;
        
        prevw = w;
        
        nzA = length(find(alpha > 0));
        if mod(t,opts.verb) == 0 || exitflag ~= -1
            fprintf('%4d: tim=%.3f, Fp=%f, Fd=%f, (Fp-Fd)=%f, (Fp-Fd)/Fp=%f, R=%f, nCP=%d, nzA=%d, mem=%dMB, wdist=%f\n', ...
                t, hist_runtime(t+1), Fp, Fd, Fp-Fd,(Fp-Fd)/Fp, sum(Rt), nCP, nzA, ceil(mem/1024/1024), wdist);
        end;

        if opts.MaxMemory < (mem/1024/1024)
            exitflag = -2;
        end;
        
        if mod(t,opts.CleanBuffer) == 0 && nzA < t*nThreads
            old_nCP = nCP;
            idx = find(alpha > 0);
            nCP = length(idx);

            alpha = alpha(idx);
            H(1:nCP,1:nCP) = H(idx,idx);
            hSG(:,1:nCP) = hSG(:,idx);
            hR(1:nCP) = hR(idx);

            fprintf('done. (old_nCP=%d, new_nCP=%d) \n',old_nCP, nCP);
        end
        
        % clear inactive CPs
        if opts.CleanICP
            iCP = find(CPcounter>=opts.CleanAfter);
            I(iCP) = [];
            alpha(iCP) = [];
            CPcounter(iCP) = [];
            f(iCP) = [];
            hSG(iCP) = [];
            nCP = nCP - length(iCP);
            H(:, iCP) = []; H(iCP, :) = [];
        end;
        
    end;

    stat = [];
    stat.Fp = Fp;
    stat.Fd = Fd;
    stat.nIter = t;
    stat.hist.Fp = hist_Fp(1:t+1);
    stat.hist.Fd = hist_Fd(1:t+1);
    stat.hist.R = hist_R(1:t+1);
    stat.hist.runtime = hist_runtime(1:t+1);
    
    % timing stats
    stat.timings.r = hist_rtime(1:t+1);
    stat.timings.subgrad = hist_subgradtime(1:t+1);
    stat.timings.hessian = hist_hesstime(1:t+1);
    stat.timings.qp = hist_qptime(1:t+1);
    stat.timings.w = hist_wtime(1:t+1);
    stat.timings.overall = hist_runtime(1:t+1);
    
    % euclidean distance of consecutive w_{t-1} and w_t
    stat.hist.wdist = hist_wdists(1:t+1);
    
end

