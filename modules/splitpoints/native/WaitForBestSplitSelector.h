#pragma once

#include <vector>
#include <limits>  

#include "VectorBuffer.h"
#include "MatrixBuffer.h"
#include "Tensor3Buffer.h"
#include "BufferCollectionStack.h"
#include "BufferCollectionUtils.h"
#include "SplitSelectorBuffers.h"
#include "ShouldSplitCriteriaI.h"
#include "FinalizerI.h"
#include "SplitSelectorI.h"
#include "LogSplitInfo.h"

// ----------------------------------------------------------------------------
//
// Find the best split (by impurity) and then check if it is valid
//
// ----------------------------------------------------------------------------
template <class BufferTypes>
class WaitForBestSplitSelector: public SplitSelectorI<BufferTypes>
{
public:
    WaitForBestSplitSelector(const std::vector<SplitSelectorBuffers>& splitBuffers,
                  const ShouldSplitCriteriaI* shouldSplitCriteria,
                  const FinalizerI<BufferTypes>* finalizer);

    WaitForBestSplitSelector(const std::vector<SplitSelectorBuffers>& splitBuffers,
                  const ShouldSplitCriteriaI* shouldSplitCriteria,
                  const FinalizerI<BufferTypes>* finalizer,
                  const SplitBuffersI* bufferSplitter);

    virtual ~WaitForBestSplitSelector();

    virtual SplitSelectorInfo<BufferTypes> ProcessSplits(const BufferCollectionStack& bufferCollectionStack, 
                                                        int depth,
                                                        BufferCollection& extraInfo, int nodeIndex) const;

    virtual SplitSelectorI<BufferTypes>* Clone() const;

private:
    std::vector<SplitSelectorBuffers> mSplitSelectorBuffers;
    const ShouldSplitCriteriaI* mShouldSplitCriteria;
    const FinalizerI<BufferTypes>* mFinalizer;
    const SplitBuffersI* mBufferSplitter;
    const LogSplitInfo<BufferTypes>* mLogger;
};

template <class BufferTypes>
WaitForBestSplitSelector<BufferTypes>::WaitForBestSplitSelector( const std::vector<SplitSelectorBuffers>& splitBuffers,
                                                  const ShouldSplitCriteriaI* shouldSplitCriteria,
                                                  const FinalizerI<BufferTypes>* finalizer)
: mSplitSelectorBuffers(splitBuffers)
, mShouldSplitCriteria(shouldSplitCriteria->Clone())
, mFinalizer(finalizer->Clone())
, mBufferSplitter(NULL)
, mLogger( new LogSplitInfo<BufferTypes>() )
{}

template <class BufferTypes>
WaitForBestSplitSelector<BufferTypes>::WaitForBestSplitSelector( const std::vector<SplitSelectorBuffers>& splitBuffers,
                                                  const ShouldSplitCriteriaI* shouldSplitCriteria,
                                                  const FinalizerI<BufferTypes>* finalizer,
                                                  const SplitBuffersI* bufferSplitter)
: mSplitSelectorBuffers(splitBuffers)
, mShouldSplitCriteria(shouldSplitCriteria->Clone())
, mFinalizer(finalizer->Clone())
, mBufferSplitter((bufferSplitter != NULL) ? bufferSplitter->Clone() : NULL)
, mLogger( new LogSplitInfo<BufferTypes>() )
{}

template <class BufferTypes>
WaitForBestSplitSelector<BufferTypes>::~WaitForBestSplitSelector()
{
    mSplitSelectorBuffers.clear();
    delete mShouldSplitCriteria;
    delete mBufferSplitter;
    delete mFinalizer;
    delete mLogger;
}


