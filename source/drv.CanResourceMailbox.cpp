/**
 * @file      drv.CanResourceMailbox.cpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2024, Sergey Baigudin, Baigudin Software
 */
#include "drv.CanResourceMailbox.hpp"

namespace eoos
{
namespace drv
{

CanResourceMailbox::CanResourceMailbox(int32_t index, cpu::reg::Can* reg)
    : lib::NonCopyable<lib::NoAllocator>()
    , Can::TxHandler()
    , index_( index )
    , reg_( reg ) {
}    

CanResourceMailbox::~CanResourceMailbox()
{
}

bool_t CanResourceMailbox::isConstructed() const
{
    return Parent::isConstructed();
}

bool_t CanResourceMailbox::isTransmited()
{
    return false;
}

void CanResourceMailbox::transmit(Can::TxMessage const& message)
{
    if( isConstructed() && isEmpty() )
    {
        reg_->tx[index_].tixr.bit.txrq = 0;
        reg_->tx[index_].tixr.bit.rtr = (message.rtr == true) ? 1 : 0;
        if( message.ide == false )
        {
            reg_->tx[index_].tixr.bit.ide = 0;
            reg_->tx[index_].tixr.bit.exid = 0;
            reg_->tx[index_].tixr.bit.stid = message.id & 0x000007FF;
        }
        else
        {
            reg_->tx[index_].tixr.bit.ide = 1;
            reg_->tx[index_].tixr.bit.exid = message.id & 0x0003FFFF;
            reg_->tx[index_].tixr.bit.stid = (message.id >> 18) & 0x000007FF;
        }
        reg_->tx[index_].tdtxr.bit.dlc = message.dlc;
        reg_->tx[index_].tdlxr.value = message.data.v32[0];
        reg_->tx[index_].tdhxr.value = message.data.v32[1];
        reg_->tx[index_].tixr.bit.txrq = 1;            
    }
}

bool_t CanResourceMailbox::isEmpty()
{
    bool_t res( false );
    if( isConstructed() )
    {
        if( index_ == 0 )
        {
            res = reg_->tsr.bit.tme0 == 1;
        }
        else if( index_ == 1 )
        {
            res = reg_->tsr.bit.tme1 == 1;
        }
        else if( index_ == 2 )
        {
            res = reg_->tsr.bit.tme2 == 1;
        }
        else
        {
            res = false;
        }
    }
    return res;
}    

} // namespace drv
} // namespace eoos
