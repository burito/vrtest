C-API OpenVR test
=================
A simple test of SteamVR, serving as a template for further work.

Quickstart
----------
    git clone --recurse-submodules git@github.com:burito/vrtest
    cd vrtest
    make -j8                    # build it using 8 threads   
    vrtest.exe                             # windows
    ./vrtest                               # linux
    ./vrtest.bin                           # osx
    ./vrtest.app/Contents/MacOS/vrtest        # osx with keyboard & menu

If you have Steam and SteamVR installed (SteamVR is listed in Steam's "Tools" menu), then press F9. If you don't have a VR headset that works with SteamVR, you can use the [null driver](https://developer.valvesoftware.com/wiki/SteamVR/steamvr.vrsettings).

Usage
-----
 * ESC - quit
 * F9 - toggle VR
 * F11 - toggle fullscreen

On Linux, to get the full Steam environment, one should use the command
    ~/.steam/steam/ubuntu12_32/steam-runtime/run.sh ./vulkan
This may not be necessary anymore.

Build Environment
-----------------
### Windows
* Install [mingw-w64-install.exe](http://sourceforge.net/projects/mingw-w64/files/) 8.1.0-x86_64-posix-seh
* Add its ```bin``` directory to your path
* Install current GPU drivers
	* Nvidia 417.35
* Install [ImageMagick](http://www.imagemagick.org/script/download.php#windows)

### Linux
* Install current GPU drivers and compiler
	* ```add-apt-repository ppa:graphics-drivers/ppa```
	* ```apt-get update```
	* ```apt-get install nvidia-410 vulkan-utils build-essential clang imagemagick```

### MacOS
* Install XCode

Libraries
---------
They are almost all in submodules now.
    git submodule init
    git submodule update --remote
GLEW doesn't distribute useable files from a git repo (the needed files are generated), so that has to be included in the project.

Submodules / Credits
====================
* [```deps/stb```](https://github.com/nothings/stb) - [Sean Barrett](http://nothings.org/)
* [```deps/fast_atof.c```](http://www.leapsecond.com/tools/fast_atof.c) - [Tom Van Baak](http://www.leapsecond.com/)
* [```deps/small-matrix-inverse```](https://github.com/niswegmann/small-matrix-inverse) - Nis Wegmann
* [```deps/openvr```](https://github.com/ValveSoftware/openvr) - Valve Software
* [```deps/models```](https://github.com/burito/models) - [Morgan McGuire's Computer Graphics Archive](https://casual-effects.com/data)
* ```deps/*gl*``` - [GLEW 2.1.0](http://glew.sourceforge.net/)
    * Add ```#define GLEW_STATIC``` to the top of ```glew.h```

For everything else, I am to blame.
