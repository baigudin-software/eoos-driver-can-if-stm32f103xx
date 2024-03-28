/**
 * @file      drv.CanResourceRx.hpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2023-2024, Sergey Baigudin, Baigudin Software
 */
#ifndef DRV_CANRESOURCERX_HPP_
#define DRV_CANRESOURCERX_HPP_

#include "lib.NonCopyable.hpp"
#include "lib.NoAllocator.hpp"
#include "api.Supervisor.hpp"
#include "drv.Can.hpp"
#include "drv.CanResourceRxFifoRoutine.hpp"
#include "cpu.Interrupt.hpp"
#include "sys.Mutex.hpp"
#include "lib.UniquePointer.hpp"

namespace eoos
{
namespace drv
{

/**
 * @class CanResourceRx
 * @brief CAN RX device resource.
 */
class CanResourceRx : public lib::NonCopyable<lib::NoAllocator>
{
    typedef lib::NonCopyable<lib::NoAllocator> Parent;

public:

    /**
     * @brief Constructor.
     *
     * @param reg CAN registers.
     * @param svc Supervisor call to the system.
     */
    CanResourceRx(cpu::reg::Can* reg, api::Supervisor& svc);
    
    /** 
     * @brief Destructor.
     */
    virtual ~CanResourceRx();
    
    /**
     * @copydoc eoos::api::Object::isConstructed()
     */
    virtual bool_t isConstructed() const;
    
    /**
     * @copydoc eoos::drv::Can::receive()
     */
    bool_t receive(Can::RxMessage* message);
    
    /**
     * @copydoc eoos::drv::Can::setReceiveFilter()
     */
    bool_t setReceiveFilter(Can::RxFilter const& filter);
    
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
        EXCEPTION_CAN1_RX0 = cpu::Interrupt<lib::NoAllocator>::EXCEPTION_USB_LP_CAN1_RX0, ///< FIFO 0 interrupt
        EXCEPTION_CAN1_RX1 = cpu::Interrupt<lib::NoAllocator>::EXCEPTION_CAN1_RX1,        ///< FIFO 1 interrupt
    };
    
    /**
     * @brief Number of RX FIFOs.
     */    
    static const int32_t NUMBER_OF_RX_FIFOS = 2;
    
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
     * @brief Target CPU interrupt resource.
     */
    lib::UniquePointer<api::CpuInterrupt> fifoInt_[NUMBER_OF_RX_FIFOS];

    /**
     * @brief Target CPU interrupt routine.
     */        
    CanResourceRxFifoRoutine  fifoIsr0_;
    CanResourceRxFifoRoutine  fifoIsr1_;
    CanResourceRxFifoRoutine* fifoIsr_[NUMBER_OF_RX_FIFOS];

};

} // namespace drv
} // namespace eoos
#endif // DRV_CANRESOURCETX_HPP_
