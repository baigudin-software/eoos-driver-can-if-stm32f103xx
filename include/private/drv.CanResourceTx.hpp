/**
 * @file      drv.CanResourceTx.hpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2023-2024, Sergey Baigudin, Baigudin Software
 */
#ifndef DRV_CANRESOURCETX_HPP_
#define DRV_CANRESOURCETX_HPP_

#include "lib.NonCopyable.hpp"
#include "lib.NoAllocator.hpp"
#include "api.Supervisor.hpp"
#include "drv.Can.hpp"
#include "drv.CanResourceTxMailbox.hpp"
#include "drv.CanResourceTxMailboxRoutine.hpp"
#include "cpu.Interrupt.hpp"
#include "sys.Mutex.hpp"
#include "sys.Semaphore.hpp"
#include "lib.UniquePointer.hpp"

namespace eoos
{
namespace drv
{

/**
 * @class CanResourceTx
 * @brief CAN TX device resource.
 */
class CanResourceTx : public lib::NonCopyable<lib::NoAllocator>
{
    typedef lib::NonCopyable<lib::NoAllocator> Parent;

public:

    /**
     * @brief Constructor.
     *
     * @param reg CAN registers.
     * @param svc Supervisor call to the system.
     */
    CanResourceTx(cpu::reg::Can* reg, api::Supervisor& svc);
    
    /** 
     * @brief Destructor.
     */
    virtual ~CanResourceTx();
    
    /**
     * @copydoc eoos::api::Object::isConstructed()
     */
    virtual bool_t isConstructed() const;
    
    /**
     * @copydoc eoos::drv::Can::transmit()
     */
    bool_t transmit(Can::TxMessage const& message);
	        
protected:

    using Parent::setConstructed;

private:

    /**
     * Constructs this object.
     *
     * @return true if object has been constructed successfully.
     */
    bool_t construct();
    
    /**
     * @brief Initializes the hardware.
     *
     * @return True if initialized.
     */
    bool_t initialize();

    /**
     * @brief Deinitializes the hardware.
     */
    void deinitialize();    
        
    /**
     * @enum Exception
     * @brief CAN Exception numbers.
     */    
    enum Exception
    {
        EXCEPTION_CAN1_TX  = cpu::Interrupt<lib::NoAllocator>::EXCEPTION_USB_HP_CAN1_TX,  ///< Transmit interrupt
    };
    
    /**
     * @brief Number of TX mailboxs.
     */    
    static const int32_t NUMBER_OF_TX_MAILBOXS = CanResourceTxMailbox::NUMBER_OF_TX_MAILBOXS;
    
    /**
     * @brief CAN registers.
     */
    cpu::reg::Can* reg_;

    /**
     * @brief Supervisor call to the system.
     */        
    api::Supervisor& svc_;

    /**
     * @brief This resource mutex.
     */
    sys::Mutex mutex_;

    /**
     * @brief TX mailboxs.
     */    
    CanResourceTxMailbox  mailbox0_;
    CanResourceTxMailbox  mailbox1_;
    CanResourceTxMailbox  mailbox2_;
    CanResourceTxMailbox* mailbox_[NUMBER_OF_TX_MAILBOXS];

    /**
     * @brief TX complite semaphore.
     */    
    sys::Semaphore mailboxSem_;    

    /**
     * @brief Target CPU interrupt resource.
     */
    lib::UniquePointer<api::CpuInterrupt> mailboxInt_;
    
    /**
     * @brief Target CPU interrupt routine.
     */        
    CanResourceTxMailboxRoutine mailboxIsr_;

};

} // namespace drv
} // namespace eoos
#endif // DRV_CANRESOURCETX_HPP_
