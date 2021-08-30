/**
 * This file will handle the Startup logic for the APM system
 */

#ifndef _APM_STARTUP_HPP_
#define _APM_STARTUP_HPP_

namespace APM::Startup
{
    /**
     * Function to handle waiting for the handlebar ON signal to start up the APM system
     */
    // TODO: Handle this using a GPIO interrupt
    bool wait_for_startup();
}

#endif //_APM_STARTUP_HPP_
