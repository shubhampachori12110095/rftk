%module regression
%{
    #define SWIG_FILE_WITH_INIT
    #include "SumOfVarianceWalker.h"
    #include "SumOfVarianceImpurity.h"
    #include "BestSplitpointsWalkingSortedStep.h"
    #include "FinalizerI.h"
    #include "MeanVarianceEstimatorFinalizer.h"
    #include "MeanVarianceCombiner.h"
    #include "MeanVarianceStatsUpdater.h"
%}

%include <exception.i>
%import(module="rftk.asserts") "asserts.i"
%import(module="rftk.buffers") "buffers.i"
%import(module="rftk.pipeline") "pipeline_external.i"
%import(module="rftk.splitpoints") "splitpoints_external.i"

%include "SumOfVarianceWalker.h"
%include "SumOfVarianceImpurity.h"
%include "BestSplitpointsWalkingSortedStep.h"
%include "FinalizerI.h"
%include "MeanVarianceEstimatorFinalizer.h"
%include "MeanVarianceCombiner.h"
%include "MeanVarianceStatsUpdater.h"