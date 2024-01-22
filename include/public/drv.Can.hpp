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
        NUMBER_CAN1 = 0
    };
    
    /**
     * @enum BitRate
     * @brief CAN bus speed in Kbit/s.
     */
    enum BitRate
    {
        BITRATE_1000 = 0,
        BITRATE_800,
        BITRATE_500,
        BITRATE_250,
        BITRATE_125,
        BITRATE_100,
        BITRATE_50,
        BITRATE_20,
        BITRATE_10
    };

    /**
     * @enum SamplePoint
     * @brief CANNN standard of sample point in percentage.
     */    
    enum SamplePoint
    {
        SAMPLEPOINT_CANOPEN = 0, // 87.5% is the preferred value for CANopen and DeviceNet
        SAMPLEPOINT_ARINC825     // 75.0% is the default value for ARINC 825
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
            uint32_t      : 30;
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
        Number      number;
        BitRate     bitRate;
        SamplePoint samplePoint;
        Reg         reg;
    };
    
    /**
     * @struct TxMessage
     * @brief CAN TX message structure definition.
     */
    struct TxMessage
    {
        uint32_t    id;         ///< An identifier of 11 bits or 29 bits
        bool_t      ide;        ///< Identifier extension bit that is false for base frame and true for extended frame
        bool_t      rtr;        ///< Remote transmission request that is true for remote request frames
        uint32_t    dlc;        ///< Data length code that is number of bytes of data (0–8 bytes)
        union
        {
            uint64_t v64[1];
            uint32_t v32[2];
            uint16_t v16[4];            
            uint8_t  v8[8];    
        } data;                 ///< Data to be transmitted
    };
    
    /**
     * @class TxHandler
     * @brief CAN TX handler.
     */    
    class TxHandler
    {
        
    public:
        
        /**
         * @brief Tests if the message is transmited.
         *
         * @return Transmit resource.
         */
        virtual bool_t isTransmited() = 0;
        
    protected:
    
        /** 
         * @brief Destructor.
         */                               
        virtual ~TxHandler() = 0;        

    };
    
    /** 
     * @brief Destructor.
     */                               
    virtual ~Can() = 0;

    /**
     * @brief Initiates the transmission of a message.
     *
     * @param message A message to tramsmit.
     * @return Transmit handler or NULLPTR if an error occurred.
     */
    virtual TxHandler* transmit(TxMessage const& message) = 0;

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
