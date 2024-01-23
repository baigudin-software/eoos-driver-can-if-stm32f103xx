/**
 * @file      drv.CanResourceRoutineTx.hpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2024, Sergey Baigudin, Baigudin Software
 */
#ifndef DRV_CANRESOURCEROUTINETX_HPP_
#define DRV_CANRESOURCEROUTINETX_HPP_

#include "lib.NonCopyable.hpp"
#include "api.Runnable.hpp"
#include "lib.NoAllocator.hpp"

namespace eoos
{
namespace drv
{

/**
 * @class CanResourceRoutineTx
 * @brief CAN TX Mailbox interrupt service routine.
 */
class CanResourceRoutineTx : public lib::NonCopyable<lib::NoAllocator>, public api::Runnable
{
    typedef lib::NonCopyable<lib::NoAllocator> Parent;

public:

    /**
     * @brief Constructor.
     */
    CanResourceRoutineTx();

    /**
     * @brief Destructor.
     */
    virtual ~CanResourceRoutineTx();

    /**
     * @copydoc eoos::api::Object::isConstructed()
     */
    virtual bool_t isConstructed() const;
    
    /**
     * @copydoc eoos::api::Runnable::start()
     */
    virtual void start();    

protected:

    using Parent::setConstructed;    

private:

    /**
     * @brief Constructs this object.
     *
     * @return true if object has been constructed successfully.
     */
    bool_t construct();

};

} // namespace drv
} // namespace eoos
#endif // DRV_CANRESOURCEROUTINETX_HPP_
