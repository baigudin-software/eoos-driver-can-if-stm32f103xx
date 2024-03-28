/**
 * @file      drv.CanResourceRxFifoRoutine.cpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2024, Sergey Baigudin, Baigudin Software
 */
#include "drv.CanResourceRxFifoRoutine.hpp"

namespace eoos
{
namespace drv
{

CanResourceRxFifoRoutine::CanResourceRxFifoRoutine(Index index)
    : lib::NonCopyable<lib::NoAllocator>()
    , api::Runnable()
    , index_( index ){
    bool_t const isConstructed( construct() );
    setConstructed( isConstructed );
}    

CanResourceRxFifoRoutine::~CanResourceRxFifoRoutine()
{
}

bool_t CanResourceRxFifoRoutine::isConstructed() const
{
    return Parent::isConstructed();
}

void CanResourceRxFifoRoutine::start()
{    
}

bool_t CanResourceRxFifoRoutine::construct()
{
    bool_t res( false );
    do 
    {
        if( !isConstructed() )
        {
            break;
        }
        res = true;
    } while(false);
    return res;    
}

} // namespace drv
} // namespace eoos
