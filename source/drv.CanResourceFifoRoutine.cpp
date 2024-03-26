/**
 * @file      drv.CanResourceFifoRoutine.cpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2024, Sergey Baigudin, Baigudin Software
 */
#include "drv.CanResourceFifoRoutine.hpp"

namespace eoos
{
namespace drv
{

CanResourceFifoRoutine::CanResourceFifoRoutine(Index index)
    : lib::NonCopyable<lib::NoAllocator>()
    , api::Runnable()
    , index_( index ){
    bool_t const isConstructed( construct() );
    setConstructed( isConstructed );
}    

CanResourceFifoRoutine::~CanResourceFifoRoutine()
{
}

bool_t CanResourceFifoRoutine::isConstructed() const
{
    return Parent::isConstructed();
}

void CanResourceFifoRoutine::start()
{    
}

bool_t CanResourceFifoRoutine::construct()
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
