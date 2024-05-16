/**
 * @file      drv.CanResourceTxMailbox.cpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2024, Sergey Baigudin, Baigudin Software
 */
#include "drv.CanResourceTxMailbox.hpp"
#include "lib.Register.hpp"

namespace eoos
{
namespace drv
{

CanResourceTxMailbox::CanResourceTxMailbox(int32_t index, cpu::reg::Can* reg)
    : lib::NonCopyable<lib::NoAllocator>()
    , index_( index )
    , reg_( reg )
    , requestStatus_( 0 )
    , errorCounter_( 0 ) {  
}    

CanResourceTxMailbox::~CanResourceTxMailbox()
{
}

bool_t CanResourceTxMailbox::isConstructed() const
{
    return Parent::isConstructed();
}

bool_t CanResourceTxMailbox::transmit(Can::Message const& message)
{
    bool_t res( false );
    if( isConstructed() && isEmpty() )
    {
        lib::Register<cpu::reg::Can::Tx::TiXr>  tixr ( reg_->tx[index_].tixr  );
        lib::Register<cpu::reg::Can::Tx::TdtXr> tdtxr( reg_->tx[index_].tdtxr );
        lib::Register<cpu::reg::Can::Tx::TdlXr> tdlxr( reg_->tx[index_].tdlxr );
        lib::Register<cpu::reg::Can::Tx::TdhXr> tdhxr( reg_->tx[index_].tdhxr );
        tixr.fetch().bit().txrq = 0;
        tixr.commit();
        tixr.fetch();
        tixr.bit().rtr = (message.rtr == true) ? 1 : 0;
        if( message.ide == false )
        {
            tixr.bit().ide = 0;
            tixr.bit().exid = 0;
            tixr.bit().stid = message.id.stid;
        }
        else
        {
            tixr.bit().ide = 1;
            tixr.bit().exid = message.id.exid;
            tixr.bit().stid = message.id.stid;
        }
        tixr.commit();
        tdtxr.bit().dlc = message.dlc;
        tdtxr.commit();
        tdlxr.value() = message.data.v32[0];
        tdlxr.commit();
        tdhxr.value() = message.data.v32[1];
        tdhxr.commit();
        tixr.fetch().bit().txrq = 1;
        tixr.commit();
        res = true;
    }
    return res;
}

int32_t CanResourceTxMailbox::getErrorCounter() const
{
    return errorCounter_;
}

bool_t CanResourceTxMailbox::isEmpty()
{
    bool_t res( false );
    if( isConstructed() )
    {
        lib::Register<cpu::reg::Can::Tsr> tsr( reg_->tsr );
        switch(index_)
        {
            case 0:
            {
                res = tsr.bit().tme0 == 1;
                break;
            }
            case 1:
            {
                res = tsr.bit().tme1 == 1;
                break;                
            }
            case 2:
            {
                res = tsr.bit().tme2 == 1;
                break;
            }
            default:
            {
                res = false;
            }
        }
    }
    return res;
}

bool_t CanResourceTxMailbox::routine()
{
    bool_t res( false );
    if( isConstructed() )
    {    
        if( fixRequestStatus() )
        {
            if( isFixedRequestCompleted() )
            {
                clearRequestStatus();
                res = true;
            }
        }
    }
    return res;
}

bool_t CanResourceTxMailbox::fixRequestStatus()
{
    bool_t res( true );
    lib::Register<cpu::reg::Can::Tsr> const tsr( reg_->tsr );
    switch(index_)
    {
        case 0:
        {
            requestStatus_.bit.tme  = tsr.bit().tme0 ;
            requestStatus_.bit.terr = tsr.bit().terr0;
            requestStatus_.bit.alst = tsr.bit().alst0;
            requestStatus_.bit.txok = tsr.bit().txok0;
            requestStatus_.bit.rqcp = tsr.bit().rqcp0;
            break;
        }
        case 1:
        {
            requestStatus_.bit.tme  = tsr.bit().tme1 ;
            requestStatus_.bit.terr = tsr.bit().terr1;
            requestStatus_.bit.alst = tsr.bit().alst1;
            requestStatus_.bit.txok = tsr.bit().txok1;
            requestStatus_.bit.rqcp = tsr.bit().rqcp1;
            break;                
        }
        case 2:
        {
            requestStatus_.bit.tme  = tsr.bit().tme2 ;
            requestStatus_.bit.terr = tsr.bit().terr2;
            requestStatus_.bit.alst = tsr.bit().alst2;
            requestStatus_.bit.txok = tsr.bit().txok2;
            requestStatus_.bit.rqcp = tsr.bit().rqcp2;
            break;
        }
        default:
        {
            res = false;
        }
    }
    return res;
}

bool_t CanResourceTxMailbox::isFixedRequestCompleted()
{
    const int32_t ERROR_COUNTER_LIMIT( 0x20000000 );
    bool_t const isTransmited( (requestStatus_.bit.rqcp == 1) && (requestStatus_.bit.tme == 1) );
    if( isTransmited && requestStatus_.bit.txok == 0 )
    {
        if( errorCounter_ < ERROR_COUNTER_LIMIT )
        {
            errorCounter_++;
        }
    }    
    return isTransmited;
}

void CanResourceTxMailbox::clearRequestStatus()
{
    switch(index_)
    {
        case 0:
        {
            reg_->tsr.value = cpu::reg::Can::Tsr::RQCP0_MASK;
            break;
        }
        case 1:
        {
            reg_->tsr.value = cpu::reg::Can::Tsr::RQCP1_MASK;
            break;                
        }
        case 2:
        {
            reg_->tsr.value = cpu::reg::Can::Tsr::RQCP2_MASK;
            break;
        }
        default:
        {
            break;
        }
    }
}

} // namespace drv
} // namespace eoos
