/**
 * @file      drv.CanResourceRx.cpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2023-2024, Sergey Baigudin, Baigudin Software
 */
#include "drv.CanResourceRx.hpp"

namespace eoos
{
namespace drv
{

CanResourceRx::CanResourceRx(cpu::reg::Can* reg, api::Supervisor& svc)
    : lib::NonCopyable<lib::NoAllocator>()
    , reg_( reg )  
    , svc_( svc )
    , mutex_()
    , fifoIsr0_( CanResourceRxFifoRoutine::INDEX_FIFO0 )
    , fifoIsr1_( CanResourceRxFifoRoutine::INDEX_FIFO1 ) {
    bool_t const isConstructed( construct() );
    setConstructed( isConstructed );
}    

CanResourceRx::~CanResourceRx()
{
    deinitialize();
}

bool_t CanResourceRx::isConstructed() const
{
    return Parent::isConstructed();
}

bool_t CanResourceRx::receive(Can::RxMessage* message)
{
	return false;
}

bool_t CanResourceRx::setReceiveFilter(Can::RxFilter const& filter)
{
    return false;
}

bool_t CanResourceRx::construct()
{
    fifoIsr_[0] = &fifoIsr0_;
    fifoIsr_[1] = &fifoIsr1_;

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
        if( !fifoIsr0_.isConstructed() )
        {
            break;
        }
        if( !fifoIsr1_.isConstructed() )
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

bool_t CanResourceRx::initialize()
{
    bool_t res( false );
    do 
    {
        // Get interrupt controller
        api::CpuInterruptController& ic( svc_.getProcessor().getInterruptController() );
        // Set ISR for Receive FIFOs
        bool_t isFifoIntCreated( true );
        for(int32_t i(0); i<NUMBER_OF_RX_FIFOS; i++)
        {
            int32_t source( -1 );
            switch(i)
            {
                case 0:
                {
                    source = EXCEPTION_CAN1_RX0;
                    break;
                }
                case 1:
                {
                    source = EXCEPTION_CAN1_RX1;
                    break;
                }
                default:
                {
                    isFifoIntCreated = false;
                    break;
                }
            }
            if( !isFifoIntCreated )
            {
                break;
            }
            fifoInt_[i].reset( ic.createResource(*fifoIsr_[i], source) );
            if( fifoInt_[i].isNull() )
            {
                isFifoIntCreated = false;
                break;
            }
            if( !fifoIsr_[i]->isConstructed() )
            {
                isFifoIntCreated = false;
                break;
            }
            fifoInt_[i]->enable();
        }
        if( !isFifoIntCreated )
        {
            break;
        }
        // Complite successfully
        res = true;
    } while(false);
    return res;
}

void CanResourceRx::deinitialize()
{
    // Unset ISR for Receive FIFOs
    for(int32_t i(0); i<NUMBER_OF_RX_FIFOS; i++)
    {
        fifoInt_[i]->disable();
    }    
}

} // namespace drv
} // namespace eoos
