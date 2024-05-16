/**
 * @file      drv.CanResourceRx.cpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2023-2024, Sergey Baigudin, Baigudin Software
 */
#include "drv.CanResourceRx.hpp"
#include "lib.Register.hpp"

namespace eoos
{
namespace drv
{

CanResourceRx::CanResourceRx(Can::Config const& config, cpu::reg::Can* reg, api::Supervisor& svc)
    : lib::NonCopyable<lib::NoAllocator>()
    , reg_( reg )
    , mutex_()
    , fifo0_( Can::RXFIFO_0, ((config.reg.mcr.rflm == 1) ? true : false), reg, svc )
    , fifo1_( Can::RXFIFO_1, ((config.reg.mcr.rflm == 1) ? true : false), reg, svc ) {
    bool_t const isConstructed( construct() );
    setConstructed( isConstructed );
}    

CanResourceRx::~CanResourceRx()
{
}

bool_t CanResourceRx::isConstructed() const
{
    return Parent::isConstructed();
}

bool_t CanResourceRx::receive(Can::Message* message, Can::RxFifo fifo)
{
    bool_t res( false );
    switch(fifo)
    {
        case Can::RXFIFO_0:
        {
            res = fifo0_.receive(message);
            break;
        }
        case Can::RXFIFO_1:
        {
            res = fifo1_.receive(message);
            break;
        }
        default:
        {
            res = false;
            break;
        }
    }
	return res;
}

bool_t CanResourceRx::setReceiveFilter(Can::RxFilter const& filter)
{
    bool_t res( false );
    if( isConstructed() && ( filter.index < Can::RxFilter::NUMBER_OF_FILTER_GROUPS ) )
    {
        lib::Guard<> const guard(mutex_);
        lib::Register<cpu::reg::Can::Fmr>   fmr  ( reg_->fmr   );
        lib::Register<cpu::reg::Can::Fm1r>  fm1r ( reg_->fm1r  );        
        lib::Register<cpu::reg::Can::Fs1r>  fs1r ( reg_->fs1r  );
        lib::Register<cpu::reg::Can::Ffa1r> ffa1r( reg_->ffa1r );
        lib::Register<cpu::reg::Can::Fa1r>  fa1r ( reg_->fa1r  );
        // Set initialization mode for the filters
        fmr.bit().finit = 1;
        fmr.commit();
        // Deactivate filter
        fa1r.clearBit( filter.index );
        fa1r.commit();
        // Set filter mode
        if( filter.mode == Can::RxFilter::MODE_IDMASK )
        {
            fm1r.clearBit( filter.index );
        }
        if( filter.mode == Can::RxFilter::MODE_IDLIST )
        {
            fm1r.setBit( filter.index );
        }
        fm1r.commit();
        // Set filter scale
        if( filter.scale == Can::RxFilter::SCALE_16BIT )
        {
            fs1r.clearBit( filter.index );
        }
        if( filter.scale == Can::RxFilter::SCALE_32BIT )
        {
            fs1r.setBit( filter.index );
        }
        fs1r.commit();
        // Set FIFO
        if( filter.fifo == Can::RxFilter::FIFO_0 )
        {
            ffa1r.clearBit( filter.index );
        }
        if( filter.fifo == Can::RxFilter::FIFO_1 )
        {
            ffa1r.setBit( filter.index );
        }
        ffa1r.commit();
        // Write filter bank
        union
        {
            cpu::reg::Can::FiRx::Value  firx[2];
            Can::RxFilter::Filters      filters;
        } reg = {
            .filters = filter.filters
        };
        reg_->firx[filter.index][0].value = reg.firx[0];
        reg_->firx[filter.index][1].value = reg.firx[1];
        // Activate filter
        fa1r.setBit( filter.index );
        fa1r.commit();        
        // Set active filters mode
        fmr.bit().finit = 0;
        fmr.commit();
        res = true;
    }
    return res;
}

bool_t CanResourceRx::construct()
{
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
        if( !fifo0_.isConstructed() )
        {
            break;
        }
        if( !fifo1_.isConstructed() )
        {
            break;
        }                
        res = true;
    } while(false);
    return res;    
}

} // namespace drv
} // namespace eoos
