# VoxBoss: The Voice Abstraction Library
Hello there, and welcome to VoxBoss, your ultimate TTS manager, putting you back in control.

VoxBoss is a budding voice abstraction library, designed to bridge the gap between various speech engines, providing a simple and unified interface to control them. It is currently lightweight and extendable, and eventually aims to be cross-platform.

Using its extendable handler system, developers can easily integrate text-to-speech functionality using their preferred speech engine, ensuring a seamless experience for the end user.

## Why VoxBoss?
While there are numerous speech engines out there, each with its own set of features and peculiarities, integrating them into projects often requires dealing with different APIs and intricacies. VoxBoss aims to simplify that, offering a consistent interface to control any supported speech engine.

## Current State
VoxBoss is in its early stages. It currently has a handler for SAPI, and a proof-of-concept test that shows it in action. The goal is to make it cross-platform and extend its support to more engines, but we're not there yet.

### A Journey, Not a Destination
Open-source projects thrive on collaboration and community contributions. Rather than waiting for a perfect "1.0" release, I've chosen to share VoxBoss with you all at this early stage, because software development is a journey, and sharing that journey can be as rewarding as reaching the destination.

Whether you're a potential user, a curious developer, or someone with expertise in voice engines, your feedback, suggestions, and contributions can shape the project's future.

## How Can You Help?
Please see the [contributing guide](contributing.md) if you're interested in helping out.

## Building
There are two source files to work with in the lib folder, vb.c and vb.h. Simply put these where your compiler can find them, and you're good to go.

To build the example, point your compiler at lib/vb.c and example/speak.c, and add lib as an include directory.

This system uses runtime linking, so you shouldn't have to link with any libraries at the compilation stage.

The library and its example have been tested with MinGW-W64 v12.2. It should theoretically work with other compilers as well.

If you have written any handlers for use with the system, you will need to include those in your compiler command.

Note at the moment there are no scripts to automate this, but it should be easy enough to make your own for your current setup.

If you have any problems, please do ask.

## Usage
Detailed usage instructions will be provided as the library matures. Meanwhile, please see the speak example for basic usage.

You may also find useful comments in vb.h.

## Licence
VoxBoss is released under the [zlib licence](licence.txt).
