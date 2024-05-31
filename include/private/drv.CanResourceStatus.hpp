/**
 * @file      drv.CanResourceStatus.hpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2024, Sergey Baigudin, Baigudin Software
 */
#ifndef DRV_CANRESOURCESTATUS_HPP_
#define DRV_CANRESOURCESTATUS_HPP_

#include "lib.NonCopyable.hpp"
#include "lib.NoAllocator.hpp"
#include "api.Supervisor.hpp"
#include "api.Runnable.hpp"
#include "lib.UniquePointer.hpp"
#include "cpu.Registers.hpp"
#include "cpu.Interrupt.hpp"

namespace eoos
{
namespace drv
{

/**
 * @class CanResourceStatus
 * @brief CAN status change errror resource.
 */
class CanResourceStatus : public lib::NonCopyable<lib::NoAllocator>, public api::Runnable
{
    typedef lib::NonCopyable<lib::NoAllocator> Parent;

public:
    
    static const int32_t NUMBER_OF_RX_FIFOS = 2;

    /**
     * @brief Constructor.
     *
     * @param reg CAN registers.
     * @param svc Supervisor call to the system.     
     */
    CanResourceStatus(cpu::reg::Can* reg, api::Supervisor& svc);
    
    /** 
     * @brief Destructor.
     */
    virtual ~CanResourceStatus();
    
    /**
     * @copydoc eoos::api::Object::isConstructed()
     */
    virtual bool_t isConstructed() const;
            
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
        EXCEPTION_CAN1_SCE = cpu::Interrupt<lib::NoAllocator>::EXCEPTION_CAN1_SCE, ///< Status change error interrupt
    };

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
#endif // DRV_CANRESOURCESTATUS_HPP_
