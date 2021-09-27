===
API
===

This documentation outlines the library for the APM.

APM
===
The code in the top level APM directory represents the universal board code
containing the base functionality for the APM board.  It involves methods
such as state control.

APMManager
----------
.. doxygenclass:: APM::APMManager
   :members:

APMUart
-------
.. doxygenclass:: APM::APMUart
   :members:

DEV
===
Devices, representation of hardware that can be interfaced with. In
general, devices are communicated with via some sort of IO interface, but that
is not strictly a rule. An LED is a simplistic example of a device.

SIM100
------
.. doxygenclass:: APM::DEV::SIM100
   :members:
