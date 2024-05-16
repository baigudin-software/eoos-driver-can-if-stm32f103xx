/**
 * @file      drv.CanResourceTxMailboxRoutine.hpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2024, Sergey Baigudin, Baigudin Software
 */
#ifndef DRV_CANRESOURCETXMAILBOXROUTINE_HPP_
#define DRV_CANRESOURCETXMAILBOXROUTINE_HPP_

#include "lib.NonCopyable.hpp"
#include "lib.NoAllocator.hpp"
#include "drv.CanResourceTxMailbox.hpp"
#include "sys.Semaphore.hpp"

namespace eoos
{
namespace drv
{

/**
 * @class CanResourceTxMailboxRoutine
 * @brief CAN device interrupt TX resource.
 */
class CanResourceTxMailboxRoutine : public lib::NonCopyable<lib::NoAllocator>, public api::Runnable
{
    typedef lib::NonCopyable<lib::NoAllocator> Parent;

public:

    /**
     * @brief Constructor.
     *
     * @param mailbox TX mailboxs.
     */
    CanResourceTxMailboxRoutine(CanResourceTxMailbox** mailbox, sys::Semaphore& mailboxSem);
    
    /** 
     * @brief Destructor.
     */
    virtual ~CanResourceTxMailboxRoutine();
    
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
     * @brief Number of TX mailboxs.
     */    
    static const int32_t NUMBER_OF_TX_MAILBOXS = CanResourceTxMailbox::NUMBER_OF_TX_MAILBOXS;
    
    /**
     * @brief TX mailbox.
     */    
    CanResourceTxMailbox** mailbox_;

    /**
     * @brief TX complite semaphore.
     */    
    sys::Semaphore& mailboxSem_;

};

} // namespace drv
} // namespace eoos
#endif // DRV_CANRESOURCETXMAILBOXROUTINE_HPP_
