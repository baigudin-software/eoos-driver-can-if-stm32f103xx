/**
 * @file      drv.CanResourceMailbox.hpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2024, Sergey Baigudin, Baigudin Software
 */
#ifndef DRV_CANRESOURCEMAILBOX_HPP_
#define DRV_CANRESOURCEMAILBOX_HPP_

#include "lib.NonCopyable.hpp"
#include "lib.NoAllocator.hpp"
#include "drv.Can.hpp"
#include "cpu.Registers.hpp"

namespace eoos
{
namespace drv
{

/**
 * @class CanResourceMailbox
 * @brief TX Mailbox handler.
 */
class CanResourceMailbox : public lib::NonCopyable<lib::NoAllocator>, public Can::TxHandler
{
    typedef lib::NonCopyable<lib::NoAllocator> Parent;

public:

    /**
     * @brief Constructor.
     *
     * @param index Mailbox index.
     * @param reg CAN controller register map.     
     */
    CanResourceMailbox(int32_t index, cpu::reg::Can* reg);
    
    /** 
     * @brief Destructor.
     */
    virtual ~CanResourceMailbox();
    
    /**
     * @copydoc eoos::api::Object::isConstructed()
     */
    virtual bool_t isConstructed() const;
    
    /**
     * @copydoc eoos::drv::Can::TxHandler::isTransmited()        
     */
    virtual bool_t isTransmited();

    /**
     * @brief Initiates the transmission of a message.
     *
     * @param message A message to tramsmit.
     */
    void transmit(Can::TxMessage const& message);

    /**
     * @brief Tests if the mailbox is ready to transmit.
     *
     * @return True if the mailbox is ready to transmit.
     */
    bool_t isEmpty();

private:

    /**
     * @brief Mailbox index.
     */
    int32_t index_;

    /**
     * @brief CAN registers.
     */
    cpu::reg::Can* reg_;

};

} // namespace drv
} // namespace eoos
#endif // DRV_CANRESOURCEMAILBOX_HPP_
