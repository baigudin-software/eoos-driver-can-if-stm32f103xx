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
#include "drv.CanResourceMailboxRoutine.hpp"
#include "drv.CanResourceFifoRoutine.hpp"
#include "cpu.Registers.hpp"
#include "cpu.Interrupt.hpp"
#include "sys.Mutex.hpp"
#include "sys.Semaphore.hpp"
#include "lib.Register.hpp"
#include "lib.Guard.hpp"
#include "lib.UniquePointer.hpp"

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
    static const int32_t NUMBER_OF_TX_MAILBOXS = CanResourceMailbox::NUMBER_OF_TX_MAILBOXS;

    /**
     * @brief Number of TX mailboxs.
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
     * @brief This resource mutex.
     */
    sys::Mutex mutex_;

    /**
     * @brief TX mailboxs.
     */    
    CanResourceMailbox  mailbox0_;
    CanResourceMailbox  mailbox1_;
    CanResourceMailbox  mailbox2_;
    CanResourceMailbox* mailbox_[NUMBER_OF_TX_MAILBOXS];

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
    CanResourceMailboxRoutine mailboxIsr_;

    /**
     * @brief Target CPU interrupt resource.
     */
    lib::UniquePointer<api::CpuInterrupt> fifoInt_[NUMBER_OF_RX_FIFOS];

    /**
     * @brief Target CPU interrupt routine.
     */        
    CanResourceFifoRoutine  fifoIsr0_;
    CanResourceFifoRoutine  fifoIsr1_;
    CanResourceFifoRoutine* fifoIsr_[NUMBER_OF_RX_FIFOS];
};

template <class A>
CanResource<A>::CanResource(Data& data, Config const& config)
    : lib::NonCopyable<A>()
    , Can()
    , data_( data )
    , config_( config )
    , reg_(  data_.reg.can[config_.number]  )    
    , mutex_()
    , mailbox0_( 0, reg_ )
    , mailbox1_( 1, reg_ )
    , mailbox2_( 2, reg_ )
    , mailboxSem_( NUMBER_OF_TX_MAILBOXS, NUMBER_OF_TX_MAILBOXS )    
    , mailboxInt_( NULLPTR )
    , mailboxIsr_( mailbox_, mailboxSem_ )
    , fifoIsr0_( CanResourceFifoRoutine::INDEX_FIFO0 )
    , fifoIsr1_( CanResourceFifoRoutine::INDEX_FIFO1 ) {
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
    bool_t res( false );
    if( isConstructed() && mailboxSem_.acquire() )
    {
        lib::Guard<A> const guard(mutex_);
        for(int32_t i(0); i<NUMBER_OF_TX_MAILBOXS; i++)
        {
            if( mailbox_[i]->isEmpty() )
            {
                res = mailbox_[i]->transmit(message);
                break;
            }
        }
    }
    return res;
}

template <class A>
bool_t CanResource<A>::receive(RxMessage* message)
{
	return false;
}

template <class A>
bool_t CanResource<A>::setReceiveFilter(RxFilter const& filter)
{
    return false;
}

template <class A>
bool_t CanResource<A>::construct()
{
    mailbox_[0] = &mailbox0_;
    mailbox_[1] = &mailbox1_;
    mailbox_[2] = &mailbox2_;

    fifoIsr_[0] = &fifoIsr0_;
    fifoIsr_[1] = &fifoIsr1_;

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
        if( !mutex_.isConstructed() )
        {
            break;
        }
        if( !mailbox0_.isConstructed() )
        {
            break;
        }
        if( !mailbox1_.isConstructed() )
        {
            break;
        }
        if( !mailbox2_.isConstructed() )
        {
            break;
        }
        if( !mailboxSem_.isConstructed() )
        {
            break;            
        }
        if( !mailboxIsr_.isConstructed() )
        {
            break;
        }
        if( !fifoIsr0_.isConstructed() )
        {
            break;
        }
        if( !fifoIsr1_.isConstructed() )
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
        // Get interrupt controller
        api::CpuInterruptController& ic( data_.svc.getProcessor().getInterruptController() );
        // Set ISR for Transmit mailbox empty interrupt enable generated 
        // when RQCPx (Request completed mailbox) bit is set.
        mailboxInt_.reset( ic.createResource(mailboxIsr_, EXCEPTION_CAN1_TX) );
        if( mailboxInt_.isNull() )
        {
            break;
        }
        if( !mailboxInt_->isConstructed()  )
        { 
            break;
        }
        mailboxInt_->enable();
        // Set ISR for Receive FIFOs
        bool_t isFifoIntCreated( true );
        for(int32_t i(0); i<NUMBER_OF_RX_FIFOS; i++)
        {
            int32_t source( -1 );
            switch(i)
            {
                case 0:
                {
                    source = EXCEPTION_CAN1_RX0;
                    break;
                }
                case 1:
                {
                    source = EXCEPTION_CAN1_RX1;
                    break;
                }
                default:
                {
                    isFifoIntCreated = false;
                    break;
                }
            }
            if( !isFifoIntCreated )
            {
                break;
            }
            fifoInt_[i].reset( ic.createResource(*fifoIsr_[i], source) );
            if( fifoInt_[i].isNull() )
            {
                isFifoIntCreated = false;
                break;
            }
            if( !fifoIsr_[i]->isConstructed() )
            {
                isFifoIntCreated = false;
                break;
            }
            fifoInt_[i]->enable();
        }
        if( !isFifoIntCreated )
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
    // Unset ISR for Transmit mailbox empty interrupt enable generated 
    mailboxInt_->disable();
    // Unset ISR for Receive FIFOs
    for(int32_t i(0); i<NUMBER_OF_RX_FIFOS; i++)
    {
        fifoInt_[i]->disable();
    }    
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
