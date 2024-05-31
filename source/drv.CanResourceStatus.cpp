/**
 * @file      drv.CanResourceStatus.cpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2024, Sergey Baigudin, Baigudin Software
 */
#include "drv.CanResourceStatus.hpp"
#include "lib.Register.hpp"
#include "sys.Thread.hpp"

namespace eoos
{
namespace drv
{

CanResourceStatus::CanResourceStatus(cpu::reg::Can* reg, api::Supervisor& svc)
    : lib::NonCopyable<lib::NoAllocator>()
    , api::Runnable()
    , reg_( reg )
    , svc_( svc )
    , int_(){
    bool_t const isConstructed( construct() );
    setConstructed( isConstructed );
}    

CanResourceStatus::~CanResourceStatus()
{
    int_->disable();
}

bool_t CanResourceStatus::isConstructed() const
{
    return Parent::isConstructed();
}

void CanResourceStatus::start()
{
    lib::Register<cpu::reg::Can::Esr> esr( reg_->esr);
    lib::Register<cpu::reg::Can::Msr> msr( reg_->msr);    
}

bool_t CanResourceStatus::construct()
{
    bool_t res( false );
    do 
    {
        if( !isConstructed() )
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

bool_t CanResourceStatus::initializeInterrupt()
{
    bool_t res( false );
    api::CpuInterruptController& ic( svc_.getProcessor().getInterruptController() );
    int_.reset( ic.createResource(*this, EXCEPTION_CAN1_SCE) );
    if( !int_.isNull() )
    {
        int_->enable();
        res = true;
    }
    return res;    
}

} // namespace drv
} // namespace eoos
