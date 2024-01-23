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
#include "drv.CanResourceMailbox.hpp"
#include "drv.CanResourceRoutineTx.hpp"
#include "cpu.Registers.hpp"
#include "cpu.Interrupt.hpp"
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
         * @param areg Target CPU register model.  
         * @param asvc Supervisor call to the system.
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
    
    /**
     * @copydoc eoos::drv::Can::transmit()
     */
    virtual TxHandler* transmit(TxMessage const& message);
        
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
     * @enum Exception
     * @brief CAN Exception numbers.
     */    
    enum Exception
    {
        EXCEPTION_CAN1_TX  = cpu::Interrupt<A>::EXCEPTION_USB_HP_CAN1_TX,  ///< Transmit interrupt
        EXCEPTION_CAN1_RX0 = cpu::Interrupt<A>::EXCEPTION_USB_LP_CAN1_RX0, ///< FIFO 0 interrupt
        EXCEPTION_CAN1_RX1 = cpu::Interrupt<A>::EXCEPTION_CAN1_RX1,        ///< FIFO 1 interrupt
        EXCEPTION_CAN1_SCE = cpu::Interrupt<A>::EXCEPTION_CAN1_SCE         ///< Status change error interrupt
    };
    
    /**
     * @brief Number of TX mailboxs.
     */    
    static const int32_t NUMBER_OF_TX_MAILBOXS = 3;
    
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
    
    /**
     * @brief TX ISR.
     */    
    CanResourceRoutineTx isrTx_;
    
    /**
     * @brief Target CPU interrupt resource.
     */    
    api::CpuInterrupt* intTx_;    

    /**
     * @brief TX mailboxs.
     */    
    CanResourceMailbox  txMailbox0_;
    CanResourceMailbox  txMailbox1_;
    CanResourceMailbox  txMailbox2_;
    CanResourceMailbox* txMailbox_[NUMBER_OF_TX_MAILBOXS];
};

template <class A>
CanResource<A>::CanResource(Data& data, Config const& config)
    : lib::NonCopyable<A>()
    , Can()
    , data_( data )
    , config_( config )
    , reg_(  data_.reg.can[config_.number]  )    
    , mutex_()
    , isrTx_()
    , intTx_( NULLPTR )
    , txMailbox0_(0, reg_)
    , txMailbox1_(1, reg_)
    , txMailbox2_(2, reg_) {
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
Can::TxHandler* CanResource<A>::transmit(TxMessage const& message)
{
    CanResourceMailbox* handler( NULLPTR );
    if( isConstructed() )
    {
        lib::Guard<A> const guard(mutex_);
        for(int32_t i=0; i<NUMBER_OF_TX_MAILBOXS; i++)
        {
            if( txMailbox_[i]->isEmpty() )
            {
                handler = txMailbox_[i];
                break;
            }
        }
        if( handler != NULLPTR )
        {
            handler->transmit(message);
        }
    }
    return handler;
}

template <class A>
bool_t CanResource<A>::construct()
{
    txMailbox_[0] = &txMailbox0_;
    txMailbox_[1] = &txMailbox1_;
    txMailbox_[2] = &txMailbox2_;    
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
    return NUMBER_CAN1 == config_.number;
}

template <class A>
bool_t CanResource<A>::initialize()
{
    bool_t res( false );
    do 
    {
        lib::Guard<A> const guard(data_.mutex);
        if( !checkClocks() )
        {
            break;
        }
        if( !enableClock(true) )
        {
            break;
        }
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
        btr.bit.lbkm = config_.reg.btr.lbkm;
        btr.bit.silm = config_.reg.btr.silm;
        reg_->btr.value = btr.value;
        // Set bus bit rate
        if( !setBitRate() )
        {
            break;
        }
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
        // Set ISR for Transmit mailbox empty interrupt enable generated 
        // when RQCPx (Request completed mailbox) bit is set.
        intTx_ = data_.svc.getProcessor().getInterruptController().createResource(isrTx_, EXCEPTION_CAN1_TX);
        if( !Parent::isConstructed(intTx_) )
        {
            break;
        }
        intTx_->enable();
        reg_->ier.bit.tmeie = 1;
        // Complite successfully
        res = true;
    } while(false);
    return res;
}

template <class A>
void CanResource<A>::deinitialize()
{
    lib::Guard<A> const guard(data_.mutex);
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
            data_.reg.rcc->apb1enr.bit.can1en = en;
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
    cpu::reg::Can::Btr reg(reg_->btr.value);
    reg.bit.brp  = cfg.bit.brp;
    reg.bit.ts1  = cfg.bit.ts1;
    reg.bit.ts2  = cfg.bit.ts2;
    reg.bit.sjw  = cfg.bit.sjw;
    reg_->btr.value = reg.value;
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
#endif // DRV_USARTRESOURCE_HPP_
