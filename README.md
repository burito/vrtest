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
On Windows, it works perfectly.
On Linux, it appears to be stuck at 60fps. The Steam Home app doesn't have this issue. There are also some tracking fails, which seems to be a feature of the Linux VR Experience at the moment. After it's been running for a few minutes they disappear completely, so my guess is linux power management is a bit trigger happy?
On Mac, it runs really horribly. I've tried using a very low poly model to mitigate the low powered hardware I have, and it has no effect. My test machine is a MacBookPro mid-2014, w/ a 750m. It claims to be compatible with HDMI 1.4, which is what the HTC Vive requires. On the monitor it runs flawlessly, on the Vive, there are some really bad, what I'm calling frame re-use issues. My best guess is the MacBookPro's HDMI 1.4 support is questionable. The Official OpenVR sample program has the same behaviour. I have a Mini DisplayPort to Mini DisplayPort cable on the way, hopefully this will solve it. There's also some odd behaviour with OpenVR initialisation calls yeilding execution, which I wasn't aware was possible, but I'm no Objective-C expert, so that could be normal. The work around is to just check the OpenVR pointers before calling them. Sooner or later they get populated as the init function finishes between frames, and it starts working.

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