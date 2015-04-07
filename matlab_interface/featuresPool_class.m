classdef featuresPool_class < handle
    %FEATURESPOOL_CLASS Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (SetAccess = private, Hidden = true)
		objectHandle; % Handle to the underlying C++ class instance
	end
    
    methods
        
        %%% Constructor & Destructor ---------------------------------------------------------------------------------------------

		%% Constructor - Create a new C++ class instance
		function this = featuresPool_class(varargin)
			this.objectHandle = featuresPool_interface('new', varargin{:});
		end

		%% Destructor - Destroy the C++ class instance
		function delete(this)
			featuresPool_interface('delete', this.objectHandle);
		end

		%%% ----------------------------------------------------------------------------------------------------------------------

        %% getHandle - class method call
        function oh = getHandle(this)
            oh = this.objectHandle;
        end;
        
        %% AddLBPSparseFeatures - class method call
        function varargout = addLBPSparseFeatures(this, varargin) 
            [varargout{1:nargout}] = featuresPool_interface('addSparseLBPfeatures', this.objectHandle, varargin{:});
        end;
        
        %% GetFeaturesRaw - class method call
        function varargout = getFeaturesRaw(this, varargin) 
            [varargout{1:nargout}] = featuresPool_interface('getFeaturesRaw', this.objectHandle, varargin{:});
        end;
        
        %% setFeaturesRaw - class method call
        function varargout = setFeaturesRaw(this, varargin) 
            [varargout{1:nargout}] = featuresPool_interface('setFeaturesRaw', this.objectHandle, varargin{:});
        end;
        
        %% computeFromNF - class method call
        function varargout = computeFromNF(this, varargin) 
            [varargout{1:nargout}] = featuresPool_interface('computeFromNF', this.objectHandle, varargin{:});
        end;
        
        %% TODO: Add other methods here
        
    end
    
end

