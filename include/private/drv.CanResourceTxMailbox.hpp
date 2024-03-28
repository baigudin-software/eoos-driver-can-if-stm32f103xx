/**
 * @file      drv.CanResourceTxMailbox.hpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2024, Sergey Baigudin, Baigudin Software
 */
#ifndef DRV_CANRESOURCETXMAILBOX_HPP_
#define DRV_CANRESOURCETXMAILBOX_HPP_

#include "lib.NonCopyable.hpp"
#include "lib.NoAllocator.hpp"
#include "drv.Can.hpp"
#include "cpu.Registers.hpp"

namespace eoos
{
namespace drv
{

/**
 * @class CanResourceTxMailbox
 * @brief TX Mailbox handler.
 */
class CanResourceTxMailbox : public lib::NonCopyable<lib::NoAllocator>
{
    typedef lib::NonCopyable<lib::NoAllocator> Parent;

public:

    /**
     * @brief Number of TX mailboxs.
     */    
    static const int32_t NUMBER_OF_TX_MAILBOXS = 3;

    /**
     * @brief Constructor.
     *
     * @param index Mailbox index.
     * @param reg CAN controller register map.     
     */
    CanResourceTxMailbox(int32_t index, cpu::reg::Can* reg);
    
    /** 
     * @brief Destructor.
     */
    virtual ~CanResourceTxMailbox();
    
    /**
     * @copydoc eoos::api::Object::isConstructed()
     */
    virtual bool_t isConstructed() const;
    
    /**
     * @brief Initiates the transmission of a message.
     *
     * @param message A message to tramsmit.
     * @return True if a transmition is initialied.     
     */
    bool_t transmit(Can::TxMessage const& message);

    /**
     * @brief Tests if the mailbox is ready to transmit.
     *
     * @return True if the mailbox is ready to transmit.
     */
    bool_t isEmpty();
    
    /**
     * @brief Routines interrupt.
     *
     * @return True is interrupt is routined.
     */    
    bool_t routine();

private:
    
    /**
     * @brief Fixs the mailbox transmition status in internal state.
     * 
     * @return True if status is fixed.
     */
    bool_t fixRequestStatus();

    /**
     * @brief Tests if the mailbox transmition is completed in fixed request status.
     *
     * @return True if transmition is completed.
     */
    bool_t isFixedRequestCompleted();

    /**
     * @brief Clears the mailbox transmition status.
     */    
    void clearRequestStatus();

    /**
     * @brief Transmit request status.
     */
    union RequestStatus
    {
        RequestStatus(){}
        RequestStatus(uint32_t v){value = v;}
       ~RequestStatus(){}    
      
        uint32_t value;
        struct 
        {
            uint32_t rqcp : 1; ///< Request completed mailbox
            uint32_t txok : 1; ///< Transmission OK of mailbox
            uint32_t alst : 1; ///< Arbitration lost for mailbox
            uint32_t terr : 1; ///< Transmission error of mailbox
            uint32_t tme  : 1; ///< Transmit mailbox empty
            uint32_t      : 27;
        } bit;
    };

    /**
     * @brief Mailbox index.
     */
    int32_t index_;

    /**
     * @brief CAN registers.
     */
    cpu::reg::Can* reg_;

    /**
     * @brief Transmit request status.
     */    
    RequestStatus requestStatus_;

};

} // namespace drv
} // namespace eoos
#endif // DRV_CANRESOURCETXMAILBOX_HPP_
