# APM

The code generated for this library can be found at [APM readthedocs](evt-apm.readthedocs.io)

## Overview

This project holds the code for the RIT Electric Vehicles Team's (EVT)
Auxiliary Power Module (APM).  To see our main code repository please see
[EVT-core](https://github.com/RIT-EVT/EVT-core).

The APM is designed to control the power distribution for the DEV1 electric
dirtbike designed by the EVT team.  It will control the bike start up
sequence, provide 12V and 5V power to the bike electronic systems (excluding
main motor), and detect faults in the power system.

Documentation on the APM Multidisciplanary Senior Design Project can be found
here: [APM Confluence](https://wiki.rit.edu/display/P21261/P21261+Project+Overview)

![APM System Architecture](./assets/DEV1%20APM%20Architecture.png)

## Build
The compilation process is handled via CMake and in general follows standard
practices. The noticable addition is the need to setup an environment variable
which stores the location to the gcc arm toolchain. Below is an example of
how to set the environment variable on Linux/Unix.

```bash
export GCC_ARM_TOOLS_PATH=/path/to/gcc_arm_tools_path/bin/
```

The compilation logic will make use of that enivronment variable to locate
the needed tools to generate the code.

After that, the compilation process follows standard practice. Below is
an example compilation process.

```bash
mkdir build/
cd build/
cmake ../
make -j
```

After that, in the `build` folder will be the compiled library code as well
as generated samples.

For building with linting enabled (recommended). First install cpplint, then
run the installation with the modification below.

```bash
mkdir build/
cd build/
cmake -DEVT_LINT=ON ../
make -j
```