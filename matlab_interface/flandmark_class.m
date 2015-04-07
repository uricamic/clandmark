%FLANDMARK_INTERFACE MATLAB class wrapper to an underlying C++ class
classdef flandmark_class < handle

  properties (SetAccess = private, Hidden = true)
	  objectHandle; % Handle to the underlying C++ class instance
  end

  methods

    %%% Constructor & Destructor ---------------------------------------------------------------------------------------------

    %% Constructor - Create a new C++ class instance
    function this = flandmark_class(varargin)
      this.objectHandle = flandmark_interface('new', varargin{:});
    end

    %% Destructor - Destroy the C++ class instance
    function delete(this)
      flandmark_interface('delete', this.objectHandle);
    end

    %%% ----------------------------------------------------------------------------------------------------------------------

    %% Detect - class method call
    function varargout = detect(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('detect', this.objectHandle, varargin{:});
    end;
	  
    %% Detect optimized - class method call
    function varargout = detect_optimized(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('detect_optimized', this.objectHandle, varargin{:});
    end;

    %% Detect optimized from Pool - class method call
    function varargout = detectOptimizedFromPool(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('detectOptimizedFromPool', this.objectHandle, varargin{:});
    end;

    %% Detect Mirrored - class method call
    function varargout = detect_mirrored(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('detectMirrored', this.objectHandle, varargin{:});
    end;

    %% Detect base - class method call
    function varargout = detect_base(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('detectBase', this.objectHandle, varargin{:});
    end;

    %% Get normalized frame - class method call
    function varargout = getNormalizedFrame(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getNF', this.objectHandle, varargin{:});
    end;

    %% Set normalization factor - class method call
    function varargout = setNormalizationFactor(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('setNormalizationFactor', this.objectHandle, varargin{:});
    end;

    %% GetWdim - class method call
    function varargout = getWdim(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getWdim', this.objectHandle, varargin{:});
    end;

    %% SetW - class method call
    function varargout = setW(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('setW', this.objectHandle, varargin{:});
    end;

    %% GetW - class method call
    function varargout = getW(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getW', this.objectHandle, varargin{:});
    end;

    %% Get Psi - class method call
    function varargout = getPsi(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getPsi', this.objectHandle, varargin{:});
    end;

    %% Get Psi Base - class method call
    function varargout = getPsi_base(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getPsi_base', this.objectHandle, varargin{:});
    end;

    %% Write - class method call
    function varargout = write(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('write', this.objectHandle, varargin{:});
    end;

    %% SetTableLoss - class method call
    function varargout = setTableLoss(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('setTableLoss', this.objectHandle, varargin{:});
    end;

    %% GetNodesSearchSpaces - class method call
    function varargout = getNodesSearchSpaces(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getNodesSearchSpaces', this.objectHandle, varargin{:});
    end;

    %% GetNodesWindowSizes - class method call
    function varargout = getNodesWindowSizes(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getNodesWindowSizes', this.objectHandle, varargin{:});
    end;

    %% getBWsize - class method call
    function varargout = getBWsize(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getBWsize', this.objectHandle, varargin{:});
    end;

    %% Get Landmarks Count - class method call
    function varargout = getLandmarksCount(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getLandmarksCount', this.objectHandle, varargin{:});
    end;

    %% Get Edges Count - class method call
    function varargout = getEdgesCount(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getEdgesCount', this.objectHandle, varargin{:});
    end;

    %% Get W edges - class method call
    function varargout = getWedges(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getWedges', this.objectHandle, varargin{:});
    end;

    %% Get W nodes - class method call
    function varargout = getWnodes(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getWnodes', this.objectHandle, varargin{:});
    end;

    %% Get H - class method call
    function varargout = getH(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getH', this.objectHandle, varargin{:});
    end;

    %% Get Hinv - class method call
    function varargout = getHinv(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getHinv', this.objectHandle, varargin{:});
    end;

    %% getIntermediateResults - class method call
    function varargout = getIntermediateResults(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getIntermediateResults', this.objectHandle, varargin{:});
    end;

    %%% Two stage learning related methods ------------------------------------------------------------------------------

    %% Get PsiNodes Base - class method call
    function varargout = getPsiNodes_base(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getPsiNodes_base', this.objectHandle, varargin{:});
    end;

    %% Get Psi - class method call
    function varargout = getPsiNodes(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getPsiNodes', this.objectHandle, varargin{:});
    end;

    %% ArgmaxNodes_base - class method call
    function varargout = argmaxNodes_base(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('argmaxNodes_base', this.objectHandle, varargin{:});
    end;

    %% Get Nodes Dimensions - class method call
    function varargout = getNodesDimensions(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getNodesDimensions', this.objectHandle, varargin{:});
    end;

  %% Get Edges Dimensions - class method call
    function varargout = getEdgesDimensions(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getEdgesDimensions', this.objectHandle, varargin{:});
    end;

    %% Get PsiNodes Dimension - class method call
    function varargout = getPsiNodesDimension(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getPsiNodesDimension', this.objectHandle, varargin{:});
    end;

    %% Get PsiEdges Dimension - class method call
    function varargout = getPsiEdgesDimension(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getPsiEdgesDimension', this.objectHandle, varargin{:});
    end;

    %% Set Nodes W - class method call
    function varargout = setWnodes(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('setWnodes', this.objectHandle, varargin{:});
    end;

    %% GetLossAt - class method call
    function varargout = getLossAt(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getLossAt', this.objectHandle, varargin{:});
    end;

    %%% -----------------------------------------------------------------------------------------------------------------

    %% Get Timing Stats - class method call
    function varargout = getTimingsStats(this, varargin)
	[varargout{1:nargout}] = flandmark_interface('getTimingsStats', this.objectHandle, varargin{:});
    end;

    %%% SPEED UP --------------------------------------------------------------------------------------------------------

    %% Set NF featuresPool - class method call
    function varargout = setFeaturesPool(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('setFeaturesPool', this.objectHandle, varargin{:});
    end;

    %% Detect base optimized - class method call
    function varargout = detect_base_optimized(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('detectBaseOptimized', this.objectHandle, varargin{:});
    end;

    %% Detect base optimized - class method call
    function varargout = detect_base_optimizedFromPool(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('detectBaseOptimizedFromPool', this.objectHandle, varargin{:});
    end;

    %% Get Psi Base - class method call
    function varargout = getPsi_base_optimized(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getPsi_base_optimized', this.objectHandle, varargin{:});
    end;

    %% Get W offsets - class method call
    function varargout = getWoffsets(this, varargin)
      [varargout{1:nargout}] = flandmark_interface('getWoffsets', this.objectHandle, varargin{:});
    end;

    %%% DEBUG -----------------------------------------------------------------------------------------------------------

    %% Get Features - class method call
    %function varargout = getNodeFeatures(this, varargin)
    %	[varargout{1:nargout}] = flandmark_interface('getNodeFeatures', this.objectHandle, varargin{:});
    %end;

    %%% -----------------------------------------------------------------------------------------------------------------

  end

end
