/**
 * @file      drv.Can.cpp
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2023, Sergey Baigudin, Baigudin Software
 */
#include "drv.Can.hpp"
#include "drv.CanController.hpp"
#include "lib.UniquePointer.hpp"

namespace eoos
{
namespace drv
{
    
static CanController* getController()
{
    static CanController* controller( NULLPTR );    
    if( controller == NULLPTR )
    {
        controller = new CanController;
        if( controller != NULLPTR )
        {
            if( !controller->isConstructed() )
            {
                delete controller;
                controller = NULLPTR;                
            }
        }
    }
    return controller;
}
    
Can* Can::create(Config const& config)
{
    lib::UniquePointer<Can> res;
    CanController* const controller( getController() );
    if( controller != NULLPTR )
    {    
        res.reset( controller->createResource(config) );
        if( !res.isNull() )
        {
            if( !res->isConstructed() )
            {
                res.reset();
            }
        }
    }
    return res.release();
}

Can::~Can(){}

} // namespace drv
} // namespace eoos
