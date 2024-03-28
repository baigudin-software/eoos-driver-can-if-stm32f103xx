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
#include "drv.CanResourceTx.hpp"
#include "drv.CanResourceRx.hpp"
#include "cpu.Registers.hpp"
#include "sys.Mutex.hpp"
#include "lib.Register.hpp"
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
         * @param areg Target CPU register model.  
         * @param asvc ` call to the system.
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
        sys::Mutex mutex;

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
    
    /**
     * @copydoc eoos::drv::Can::transmit()
     */
    virtual bool_t transmit(TxMessage const& message);
	
    /**
     * @copydoc eoos::drv::Can::receive()
     */
    virtual bool_t receive(RxMessage* message);
    
    /**
     * @copydoc eoos::drv::Can::setReceiveFilter()
     */
    virtual bool_t setReceiveFilter(RxFilter const& filter);
        
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
     * @brief Deinitializes the hardware.
     */
    void deinitialize();

    /**
     * @brief Checks system clocks.
     *
     * @return True if clocks is correct.
     */
    bool_t checkClocks();
    
    /**
     * @brief Enables or disables clock peripheral.
     *
     * bxCAN is APB1 peripheral, enable PCLK1 of 36 MHz for SYSCLK of 72 MHz.
     *
     * @param enable True to enable and false to disable.
     * @return True if clock is set.
     */
    bool_t enableClock(bool_t enable);    

    /**
     * @brief Set CAN bus bit rate.
     *
     * @ref http://www.bittiming.can-wiki.info/ 
     * @return True if bit rate is set.
     */
    bool_t setBitRate();
    
    /**
     * @brief Number of RX FIFOs.
     */    
    static const int32_t NUMBER_OF_RX_FIFOS = 2;
    
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
     * @brief TX resource.
     */        
    CanResourceTx tx_;

    /**
     * @brief RX resource.
     */        
    CanResourceRx rx_;

};

template <class A>
CanResource<A>::CanResource(Data& data, Config const& config)
    : lib::NonCopyable<A>()
    , Can()
    , data_( data )
    , config_( config )
    , reg_(  data_.reg.can[config_.number]  )    
    , tx_( reg_, data_.svc )
    , rx_( reg_, data_.svc ) {
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
bool_t CanResource<A>::transmit(TxMessage const& message)
{
    return tx_.transmit(message);
}

template <class A>
bool_t CanResource<A>::receive(RxMessage* message)
{
    return rx_.receive(message);
}

template <class A>
bool_t CanResource<A>::setReceiveFilter(RxFilter const& filter)
{
    return rx_.setReceiveFilter(filter);
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
        if( !tx_.isConstructed() )
        {
            break;
        }
        if( !rx_.isConstructed() )
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
    return NUMBER_CAN1 == config_.number;
}

template <class A>
bool_t CanResource<A>::initialize()
{
    bool_t res( false );
    do 
    {
        lib::Guard<A> const guard(data_.mutex);
        lib::Register<cpu::reg::Can::Mcr> mcr( reg_->mcr );
        lib::Register<cpu::reg::Can::Msr> msr( reg_->msr );
        lib::Register<cpu::reg::Can::Btr> btr( reg_->btr );
        lib::Register<cpu::reg::Can::Ier> ier( reg_->ier );
        if( !checkClocks() )
        {
            break;
        }
        // Enable clock peripheral
        if( !enableClock(true) )
        {
            break;
        }
        // Exit sleep mode
        mcr.fetch().bit().sleep = 0;
        mcr.commit();
        // Enter to the Initialization mode
        mcr.fetch().bit().inrq = 1;
        mcr.commit();
        // Wait the acknowledge
        uint32_t timeout( 0x0000FFFF );
        while( true )
        {
            if( msr.fetch().bit().inak == 1 )
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
        mcr.fetch();
        mcr.bit().dbf  = config_.reg.mcr.dbf;
        mcr.bit().ttcm = config_.reg.mcr.ttcm;
        mcr.bit().abom = config_.reg.mcr.abom;
        mcr.bit().awum = config_.reg.mcr.awum;
        mcr.bit().nart = config_.reg.mcr.nart;
        mcr.bit().rflm = config_.reg.mcr.rflm;
        mcr.bit().txfp = config_.reg.mcr.txfp;
        mcr.commit();
        // Set debug mode
        if( config_.reg.mcr.dbf == 1 )
        {
            lib::Register<cpu::reg::Dbg::Cr> cr( data_.reg.dbg->cr );
            cr.fetch().bit().dbgcan1stop = 1;
            cr.commit();
        }
        // Set the bit timing register
        btr.fetch();
        btr.bit().lbkm = config_.reg.btr.lbkm;
        btr.bit().silm = config_.reg.btr.silm;
        btr.commit();
        // Set bus bit rate
        if( !setBitRate() )
        {
            break;
        }
        // Enter to the Normal mode
        mcr.fetch().bit().inrq = 0;
        mcr.commit();
        // Wait the acknowledge
        timeout = 0x0000FFFF;
        while( true )
        {
            if( msr.fetch().bit().inak == 0 )
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
        // Enable interrupts
        ier.fetch();
        ier.bit().tmeie  = 1;
        ier.bit().fmpie0 = 1;
        ier.bit().ffie0  = 1;
        ier.bit().fovie0 = 1;
        ier.bit().fmpie1 = 1;
        ier.bit().ffie1  = 1;
        ier.bit().fovie1 = 1;
        ier.commit();
        // Complite successfully
        res = true;
    } while(false);
    return res;
}

template <class A>
void CanResource<A>::deinitialize()
{
    lib::Guard<A> const guard(data_.mutex);
    lib::Register<cpu::reg::Can::Ier> ier( reg_->ier );
    // Disable interrupts
    ier.fetch();
    ier.bit().tmeie  = 0;
    ier.bit().fmpie0 = 0;
    ier.bit().ffie0  = 0;
    ier.bit().fovie0 = 0;
    ier.bit().fmpie1 = 0;
    ier.bit().ffie1  = 0;
    ier.bit().fovie1 = 0;
    ier.commit();
    // Disable clock peripheral.        
    static_cast<void>(enableClock(false));
}

template <class A>
bool_t CanResource<A>::checkClocks()
{
    bool_t res( false );
    if( data_.svc.getProcessor().getPllController().getCpuClock() == 72000000 )
    {
        res = true;
    }
    return res;
}

template <class A>
bool_t CanResource<A>::enableClock(bool_t enable)
{
    bool_t res(true);
    uint32_t en = (enable) ? 1 : 0;
    switch( config_.number )
    {
        case NUMBER_CAN1:
        {
            lib::Register<cpu::reg::Rcc::Apb1enr> apb1enr( data_.reg.rcc->apb1enr );
            apb1enr.fetch().bit().can1en = en;
            apb1enr.commit();
            break;
        }
        default:
        {
            res = false;
            break;
        }
    }
    return res;
}

template <class A>
bool_t CanResource<A>::setBitRate()
{
    uint32_t const value[2][9] = {
        {   // For CANopen
            0x001e0001, // 1000 
            0x001b0002, // 800  
            0x001e0003, // 500  
            0x001c0008, // 250  
            0x001c0011, // 125  
            0x001e0013, // 100  
            0x001c002c, // 50   
            0x001e0063, // 20   
            0x001c00e0  // 10   
        }, 
        {   // For ARINC 825
            0x003c0001, // 1000 
            0x00390002, // 800  
            0x003c0003, // 500  
            0x003a0008, // 250  
            0x003a0011, // 125  
            0x004d0011, // 100  
            0x004d0023, // 50   
            0x004d0059, // 20   
            0x003a00e0  // 10   
        }
    };
    cpu::reg::Can::Btr cfg(value[config_.samplePoint][config_.bitRate]);    
    lib::Register<cpu::reg::Can::Btr> btr( reg_->btr );
    btr.fetch();
    btr.bit().brp = cfg.bit.brp;
    btr.bit().ts1 = cfg.bit.ts1;
    btr.bit().ts2 = cfg.bit.ts2;
    btr.bit().sjw = cfg.bit.sjw;
    btr.commit();
    return true;
}

template <class A>
CanResource<A>::Data::Data(cpu::Registers& areg, api::Supervisor& asvc)
    : reg( areg )
    , svc( asvc )
    , mutex() {
}

} // namespace drv
} // namespace eoos
#endif // DRV_CANRESOURCE_HPP_
