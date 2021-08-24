/**
 * Implementation of the Startup functionality
 */

#include <APM/startup.hpp>
#include <EVT/utils/time.hpp>

bool APM::Startup::wait_for_startup() {
    // Wait and do nothing
    EVT::core::time::wait(1000);
    return false;
}
