#include <ctime>

#include "BufferCollectionStack.h"
#include "Forest.h"
#include "TreeLearnerI.h"
#include "ParallelForestLearner.h"

#if USE_BOOST_THREAD
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#endif

void TrainTrees(    const TreeLearnerI* treeLearner,
                    const BufferCollectionStack& data,
                    int treeStartIndex,
                    int treeStride,
                    int numberOfTrees,
                    Forest* forestOut )
{
    for(int i=treeStartIndex; i<numberOfTrees; i+=treeStride)
    {
        treeLearner->Learn(data, forestOut->mTrees[i], i + static_cast<unsigned int>(std::time(NULL)) );
    }
}

ParallelForestLearner::ParallelForestLearner( const TreeLearnerI* treeLearner, int numberOfTrees, int maxIntParamsDim, int maxFloatParamsDim, int maxYsDim, int numberOfJobs )
: mForest( new Forest(numberOfTrees, 1, maxIntParamsDim, maxFloatParamsDim, maxYsDim) )
, mTreeLearner( treeLearner->Clone() )
, mForestSteps(NULL)
, mNumberOfTrees(numberOfTrees)
, mNumberOfJobs(numberOfJobs)
{}

ParallelForestLearner::ParallelForestLearner( const TreeLearnerI* treeLearner, const PipelineStepI* forestSteps, int numberOfTrees, int maxIntParamsDim, int maxFloatParamsDim, int maxYsDim, int numberOfJobs )
: mForest( new Forest(numberOfTrees, 1, maxIntParamsDim, maxFloatParamsDim, maxYsDim) )
, mTreeLearner( treeLearner->Clone() )
, mForestSteps( forestSteps->Clone() )
, mNumberOfTrees(numberOfTrees)
, mNumberOfJobs(numberOfJobs)
{}


ParallelForestLearner::~ParallelForestLearner()
{
    delete mForest;
    delete mTreeLearner;
    if( mForestSteps != NULL)
    {
        delete mForestSteps;
    }
}

Forest ParallelForestLearner::Learn( const BufferCollection& data )
{
    BufferCollectionStack stack;
    stack.Push(&data);

    BufferCollection* forestData = NULL;
    if( mForestSteps != NULL)
    {
        boost::mt19937 gen;
        gen.seed(static_cast<unsigned int>(std::time(NULL)));

        BufferCollection* forestData = new BufferCollection();
        stack.Push(forestData);
        mForestSteps->ProcessStep(stack, *forestData, gen);
    }

#if USE_BOOST_THREAD
    std::vector< boost::shared_ptr< boost::thread > > threadVec;
    for(int job=0; job<mNumberOfJobs; job++)
    {
        threadVec.push_back( boost::make_shared<boost::thread>(TrainTrees, mTreeLearner, stack, job, mNumberOfJobs, mNumberOfTrees, mForest) );
    }
    for(int job=0; job<mNumberOfJobs; job++)
    {
        threadVec[job]->join();
    }
#else
    TrainTrees(mTreeLearner, stack, 0, 1, mNumberOfTrees, mForest);
#endif
    if( forestData != NULL )
    {
        delete forestData;
    }

    return *mForest;
}


