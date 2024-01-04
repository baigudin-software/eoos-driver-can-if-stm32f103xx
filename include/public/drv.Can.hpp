/**
 * @file      drv.Can.hpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2023-2024, Sergey Baigudin, Baigudin Software
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
     * @brief CAN controller number.
     */
    enum Number
    {
        NUMBER_CAN1 = 0,
        NUMBER_CAN2 = 1        
    };
    
    /**
     * @struct Reg
     * @brief CAN controller registers.
     */
    struct Reg
    {
        // CAN master control register
        struct Mcr
        {
            uint32_t       : 2;
            uint32_t txfp  : 1;     ///< Transmit FIFO priority (reset value is 0)
            uint32_t rflm  : 1;     ///< Receive FIFO locked mode (reset value is 0)
            uint32_t nart  : 1;     ///< No automatic retransmission (reset value is 0)
            uint32_t awum  : 1;     ///< Automatic wake-up mode (reset value is 0)
            uint32_t abom  : 1;     ///< Automatic bus-off management (reset value is 0)
            uint32_t ttcm  : 1;     ///< Time triggered communication mode (reset value is 0)
            uint32_t       : 24;
        } mcr;
        // CAN bit timing register
        struct Btr
        {
            uint32_t brp  : 10;     ///< Baud rate prescaler (reset value is 0)
            uint32_t      : 6;
            uint32_t ts1  : 4;      ///< Time segment 1 (reset value is 3)
            uint32_t ts2  : 3;      ///< Time segment 2 (reset value is 2)
            uint32_t      : 1;
            uint32_t sjw  : 2;      ///< Resynchronization jump width (reset value is 0)
            uint32_t      : 4;
            uint32_t lbkm : 1;      ///< Loop back mode for debug (reset value is 0)
            uint32_t silm : 1;      ///< Silent mode for debug (reset value is 0)
        } btr;
    };

    /**
     * @struct Config
     * @brief Configure CAN driver resource.
     */    
    struct Config
    {
        Number  number;
        Reg     reg;
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
