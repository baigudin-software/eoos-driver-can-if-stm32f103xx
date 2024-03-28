/**
 * @file      drv.CanResourceTxMailboxRoutine.cpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2024, Sergey Baigudin, Baigudin Software
 */
#include "drv.CanResourceTxMailboxRoutine.hpp"
#include "sys.Thread.hpp"

namespace eoos
{
namespace drv
{

CanResourceTxMailboxRoutine::CanResourceTxMailboxRoutine(CanResourceTxMailbox** mailbox, sys::Semaphore& mailboxSem)
    : lib::NonCopyable<lib::NoAllocator>()
    , api::Runnable()
    , mailbox_( mailbox )
    , mailboxSem_( mailboxSem ) {
    bool_t const isConstructed( construct() );
    setConstructed( isConstructed );
}    

CanResourceTxMailboxRoutine::~CanResourceTxMailboxRoutine()
{
}

bool_t CanResourceTxMailboxRoutine::isConstructed() const
{
    return Parent::isConstructed();
}

void CanResourceTxMailboxRoutine::start()
{    
    bool_t hasToSwitchContex( false );
    for(int32_t i(0); i<NUMBER_OF_TX_MAILBOXS; i++)
    {
        if( mailbox_[i]->routine() )
        {
            if( mailboxSem_.releaseFromInterrupt() )
            {
                hasToSwitchContex = mailboxSem_.hasToSwitchContex() || hasToSwitchContex;
            }        
        }
    }
    if( hasToSwitchContex )
    {
        sys::Thread::yieldFromInterrupt();
    }
}

bool_t CanResourceTxMailboxRoutine::construct()
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
