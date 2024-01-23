/**
 * @file      drv.CanResourceRoutineTx.cpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2024, Sergey Baigudin, Baigudin Software
 */
#include "drv.CanResourceRoutineTx.hpp"

namespace eoos
{
namespace drv
{

CanResourceRoutineTx::CanResourceRoutineTx()
    : lib::NonCopyable<lib::NoAllocator>()
    , api::Runnable() {
    bool_t const isConstructed( construct() );
    setConstructed( isConstructed );    
}

CanResourceRoutineTx::~CanResourceRoutineTx()
{
}

bool_t CanResourceRoutineTx::isConstructed() const
{
    return Parent::isConstructed();
}

void CanResourceRoutineTx::start()
{
}

bool_t CanResourceRoutineTx::construct()
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
