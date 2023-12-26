/**
 * @file      drv.CanDefinitions.hpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2023, Sergey Baigudin, Baigudin Software
 *
 * @brief Driver global definitions.
 */
#ifndef DRV_CANDEFINITIONS_HPP_
#define DRV_CANDEFINITIONS_HPP_

/**
 * @brief Define number of static allocated resources.
 * 
 * @note
 *  - If EOOS_GLOBAL_DRV_NUMBER_OF_<resource_name> does not equal zero and EOOS_GLOBAL_ENABLE_NO_HEAP is any,
 *    the resource will be allocated in pre-allocated pool memory.
 *  - If EOOS_GLOBAL_DRV_NUMBER_OF_<resource_name> equals zero and EOOS_GLOBAL_ENABLE_NO_HEAP is not defined, 
 *    the resource will be allocated in heap memory.
 *  - If EOOS_GLOBAL_DRV_NUMBER_OF_<resource_name> equals zero and EOOS_GLOBAL_ENABLE_NO_HEAP is defined, 
 *    the resource will NOT be allocated.
 *  - EOOS_GLOBAL_DRV_NUMBER_OF_<resource_name> less then zero is prohibbited.
 * 
 * @note 
 *  To comply MISRA-C++:2008 in Rule 18–4–1:
 *  - EOOS_GLOBAL_DRV_NUMBER_OF_<resource_name> must not equal zero
 *  - EOOS_GLOBAL_ENABLE_NO_HEAP must be defined.
 *
 * @note 
 * 	The EOOS_GLOBAL_DRV_NUMBER_OF_<resource_name> shall be passed to the project build system through compile definition.
 */
#ifndef EOOS_GLOBAL_DRV_NUMBER_OF_CANS
    /**
     * @brief Number of CAN driver resources that must be form 1 to infinity.
     */
    #define EOOS_GLOBAL_DRV_NUMBER_OF_CANS (1)
#endif

/**
 * @brief Do compile error check of static allocated resources.
 */
#ifndef EOOS_GLOBAL_ENABLE_NO_HEAP
    #error "The EOOS_GLOBAL_ENABLE_NO_HEAP must be defined for EOOS Driver layer to comply MISRA-C++:2008"
#endif

#endif // DRV_CANDEFINITIONS_HPP_