template <class BufferTypes>
SplitSelectorInfo<BufferTypes> WaitForBestSplitSelector<BufferTypes>::ProcessSplits(const BufferCollectionStack& readCollection, 
                                                                                    int depth,
                                                                                    BufferCollection& extraInfo, int nodeIndex) const
{
    TimeLogger timer(extraInfo, "WaitForBestSplitSelector");

    typename BufferTypes::ImpurityValue maxImpurity = std::numeric_limits<typename BufferTypes::ImpurityValue>::min();
    int bestSplitSelectorBuffers = SPLIT_SELECTOR_NO_SPLIT;
    int bestFeature = SPLIT_SELECTOR_NO_SPLIT;
    int bestSplitpoint = SPLIT_SELECTOR_NO_SPLIT;

    const int numberOfWaitForBestmSplitSelectorBuffers = mSplitSelectorBuffers.size();

    for(int s=0; s<numberOfWaitForBestmSplitSelectorBuffers; s++)
    {
        const SplitSelectorBuffers& ssb = mSplitSelectorBuffers[s];

        const MatrixBufferTemplate<typename BufferTypes::ImpurityValue>& impurities
           = readCollection.GetBuffer< MatrixBufferTemplate<typename BufferTypes::ImpurityValue> >(ssb.mImpurityBufferId);

        const VectorBufferTemplate<typename BufferTypes::Index>& splitpointCounts
               = readCollection.GetBuffer< VectorBufferTemplate<typename BufferTypes::Index> >(ssb.mSplitpointsCountsBufferId);

        for(int f=0; f<impurities.GetM(); f++)
        {
            for(int t=0; t<splitpointCounts.Get(f); t++)
            {
                const typename BufferTypes::ImpurityValue impurity = impurities.Get(f,t);
                if( impurity > maxImpurity )
                {
                    maxImpurity = impurity;
                    bestSplitSelectorBuffers = s;
                    bestFeature = f;
                    bestSplitpoint = t;
                }
            }
        }
    }

    const bool isSet = bestSplitSelectorBuffers !=  SPLIT_SELECTOR_NO_SPLIT
                        && bestFeature != SPLIT_SELECTOR_NO_SPLIT
                        && bestSplitpoint != SPLIT_SELECTOR_NO_SPLIT;

    if( isSet )
    {
        const SplitSelectorBuffers& ssb = mSplitSelectorBuffers[bestSplitSelectorBuffers];
        const Tensor3BufferTemplate<typename BufferTypes::DatapointCounts>& childCounts
                    = readCollection.GetBuffer< Tensor3BufferTemplate<typename BufferTypes::DatapointCounts> >(ssb.mChildCountsBufferId);

        // Reset if should not split
        const typename BufferTypes::DatapointCounts leftCounts = childCounts.Get(bestFeature,bestSplitpoint,LEFT_CHILD_INDEX);
        const typename BufferTypes::DatapointCounts rightCounts = childCounts.Get(bestFeature,bestSplitpoint,RIGHT_CHILD_INDEX);
        if( !mShouldSplitCriteria->ShouldSplit(depth, maxImpurity, leftCounts+rightCounts, leftCounts, rightCounts, extraInfo, nodeIndex, true))
        {
            bestSplitSelectorBuffers = SPLIT_SELECTOR_NO_SPLIT;
            bestFeature = SPLIT_SELECTOR_NO_SPLIT;
            bestSplitpoint = SPLIT_SELECTOR_NO_SPLIT;
        }
    }

    mLogger->Log(mSplitSelectorBuffers,
                mShouldSplitCriteria,
                readCollection, depth,
                bestSplitSelectorBuffers, bestFeature, bestSplitpoint,
                extraInfo, nodeIndex);

    return SplitSelectorInfo<BufferTypes>(mSplitSelectorBuffers[bestSplitSelectorBuffers],
                                            readCollection, mFinalizer, mBufferSplitter,
                                            bestFeature, bestSplitpoint, depth);
}

template <class BufferTypes>
SplitSelectorI<BufferTypes>* WaitForBestSplitSelector<BufferTypes>::Clone() const
{
    WaitForBestSplitSelector* clone = new WaitForBestSplitSelector<BufferTypes>(mSplitSelectorBuffers, 
                                                                                      mShouldSplitCriteria, 
                                                                                      mFinalizer, 
                                                                                      mBufferSplitter);
    return clone;
}