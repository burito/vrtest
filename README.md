C-API OpenVR test
=================

Build Environment
-----------------
A full description can be found here...
http://danpburke.blogspot.com.au/2017/06/fresh-system-install.html
It's basically Mingw64 on windows, Clang everywhere else. Windows and Linux use ImageMagick to process the icon as part of the build process.

    make -j8   # build it using 8 threads   
    make clean # deletes all output files

There are no external dependancies. It will "just work". Visual Studio Code is the editor I'm using, and relevant config files are included.

Execution
---------
On Windows...

    gui.exe

On Linux, use...

    ~/.steam/steam/ubuntu12_32/steam-runtime/run.sh ./gui

On Mac, use one of...

    open ./gui.app
    ./gui.app/Contents/MacOS/gui

It's not correctly packaged, ie it needs to be run from this directory so that it can find the shaders and the stanford bunny.

Current Status
--------------
It works "perfectly" on Windows, it works "technically" on Linux (frame rate is questionable), and it works "hypothetically" on Mac.

Credits
-------

* src/stb_image.h - Sean Barret http://nothings.org/
* src/fast_atof.c - Tom Van Baak  http://www.leapsecond.com/
* src/invert4x4_sse.h - https://github.com/niswegmann/small-matrix-inverse
* src/openvr/* - Valve https://github.com/ValveSoftware/openvr
* src/glew/* - http://glew.sourceforge.net/
* data/stanford-bunny.obj - http://graphics.stanford.edu/data/3Dscanrep/

For everything else, I am to blame.

Except for the Stanford Bunny, everything is some flavour of free for commercial use. Check each file for details.