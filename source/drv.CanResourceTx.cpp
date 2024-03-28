/**
 * @file      drv.CanResourceTx.cpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2023-2024, Sergey Baigudin, Baigudin Software
 */
#include "drv.CanResourceTx.hpp"

namespace eoos
{
namespace drv
{

CanResourceTx::CanResourceTx(cpu::reg::Can* reg, api::Supervisor& svc)
    : lib::NonCopyable<lib::NoAllocator>()
    , reg_( reg )  
    , svc_( svc )
    , mutex_()
    , mailbox0_( 0, reg_ )
    , mailbox1_( 1, reg_ )
    , mailbox2_( 2, reg_ )
    , mailboxSem_( NUMBER_OF_TX_MAILBOXS, NUMBER_OF_TX_MAILBOXS )    
    , mailboxInt_( NULLPTR )
    , mailboxIsr_( mailbox_, mailboxSem_ ) {
    bool_t const isConstructed( construct() );
    setConstructed( isConstructed );
}    

CanResourceTx::~CanResourceTx()
{
    deinitialize();
}

bool_t CanResourceTx::isConstructed() const
{
    return Parent::isConstructed();
}

bool_t CanResourceTx::transmit(Can::TxMessage const& message)
{
    bool_t res( false );
    if( isConstructed() && mailboxSem_.acquire() )
    {
        lib::Guard<> const guard(mutex_);
        for(int32_t i(0); i<NUMBER_OF_TX_MAILBOXS; i++)
        {
            if( mailbox_[i]->isEmpty() )
            {
                res = mailbox_[i]->transmit(message);
                break;
            }
        }
    }
    return res;
}

bool_t CanResourceTx::construct()
{
    mailbox_[0] = &mailbox0_;
    mailbox_[1] = &mailbox1_;
    mailbox_[2] = &mailbox2_;

    bool_t res( false );
    do 
    {
        if( !isConstructed() )
        {
            break;
        }
        if( !mutex_.isConstructed() )
        {
            break;
        }
        if( !mailbox0_.isConstructed() )
        {
            break;
        }
        if( !mailbox1_.isConstructed() )
        {
            break;
        }
        if( !mailbox2_.isConstructed() )
        {
            break;
        }
        if( !mailboxSem_.isConstructed() )
        {
            break;            
        }
        if( !mailboxIsr_.isConstructed() )
        {
            break;
        }
        if( !initialize() )
        {
            break;
        }
        res = true;
    } while(false);
    return res;    
}

bool_t CanResourceTx::initialize()
{
    bool_t res( false );
    do 
    {
        // Get interrupt controller
        api::CpuInterruptController& ic( svc_.getProcessor().getInterruptController() );
        // Set ISR for Transmit mailbox empty interrupt enable generated 
        // when RQCPx (Request completed mailbox) bit is set.
        mailboxInt_.reset( ic.createResource(mailboxIsr_, EXCEPTION_CAN1_TX) );
        if( mailboxInt_.isNull() )
        {
            break;
        }
        if( !mailboxInt_->isConstructed()  )
        { 
            break;
        }
        mailboxInt_->enable();
        // Complite successfully
        res = true;
    } while(false);
    return res;
}

void CanResourceTx::deinitialize()
{
    // Unset ISR for Transmit mailbox empty interrupt enable generated 
    mailboxInt_->disable();    
}

} // namespace drv
} // namespace eoos
