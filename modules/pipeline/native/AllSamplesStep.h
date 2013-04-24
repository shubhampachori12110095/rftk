#pragma once

#include <asserts.h>
#include <bootstrap.h>

#include "BufferCollection.h"
#include "PipelineStepI.h"
#include "UniqueBufferId.h"

// ----------------------------------------------------------------------------
//
// Produces index and weight buffers for all datapoints (all weights are one)
//
// ----------------------------------------------------------------------------
template <class FloatType, class IntType>
class AllSamplesStep: public PipelineStepI
{
public:
    AllSamplesStep( const UniqueBufferId::BufferId& dataBuffer );
    virtual ~AllSamplesStep();

    virtual PipelineStepI* Clone() const;

    virtual void ProcessStep(   const BufferCollectionStack& readCollection,
                                BufferCollection& writeCollection) const;

    // Read only output buffer
    const UniqueBufferId::BufferId IndicesBufferId;
    const UniqueBufferId::BufferId WeightsBufferId;
private:
    const UniqueBufferId::BufferId mDataBufferId;

};


template <class FloatType, class IntType>
AllSamplesStep<FloatType, IntType>::AllSamplesStep(const UniqueBufferId::BufferId& dataBuffer)
: IndicesBufferId(UniqueBufferId::GetBufferId("IndicesBuffer"))
, WeightsBufferId(UniqueBufferId::GetBufferId("WeightsBuffer"))
, mDataBufferId(dataBuffer)
{}

template <class FloatType, class IntType>
AllSamplesStep<FloatType, IntType>::~AllSamplesStep()
{}

template <class FloatType, class IntType>
PipelineStepI* AllSamplesStep<FloatType, IntType>::Clone() const
{
    AllSamplesStep<FloatType, IntType>* clone = new AllSamplesStep<FloatType, IntType>(*this);
    return clone;
}

template <class FloatType, class IntType>
void AllSamplesStep<FloatType, IntType>::ProcessStep(const BufferCollectionStack& readCollection,
                                                                BufferCollection& writeCollection) const
{
    ASSERT(readCollection.HasBuffer< MatrixBufferTemplate<FloatType> >(mDataBufferId));

    const MatrixBufferTemplate<FloatType> & buffer 
          = readCollection.GetBuffer <MatrixBufferTemplate<FloatType> >(mDataBufferId);
    VectorBufferTemplate<IntType>& indices 
          = writeCollection.GetOrAddBuffer< VectorBufferTemplate<IntType> >(IndicesBufferId);
    VectorBufferTemplate<FloatType>& weights 
          = writeCollection.GetOrAddBuffer< VectorBufferTemplate<FloatType> >(WeightsBufferId);     

    const IntType numberOfSamples = buffer.GetM();
    weights.Resize(numberOfSamples);
    weights.SetAll(FloatType(1));
    indices.Resize(numberOfSamples);
    for(int i=0; i<numberOfSamples; i++)
    {
        indices.Set(i, i);
    } 
}