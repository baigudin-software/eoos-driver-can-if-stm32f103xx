/**
 * @file      drv.CanResource.hpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2023-2024, Sergey Baigudin, Baigudin Software
 */
#ifndef DRV_CANRESOURCE_HPP_
#define DRV_CANRESOURCE_HPP_

#include "api.Supervisor.hpp"
#include "lib.NonCopyable.hpp"
#include "drv.Can.hpp"
#include "cpu.Registers.hpp"
#include "lib.NoAllocator.hpp"
#include "lib.Mutex.hpp"
#include "lib.Guard.hpp"

namespace eoos
{
namespace drv
{

/**
 * @class CanResource
 * @brief CAN device resource.
 * 
 * @tparam A Heap memory allocator class.
 */
template <class A>
class CanResource : public lib::NonCopyable<A>, public Can
{
    typedef lib::NonCopyable<A> Parent;

public:

    /**
     * @struct Data
     * @brief Global data for all these resources;
     */
    struct Data
    {
        /**
         * @brief Constructor.
         *
         * @param reg Target CPU register model.  
         * @param svc Supervisor call to the system.
         */
        Data(cpu::Registers& areg, api::Supervisor& asvc);
        
        /**
         * @brief Target CPU register model.
         */
        cpu::Registers& reg;

        /**
         * @brief Supervisor call to the system.
         */        
        api::Supervisor& svc;

        /**
         * @brief All the resource guard.
         */        
        lib::Mutex<lib::NoAllocator> mutex;

    };

    /**
     * @brief Constructor.
     *
     * @param data Global data for all resource objects.
     * @param config Configuration of the driver resource.     
     */
    CanResource(Data& data, Config const& config);
    
    /** 
     * @brief Destructor.
     */                               
    virtual ~CanResource();
    
    /**
     * @copydoc eoos::api::Object::isConstructed()
     */
    virtual bool_t isConstructed() const;  
        
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
     * @brief Tests if given number is correct.
     *
     * @return True if correct.
     */
    bool_t isNumberValid();
    
    /**
     * @brief Initializes the hardware.
     *
     * @return True if initialized.
     */
    bool_t initialize();

    /**
     * @brief Initializes the hardware.
     */
    void deinitialize();
    
    /**
     * @brief Enables or disables USART clock peripheral.
     *
     * bxCAN is APB1 peripheral, enable PCLK1 of 36 MHz for SYSCLK of 72 MHz.
     *
     * @param enable True to enable and false to disable.
     * @todo Check SYSCLK is 72 MHz and enable the clock if only.
     */
    void enableClock(bool_t enable);    
    
    /**
     * @brief Global data for all these objects;
     */
    Data& data_;
        
    /**
     * @brief Configuration of the resource.
     */
    Config config_;  

    /**
     * @brief CAN registers.
     */
    cpu::reg::Can* reg_;
    
    /**
     * @brief This resource mutex.
     */
    lib::Mutex<A> mutex_;
    
};

template <class A>
CanResource<A>::CanResource(Data& data, Config const& config)
    : lib::NonCopyable<A>()
    , data_( data )
    , config_( config )
    , reg_(  data_.reg.can[config_.number]  )    
    , mutex_() {
    bool_t const isConstructed( construct() );
    setConstructed( isConstructed );
}    

template <class A>
CanResource<A>::~CanResource()
{
    deinitialize();
}

template <class A>
bool_t CanResource<A>::isConstructed() const
{
    return Parent::isConstructed();
}

template <class A>
bool_t CanResource<A>::construct()
{
    bool_t res( false );
    do 
    {
        if( !isConstructed() )
        {
            break;
        }
        if( !isNumberValid() )
        {
            break;
        }
        if( !initialize() )
        {
            break;
        }
        res = true;
    } while(false);
    return res;    
}

template<class A>
bool_t CanResource<A>::isNumberValid()
{
    return (NUMBER_CAN1 == config_.number) || (NUMBER_CAN1 == config_.number);
}

template <class A>
bool_t CanResource<A>::initialize()
{
    bool_t res( false );
    do 
    {
        lib::Guard<A> const guard(data_.mutex);
        enableClock(true);
        // Exit sleep mode
        reg_->mcr.bit.sleep = 0;
        // Enter to the Initialization mode
        reg_->mcr.bit.inrq = 1;
        // Wait the acknowledge
        uint32_t timeout( 0x0000FFFF );
        while( true )
        {
            if( reg_->msr.bit.inak == 1 )
            {
                break;
            }
            if( timeout-- == 0 )
            {
                break;
            }
        }
        if( timeout == 0 )
        {
            break;
        }
        // Set master control register
        cpu::reg::Can::Mcr mcr(reg_->mcr.value);        
        mcr.bit.ttcm = config_.reg.mcr.ttcm;
        mcr.bit.abom = config_.reg.mcr.abom;
        mcr.bit.awum = config_.reg.mcr.awum;
        mcr.bit.nart = config_.reg.mcr.nart;
        mcr.bit.rflm = config_.reg.mcr.rflm;
        mcr.bit.txfp = config_.reg.mcr.txfp;
        reg_->mcr.value = mcr.value;
        // Set the bit timing register
        cpu::reg::Can::Btr btr(reg_->btr.value);
        btr.bit.brp  = config_.reg.btr.brp;
        btr.bit.ts1  = config_.reg.btr.ts1;
        btr.bit.ts2  = config_.reg.btr.ts2;
        btr.bit.sjw  = config_.reg.btr.sjw;
        btr.bit.lbkm = config_.reg.btr.lbkm;
        btr.bit.silm = config_.reg.btr.silm;
        reg_->btr.value = btr.value;
        // Enter to the Normal mode
        reg_->mcr.bit.inrq = 0;
        // Wait the acknowledge
        timeout = 0x0000FFFF;
        while( true )
        {
            if( reg_->msr.bit.inak == 0 )
            {
                break;
            }
            if( timeout-- == 0 )
            {
                break;
            }
        }
        if( timeout == 0 )
        {
            break;
        }
        res = true;
    } while(false);
    return res;
}

template <class A>
void CanResource<A>::deinitialize()
{
    lib::Guard<A> const guard(data_.mutex);
    enableClock(false);
}

template <class A>
void CanResource<A>::enableClock(bool_t enable)
{
    uint32_t en = (enable) ? 1 : 0;
    switch( config_.number )
    {
        case NUMBER_CAN1:
        {
            data_.reg.rcc->apb1enr.bit.can1en = en;
            break;
        }
        case NUMBER_CAN2:
        {
            data_.reg.rcc->apb1enr.bit.can2en = en;
            break;
        }
        default:
        {
            break;
        }
    }
}

template <class A>
CanResource<A>::Data::Data(cpu::Registers& areg, api::Supervisor& asvc)
    : reg( areg )
    , svc( asvc )
    , mutex() {
}

} // namespace drv
} // namespace eoos
#endif // DRV_USARTRESOURCE_HPP_
