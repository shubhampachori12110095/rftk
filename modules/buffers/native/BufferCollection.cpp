#include <stdio.h>

#include <asserts.h>
#include "BufferCollection.h"


BufferCollection::BufferCollection()
: mBuffers()
{
}

BufferCollection::BufferCollection(const BufferCollection& bc)
: mBuffers()
{
    for(std::map<BufferCollectionKey_t, boost::any>::const_iterator it=bc.mBuffers.begin(); 
        it!=bc.mBuffers.end(); 
        ++it )
    {
        mBuffers[it->first] = boost::any(it->second);
    }
}

BufferCollection::~BufferCollection()
{
    // printf("~BufferCollection() %p\n", this);
    // for(std::map<BufferCollectionKey_t, boost::any>::const_iterator it=mBuffers.begin(); 
    //     it!=mBuffers.end(); 
    //     ++it )
    // {
    //     printf(" %s\n", it->first.c_str());
    // }
    mBuffers.clear();
}

BufferCollection& BufferCollection::operator=(const BufferCollection& bc)
{
    if(this != &bc) // protect against invalid self-assignment
    {
        mBuffers.clear();
        for(std::map<BufferCollectionKey_t, boost::any>::const_iterator it=bc.mBuffers.begin(); 
            it!=bc.mBuffers.end(); 
            ++it )
        {
            mBuffers[it->first] = boost::any(it->second);
        }
    }
    return *this;
}

#define DEFINE_BUFFER_SWIG_INTERFACE_FOR_TYPE(BUFFER_TYPE) \
bool BufferCollection::Has ## BUFFER_TYPE(std::string name) const \
{ \
    return HasBuffer(name) && HasBuffer<BUFFER_TYPE>(name); \
} \
void BufferCollection::Add ## BUFFER_TYPE(std::string name, BUFFER_TYPE const& data ) \
{ \
    ASSERT(!(HasBuffer(name) && !HasBuffer<BUFFER_TYPE>(name))); \
    AddBuffer<BUFFER_TYPE>(name, data); \
} \
void BufferCollection::Append ## BUFFER_TYPE(std::string name, BUFFER_TYPE const& data ) \
{ \
    AppendBuffer<BUFFER_TYPE>(name, data); \
} \
const BUFFER_TYPE& BufferCollection::Get ## BUFFER_TYPE(const std::string& name) const \
{ \
    return GetBuffer<BUFFER_TYPE>(name); \
} \
BUFFER_TYPE& BufferCollection::Get ## BUFFER_TYPE(const std::string& name) \
{ \
    return GetBuffer<BUFFER_TYPE>(name); \
}

DEFINE_BUFFER_SWIG_INTERFACE_FOR_TYPE(Float32VectorBuffer)
DEFINE_BUFFER_SWIG_INTERFACE_FOR_TYPE(Float64VectorBuffer)
DEFINE_BUFFER_SWIG_INTERFACE_FOR_TYPE(Int32VectorBuffer)
DEFINE_BUFFER_SWIG_INTERFACE_FOR_TYPE(Int64VectorBuffer)
DEFINE_BUFFER_SWIG_INTERFACE_FOR_TYPE(Float32MatrixBuffer)
DEFINE_BUFFER_SWIG_INTERFACE_FOR_TYPE(Float64MatrixBuffer)
DEFINE_BUFFER_SWIG_INTERFACE_FOR_TYPE(Int32MatrixBuffer)
DEFINE_BUFFER_SWIG_INTERFACE_FOR_TYPE(Int64MatrixBuffer)
DEFINE_BUFFER_SWIG_INTERFACE_FOR_TYPE(Float32SparseMatrixBuffer)
DEFINE_BUFFER_SWIG_INTERFACE_FOR_TYPE(Float64SparseMatrixBuffer)
DEFINE_BUFFER_SWIG_INTERFACE_FOR_TYPE(Int32SparseMatrixBuffer)
DEFINE_BUFFER_SWIG_INTERFACE_FOR_TYPE(Int64SparseMatrixBuffer)
DEFINE_BUFFER_SWIG_INTERFACE_FOR_TYPE(Float32Tensor3Buffer)
DEFINE_BUFFER_SWIG_INTERFACE_FOR_TYPE(Float64Tensor3Buffer)
DEFINE_BUFFER_SWIG_INTERFACE_FOR_TYPE(Int32Tensor3Buffer)
DEFINE_BUFFER_SWIG_INTERFACE_FOR_TYPE(Int64Tensor3Buffer)


bool BufferCollection::HasBuffer(std::string name) const
{
    return (mBuffers.find(name) != mBuffers.end());
}

void BufferCollection::Print() const
{
    for(BufferMapType::const_iterator iter = mBuffers.begin(); iter != mBuffers.end(); ++iter)
    {
        printf("%s\n", iter->first.c_str());
    }
}

std::list<std::string> BufferCollection::GetKeys() const
{
    std::list<std::string> keys;
    for(std::map<BufferCollectionKey_t, boost::any>::const_iterator it=mBuffers.begin(); 
        it!=mBuffers.end(); 
        ++it )
    {
        keys.push_back(it->first);
    }
    return keys;
}
