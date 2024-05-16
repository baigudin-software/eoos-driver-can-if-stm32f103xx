/**
 * @file      drv.CanResourceRxFifo.cpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2024, Sergey Baigudin, Baigudin Software
 */
#include "drv.CanResourceRxFifo.hpp"
#include "lib.Register.hpp"
#include "sys.Thread.hpp"

namespace eoos
{
namespace drv
{

CanResourceRxFifo::CanResourceRxFifo(Can::RxFifo index,  bool_t isLocked, cpu::reg::Can* reg, api::Supervisor& svc)
    : lib::NonCopyable<lib::NoAllocator>()
    , api::Runnable()
    , fifo_(isLocked)
    , mutex_()
    , sem_(0, NUMBER_OF_RX_MAILBOXS_IN_FIFO)
    , index_( index )
    , reg_( reg )
    , svc_( svc )
    , int_(){
    bool_t const isConstructed( construct() );
    setConstructed( isConstructed );
}    

CanResourceRxFifo::~CanResourceRxFifo()
{
    int_->disable();
}

bool_t CanResourceRxFifo::isConstructed() const
{
    return Parent::isConstructed();
}

bool_t CanResourceRxFifo::receive(Can::Message* message)
{
    bool_t res( false );
    if( isConstructed() && sem_.acquire() )
    {
        lib::Guard<> const guard(mutex_);
        if( !fifo_.isEmpty() )
        {
            *message = fifo_.peek();
            fifo_.remove();
            res = true;
        }
    }
    return res;
}

void CanResourceRxFifo::start()
{
    lib::Register<cpu::reg::Can::RfXr>      rfxr ( reg_->rfxr[index_]     );    
    if( rfxr.bit().fmpx > 0 )
    {
        lib::Register<cpu::reg::Can::Rx::RiXr>  rixr ( reg_->rx[index_].rixr  );
        lib::Register<cpu::reg::Can::Rx::RdtXr> rdtxr( reg_->rx[index_].rdtxr );
        lib::Register<cpu::reg::Can::Rx::RdlXr> rdlxr( reg_->rx[index_].rdlxr );    
        lib::Register<cpu::reg::Can::Rx::RdhXr> rdhxr( reg_->rx[index_].rdhxr );
        Can::Message message;   
        message.id.stid = rixr.bit().stid;
        message.id.exid = rixr.bit().exid;
        message.rtr = (rixr.bit().rtr == 1) ? true : false;
        message.ide = (rixr.bit().ide == 1) ? true : false;
        message.dlc = rdtxr.bit().dlc;
        message.data.v32[0] = rdlxr.value();
        message.data.v32[1] = rdhxr.value();
        bool_t isAddedToLast( !fifo_.isLocked() && fifo_.isFull() );
        if( fifo_.add(message) )
        {
            if( !isAddedToLast )
            {
                if( sem_.releaseFromInterrupt() )
                {
                    if( sem_.hasToSwitchContex() )
                    {
                        sys::Thread::yieldFromInterrupt();
                    }
                }
            }
        }
        rfxr.bit().rfomx = 1;
        rfxr.commit();
    }
}

bool_t CanResourceRxFifo::construct()
{
    bool_t res( false );
    do 
    {
        if( !isConstructed() )
        {
            break;
        }
        if( !fifo_.isConstructed() )
        {
            break;
        }
        if( !mutex_.isConstructed() )
        {
            break;
        }
        if( !sem_.isConstructed() )
        {
            break;
        }        
        if( !initializeInterrupt() )
        {
            break;
        }
        res = true;
    } while(false);
    return res;    
}

bool_t CanResourceRxFifo::initializeInterrupt()
{
    const int32_t EXCEPTION_WRONG( -1 );
    int32_t source( EXCEPTION_WRONG );
    if(index_ == Can::RXFIFO_0)
    {
        source = EXCEPTION_CAN1_RX0;
    }
    else if(index_ == Can::RXFIFO_1)
    {
        source = EXCEPTION_CAN1_RX1;
    }
    else
    {
        source = EXCEPTION_WRONG;
    }
    bool_t res( false );
    if( source != EXCEPTION_WRONG )
    {
        api::CpuInterruptController& ic( svc_.getProcessor().getInterruptController() );
        int_.reset( ic.createResource(*this, source) );
        if( !int_.isNull() )
        {
            int_->enable();
            res = true;            
        }
    }
    return res;    
}

} // namespace drv
} // namespace eoos
