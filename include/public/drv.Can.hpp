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
     * @brief CAN standard of sample point in percentage.
     */    
    enum SamplePoint
    {
        SAMPLEPOINT_CANOPEN = 0, ///< 87.5% is the preferred value for CANopen and DeviceNet
        SAMPLEPOINT_ARINC825     ///< 75.0% is the default value for ARINC 825
    };

    /**
     * @struct Reg
     * @brief CAN controller registers.
     */
    struct Reg
    {
        /**
         * @struct Mcr
         * @brief CAN master control register.
         */
        struct Mcr
        {
            uint32_t       : 2;
            uint32_t txfp  : 1;     ///< Transmit FIFO priority 							(reset value is 0)
            uint32_t rflm  : 1;     ///< Receive FIFO locked mode 							(reset value is 0)
            uint32_t nart  : 1;     ///< No automatic retransmission 						(reset value is 0)
            uint32_t awum  : 1;     ///< Automatic wake-up mode 							(reset value is 0)
            uint32_t abom  : 1;     ///< Automatic bus-off management 						(reset value is 0)
            uint32_t ttcm  : 1;     ///< Time triggered communication mode 					(reset value is 0)
            uint32_t       : 8;
            uint32_t dbf   : 1;     ///< CAN reception and transmission frozen during debug	(reset value is 1)
            uint32_t       : 15;

        };

        /**
         * @struct Btr
         * @brief CAN bit timing register.
         */
        struct Btr
        {
            uint32_t      : 30;
            uint32_t lbkm : 1;      ///< Loop back mode for debug 							(reset value is 0)
            uint32_t silm : 1;      ///< Silent mode for debug 								(reset value is 0)
        };
        
        Mcr mcr;
        Btr btr;
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
     * @struct RxMessage
     * @brief CAN RX message structure definition.
     */
    struct RxMessage
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
     * @struct RxFilter
     * @brief CAN RX message filter initialization structure.
     */
    struct RxFilter
    {        
        /**
         * @enum Fifo
         * @brief Specifies the FIFO (0 or 1) which will be assigned to the filter.
         */
        enum Fifo
        {
            FIFO_0 = 0,            
            FIFO_1 = 1
        };
        
        /**
         * @enum Mode
         * @brief Specifies the filter mode.
         */
        enum Mode
        {
            MODE_IDMASK = 0, ///< Identifier Mask mode
            MODE_IDLIST = 1  ///< Identifier List mode
        };

        /**
         * @enum Scale
         * @brief Specifies the filter scale.
         */
        enum Scale
        {
            Scale_16BIT = 0, ///< Two 16-bit filters
            Scale_32BIT = 1  ///< One 32-bit filter
        };
        
        /**
         * @union Filter32
         * @brief 32-Bit Filter mapping.
         */
        union Filter32
        {
            typedef uint32_t Value;
            Filter32(){}
            Filter32(Value v){value = v;}
           ~Filter32(){}    
          
            Value value;
            struct Bit
            {
                Value      : 1;
                Value rtr  : 1;
                Value ide  : 1;
                Value exid : 18;
                Value stid : 11;
            } bit;
        };

        /**
         * @struct FilterIdMask32
         * @brief One 32-bit Filter - Identifier Mask mode.
         */
        struct FilterIdMask32
        {
            Filter32 id;
            Filter32 mask;
        };
        
        /**
         * @struct FilterIdList32
         * @brief Two 32-bit Filters - Identifier List mode.
         */
        struct FilterIdList32
        {
            Filter32 id[2];
        };
        
        /**
         * @union FilterGroup32
         * @brief Union for 32-bit of one Identifier Mask Filter or two Identifier List Filters.
         */        
        union FilterGroup32
        {
            FilterIdMask32 idMask;
            FilterIdList32 idList;
        };
        
        /**
         * @struct Filter16
         * @brief 16-Bit Filter mapping.
         */
        union Filter16
        {
            typedef uint16_t Value;
            Filter16(){}
            Filter16(Value v){value = v;}
           ~Filter16(){}    
          
            Value value;
            struct Bit
            {
                Value exid1715 : 3;
                Value rtr      : 1;
                Value ide      : 1;
                Value stid     : 11;
            } bit;
        };        

        /**
         * @struct FilterIdMask16
         * @brief 16-bit Filter - Identifier Mask mode.
         */
        struct FilterIdMask16
        {
            Filter16 id0;
            Filter16 mask0;
            Filter16 id1;
            Filter16 mask1;
        };

        /**
         * @struct FilterIdList16
         * @brief 16-bit Filters - Identifier List mode.
         */        
        struct FilterIdList16
        {
            Filter16 id[4];
        };

        /**
         * @union FilterGroup16
         * @brief Union for 16-bit of two Identifier Mask Filter or four Identifier List Filters.
         */        
        union FilterGroup16
        {
            FilterIdMask16 idMask;
            FilterIdList16 idList;
        };        

        /**
         * @union Filters
         * @brief Filters of all groups.
         */
        union Filters
        {
            FilterGroup32 group32;
            FilterGroup16 group16;
        };

        static const uint32_t NUMBER_OF_FILTER_GROUPS = 14; ///< Number of groups of filters.
        
        Filters     filters; ///< Specifies the filters of all groups depending on Mode and Scale.
        Fifo        fifo;    ///< Specifies the FIFO (0 or 1) which will be assigned to the filter.
        uint32_t    index;   ///< Specifies the filter index in ranges from 0 to 13.
        Mode        mode;    ///< Specifies the filter mode.
        Scale       scale;   ///< Specifies the filter scale.
    };

    /** 
     * @brief Destructor.
     */                               
    virtual ~Can() = 0;

    /**
     * @brief Initiates the transmission of a message.
     *
     * The function initiates a transmission of a passed message
     * by setting an internal CAN controller task to transmit.
     * If all the task are busy, the function waits till a task
     * becomes to be free and sets it. 
     *
     * @param message A message to tramsmit.
     * @return True if a transmition is initialied.
     */
    virtual bool_t transmit(TxMessage const& message) = 0;

    /**
     * @brief Receives a message.
     *
     * The function receives a message to the passed message structure.
     * If no messages in receiving buffers, the function waits till
     * a message comes. 
     *
     * @param message A message structure to receive to it.
     * @return True if a message is received successfully.
     */
    virtual bool_t receive(RxMessage* message) = 0;

    /**
     * @brief Sets filter for receiving messages.
     *
     * @param filter A filter to receive messages.
     * @return True if the filter is set successfully.
     */
    virtual bool_t setReceiveFilter(RxFilter const& filter) = 0;

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
