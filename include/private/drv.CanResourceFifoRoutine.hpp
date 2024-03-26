/**
 * @file      drv.CanResourceFifoRoutine.hpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2024, Sergey Baigudin, Baigudin Software
 */
#ifndef DRV_CANRESOURCEFIFOROUTINE_HPP_
#define DRV_CANRESOURCEFIFOROUTINE_HPP_

#include "lib.NonCopyable.hpp"
#include "lib.NoAllocator.hpp"
#include "drv.CanResourceMailbox.hpp"
#include "sys.Semaphore.hpp"

namespace eoos
{
namespace drv
{

/**
 * @class CanResourceFifoRoutine
 * @brief CAN device interrupt RX resource.
 */
class CanResourceFifoRoutine : public lib::NonCopyable<lib::NoAllocator>, public api::Runnable
{
    typedef lib::NonCopyable<lib::NoAllocator> Parent;

public:

    /**
     * @enum Index
     * @brief CAN FIFO index.
     */
    enum Index
    {
        INDEX_FIFO0 = 0,
        INDEX_FIFO1
    };

    /**
     * @brief Constructor.
     *
     * @param FIFO RX index.
     */
    CanResourceFifoRoutine(Index index);
    
    /** 
     * @brief Destructor.
     */
    virtual ~CanResourceFifoRoutine();
    
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
     * Constructs this object.
     *
     * @return true if object has been constructed successfully.
     */
    bool_t construct();
    
    /**
     * @brief CAN FIFO index.
     */    
    int32_t index_;

};

} // namespace drv
} // namespace eoos
#endif // DRV_CANRESOURCEFIFOROUTINE_HPP_
