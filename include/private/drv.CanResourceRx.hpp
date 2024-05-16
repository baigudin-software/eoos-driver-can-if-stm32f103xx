/**
 * @file      drv.CanResourceRx.hpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2023-2024, Sergey Baigudin, Baigudin Software
 */
#ifndef DRV_CANRESOURCERX_HPP_
#define DRV_CANRESOURCERX_HPP_

#include "lib.NonCopyable.hpp"
#include "lib.NoAllocator.hpp"
#include "drv.Can.hpp"
#include "drv.CanResourceRxFifo.hpp"
#include "sys.Mutex.hpp"

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
     * @param config Configuration of the driver resource.          
     * @param reg    CAN registers.
     * @param svc    Supervisor call to the system.
     */
    CanResourceRx(Can::Config const& config, cpu::reg::Can* reg, api::Supervisor& svc);
    
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
    bool_t receive(Can::Message* message, Can::RxFifo fifo);
    
    /**
     * @copydoc eoos::drv::Can::setReceiveFilter()
     */
    bool_t setReceiveFilter(Can::RxFilter const& filter);
    
protected:

    using Parent::setConstructed;

private:

    /**
     * @brief Constructs this object.
     *
     * @return true if object has been constructed successfully.
     */
    bool_t construct();
    
    /**
     * @brief CAN registers.
     */
    cpu::reg::Can* reg_;

    /**
     * @brief This resource mutex.
     */
    sys::Mutex mutex_;

    /**
     * @brief RX FIFOs.
     */        
    CanResourceRxFifo fifo0_;
    CanResourceRxFifo fifo1_;

};

} // namespace drv
} // namespace eoos
#endif // DRV_CANRESOURCERX_HPP_
