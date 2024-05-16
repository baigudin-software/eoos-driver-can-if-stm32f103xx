/**
 * @file      drv.CanResourceRxFifo.hpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2024, Sergey Baigudin, Baigudin Software
 */
#ifndef DRV_CANRESOURCERXFIFO_HPP_
#define DRV_CANRESOURCERXFIFO_HPP_

#include "lib.NonCopyable.hpp"
#include "lib.NoAllocator.hpp"
#include "api.Supervisor.hpp"
#include "api.Runnable.hpp"
#include "drv.Can.hpp"
#include "lib.UniquePointer.hpp"
#include "lib.Fifo.hpp"
#include "sys.Mutex.hpp"
#include "sys.Semaphore.hpp"
#include "cpu.Registers.hpp"
#include "cpu.Interrupt.hpp"

namespace eoos
{
namespace drv
{

/**
 * @class CanResourceRxFifo
 * @brief CAN RX HW FIFO.
 */
class CanResourceRxFifo : public lib::NonCopyable<lib::NoAllocator>, public api::Runnable
{
    typedef lib::NonCopyable<lib::NoAllocator> Parent;

public:

    /**
     * @brief Number of TX mailboxs.
     */    
    static const int32_t NUMBER_OF_RX_FIFOS = 2;

    /**
     * @brief Constructor.
     *
     * @param number FIFO RX index.   
     * @param isLocked FIFO locked mode flag.     
     * @param reg CAN registers.
     * @param svc Supervisor call to the system.     
     */
    CanResourceRxFifo(Can::RxFifo index, bool_t isLocked, cpu::reg::Can* reg, api::Supervisor& svc);
    
    /** 
     * @brief Destructor.
     */
    virtual ~CanResourceRxFifo();
    
    /**
     * @copydoc eoos::api::Object::isConstructed()
     */
    virtual bool_t isConstructed() const;
    
    /**
     * @brief Receives a message.
     *
     * The function receives a message to the passed message structure.
     * If no messages in receiving buffers, the function waits till
     * a message comes. 
     *
     * @param message A message structure to receive to it.
     * @return True if a message is received successfully.
     */
    bool_t receive(Can::Message* message);
        
protected:

    using Parent::setConstructed;

private:

    /**
     * @copydoc eoos::api::Runnable::start()
     */
    virtual void start();

    /**
     * @brief Constructs this object.
     *
     * @return true if object has been constructed successfully.
     */
    bool_t construct();
    
    /**
     * @brief Initializes the FIFO interrupt.
     *
     * @return true if interrupt has been initialized successfully.
     */    
    bool_t initializeInterrupt();
    
    /**
     * @enum Exception
     * @brief CAN Exception numbers.
     */    
    enum Exception
    {
        EXCEPTION_CAN1_RX0 = cpu::Interrupt<lib::NoAllocator>::EXCEPTION_USB_LP_CAN1_RX0, ///< FIFO 0 interrupt
        EXCEPTION_CAN1_RX1 = cpu::Interrupt<lib::NoAllocator>::EXCEPTION_CAN1_RX1,        ///< FIFO 1 interrupt
    };
    
    /**
     * @brief Number of TX mailboxs.
     */    
    static const int32_t NUMBER_OF_RX_MAILBOXS_IN_FIFO = 3;
    
    /**
     * @brief SW FIFO.
     */
    lib::Fifo<Can::Message,NUMBER_OF_RX_MAILBOXS_IN_FIFO,lib::NoAllocator> fifo_;
    
    /**
     * @brief This resource mutex.
     */
    sys::Mutex mutex_;
    
    /**
     * @brief RX complite semaphore.
     */    
    sys::Semaphore sem_;
    
    /**
     * @brief CAN FIFO index.
     */    
    Can::RxFifo index_;
        
    /**
     * @brief CAN registers.
     */
    cpu::reg::Can* reg_;
    
    /**
     * @brief Supervisor call to the system.
     */        
    api::Supervisor& svc_;
    
    /**
     * @brief Target CPU interrupt resource.
     */
    lib::UniquePointer<api::CpuInterrupt> int_;

};

} // namespace drv
} // namespace eoos
#endif // DRV_CANRESOURCERXFIFO_HPP_
