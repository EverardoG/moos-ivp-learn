# moos-ivp-learn

# Introduction

The moos-ivp-learn repository was created for running multiagent learning on the herons. This repository includes a few core components necessary for learning. These include the following.

* `pSectorSense` - Simulated sensor for vehicle to sense swimmers and other vehicles
* `uFldRecordKeeper` - Keeps records on vehicle and swimmer position histories
* `lib_BHV_neural_network` - Behavior for running neural network to generate cost functions

Each component is described in more detail below. After that is the boilerplate for this README from the `moos-ivp-extend` repo.

## pSectorSense

This app takes as input a *swimmer_positions.txt* file that includes the positions of all swimmers at the start of the mission. SectorSense provides a sector-based sensor that divides the world into equal-sized sectors relative to the position and heading of the vehicle using the sensor. Each sector has 2 readings associated with it. One is the density of swimmers in that sector, and the other is the density of vehicles in that sector.

The sectors rotate with the vehicle, meaning that if the vehicle's heading changes, so can the sensor readings. This sensor has a sensing radius, so that any swimmers or vehicles outside of this radius (too far away from the vehicle) will not be included in the density measurement. Density is roughly the sum of inverse distances to all entities within a sector.

As an example, let's say we are using 4 sectors. That means we are breaking up the world into quadrants for a vehicle. That also means we have 8 sensor readings. 4 readings are of other vehicles and 4 are of the swimmers. The sensor readings put together give a rough idea of where other vehicles and swimmers are distributed.

This app will also work in tandem with a field manager app that tracks swimmers so that new swimmers can be added and the existing swimmers can be removed. Note that each vehicle's pSectorSense app will need to be updated individually when a swimmer is added or removed. Otherwise, the sensors will be out of sync. The idea for removing swimmers is that swimmers will be removed, or marked as "rescued", when a vehicle gets close enough to that swimmer. Once a swimmer is rescued, then it will no longer show up in the sensor readings.

## uFldRecordKeeper

This app tracks the positions of all swimmers and vehicles. This also tracks whether each swimmer has been "rescued" or not. This is a simple app meant for record-keeping. This is useful for figuring out how many swimmers were rescued so we can compute a score at the end of a mission.

## lib_BHV_neural_network

This app reads in neural network structure and parameters from a file, and loads those parameters into a neural network. This network takes as input the sensor readings for sectors, and outputs cost functions for heading and velocity to be resolved in IvP.

# Directory Structure

The directory structure for the moos-ivp-extend is described below:

| Directory        | Description                                 |
|:---------------- |:------------------------------------------- |
| bin              | Directory for generated executable files    |
| build            | Directory for build object files            |
| build.sh         | Script for building moos-ivp-extend         |
| CMakeLists.txt   | CMake configuration file for the project    |
| data             | Directory for storing data                  |
| lib              | Directory for generated library files       |
| missions         | Directory for mission files                 |
| README           | Contains helpful information - (this file). |
| scripts          | Directory for script files                  |
| src              | Directory for source code                   |


# Build Instructions

## Linux and Mac Users

To build on Linux and Apple platforms, execute the build script within this
directory:

```bash
   $ ./build.sh
```

To build without using the supplied script, execute the following commands
within this directory:

```bash
   $ mkdir -p build
   $ cd build
   $ cmake ../
   $ make
   $ cd ..
```


## Windows Users

To build on Windows platform, open CMake using your favorite shortcut. Then
set the source directory to be this directory and set the build directory
to the "build" directory inside this directory.

The source directory is typically next to the question:
   "Where is the source code?"

The build directory is typically next to the question:
   "Where to build the binaries?"

Alternatively, CMake can be invoked via the command line. However, you must
specify your generator. Use "cmake --help" for a list of generators and
additional help.


# Environment variables

The moos-ivp-extend binaries files should be added to your path to allow them
to be launched from pAntler.

In order for generated IvP Behaviors to be recognized by the IvP Helm, you
should add the library directory to the "IVP_BEHAVIOR_DIRS" environment
variable.

# END of README

