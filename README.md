# VoxBoss: The Voice Abstraction Library
Hello there, and welcome to VoxBoss: The ultimate TTS manager that puts you back in control.

VoxBoss is a budding voice abstraction library that is lightweight, extendable, and cross-platform. Its main purposes are:

* To allow developers to integrate any speech engine they wish with relative ease using a handler-registry system,

* to provide developers with a single interface to all supported engines,

* And to provide as smooth an experience as possible for the user in the event of failure.

Please note: The default branch is "dev". Any code you download from there may be unstable. For more stable code, please switch to the "main" branch.

## Current State
VoxBoss is in its early prerelease stages:

* It has handlers for SAPI and Mac's TTS features, and a proof-of-concept test that shows it in action.
* Its API is very minimal, only providing required functionality for the time being (speak, stop, and is speaking).
* It has no Linux handlers yet.

Things that are planned include:

* API expansion: Ability to change parameters (voices, languages, pitch, rate, volume etc), more configuration (library linkage, default parameters etc),
* Support for Linux and other platforms,
* More handlers (builtin and pluggable),
* Possibly screen reader support (speech and braille) subject to licensing checks,
* Possibly shared libraries for use with other languages,
* And who knows what else.

### A Journey, Not a Destination
Open-source projects thrive on collaboration and community contributions. Rather than waiting for a perfect "1.0" release, I've chosen to share this with you all at this early stage, because software development is a journey, and sharing that journey can be as rewarding as reaching the destination.

Whether you're a potential user, a curious developer, or someone with expertise in voice engines, your feedback, suggestions, and contributions can shape the project's future.

## How Can You Help?
Please see the [contributing guide](contributing.md) if you're interested in helping out.

## Building
There are two source files to work with in the lib folder, vb.c and vb.h. Simply put these where your compiler can find them, and you're good to go.

To build the example, point your compiler at lib/vb.c and example/speak.c, and add lib as an include directory.

```shell
gcc -o speak src/lib/vb.c src/example/speak.c -I src/lib
```

This system uses runtime linking, so you shouldn't have to link with any libraries at the compilation stage. Note that this comes at the cost of speed (this may become configurable at a later date).

It compiles successfully on Windows, Linux and Mac (though no speech handlers have been implemented for Linux yet). The example has been tested successfully on Windows and Mac.

If you have written any handlers for use with the system, you will need to include those in your compiler command.

```shell
gcc -o speak src/lib/vb.c src/lib/myhandler.c src/example/speak.c -I src/lib
```

If you have any problems, please do ask.

## Usage
Detailed usage instructions will be provided as the library matures. Meanwhile, please see the speak example for basic usage.

You may also find useful comments in the header file (vb.h).

## Licence
This project is released under the [zlib licence](licence.txt).
