/**
 * @file      drv.CanResource.hpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2023, Sergey Baigudin, Baigudin Software
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
     * @brief Global data for all these objects;
     */
    Data& data_;
        
    /**
     * @brief Configuration of the resource.
     */
    Config config_;    
    
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
        res = true;
    } while(false);
    return res;
}

template <class A>
void CanResource<A>::deinitialize()
{
    lib::Guard<A> const guard(data_.mutex);
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
