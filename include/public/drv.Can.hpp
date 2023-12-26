/**
 * @file      drv.Can.hpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2023, Sergey Baigudin, Baigudin Software
 */
#ifndef DRV_CAN_HPP_
#define DRV_CAN_HPP_

#include "api.Object.hpp"

namespace eoos
{
namespace drv
{

/**
 * @class Can
 * @brief Controller area network (CAN) device driver.
 */
class Can : public api::Object
{
public:

    /**
     * @enum Number
     * @brief NULL numbers.
     */
    enum Number
    {
        NUMBER_CAN1 = 0,
        NUMBER_CAN2 = 1        
    };

    /**
     * @struct Config
     * @brief Configure CAN driver resource.
     */    
    struct Config
    {
        Number number;
    };
    
    /** 
     * @brief Destructor.
     */                               
    virtual ~Can() = 0;

    /**
     * @brief Create the driver resource.
     *
     * @param config Configuration of the driver resource.
     * @return A new driver resource, or NULLPTR if an error has been occurred.
     */
    static Can* create(Config const& config);

};

} // namespace drv
} // namespace eoos
#endif // DRV_CAN_HPP_
