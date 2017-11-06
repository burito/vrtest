CFLAGS = -g -std=c11 -Isrc -Isrc/openvr

PLATFORM = 
LIBRARIES = 
SDIR = src

OBJS = $(PLATFORM) main.o text.o mesh.o image.o stb_image.o 3dmaths.o shader.o

# Build rules
WDIR = build/win
_WOBJS = $(OBJS) GL/glew.o win32.o win32.res
WOBJS = $(patsubst %,$(WDIR)/%,$(_WOBJS))
WLIBS = $(LIBRARIES) lib/win/openvr_api.dll -lshell32 -luser32 -lgdi32 -lopengl32 -lwinmm -lws2_32 -lxinput9_1_0
#	-L"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v7.0\lib\x64"
#	-L"C:\Program Files (x86)\AMD APP SDK\3.0-0-Beta\lib\x86_64"

LDIR = build/lin
LCC = clang
_LOBJS = $(OBJS) GL/glew.o x11.o 
LOBJS = $(patsubst %,$(LDIR)/%,$(_LOBJS))
LLIBS = $(LIBRARIES) ./lib/linux/libopenvr_api.so -lm -lGL -lX11 -lGLU -lXi -ldl

MDIR = build/mac
MCC = clang
_MOBJS = $(OBJS)
MFLAGS = -Wall
MOBJS = $(patsubst %,$(MDIR)/%,$(_MOBJS))
MLIBS = -F/System/Library/Frameworks -F. -framework OpenGL -framework CoreVideo -framework Cocoa -framework IOKit ./lib/mac/libopenvr_api.dylib


# Evil platform detection magic
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
default: gui
WCC = x86_64-w64-mingw32-gcc
WINDRES = x86_64-w64-mingw32-windres
#WCC = i686-w64-mingw32-gcc
#WINDRES = i686-w64-mingw32-windres
else
ifeq ($(UNAME), Darwin)
default: gui.app
else
WCC = gcc
WINDRES = windres
default: gui.exe
endif 
endif


$(WDIR)/Icon.ico: $(SDIR)/Icon.png
	magick convert -resize 256x256 $^ $@
$(WDIR)/win32.res: $(SDIR)/win32.rc $(WDIR)/Icon.ico
	$(WINDRES) -I $(WDIR) -O coff src/win32.rc -o $@
$(WDIR)/%.o: $(SDIR)/%.c
	$(WCC) $(CFLAGS) -DWIN32 $(INCLUDES)-c $< -o $@
openvr_api.dll:
	cp lib/win/openvr_api.dll .
gui.exe: openvr_api.dll $(WOBJS)
	$(WCC) $(WOBJS)$(WLIBS) -o $@

# crazy stuff to get icons on x11
$(LDIR)/x11icon: $(SDIR)/x11icon.c
	$(LCC) $^ -o $@
$(LDIR)/icon.rgba: $(SDIR)/Icon.png
	convert -resize 256x256 $^ $@
#	magick convert -resize 256x256 $^ $@
$(LDIR)/icon.argb: $(LDIR)/icon.rgba $(LDIR)/x11icon
	./build/lin/x11icon < $(LDIR)/icon.rgba > $@
$(LDIR)/icon.h: $(LDIR)/icon.argb
	bin2h 13 < $^ > $@
$(LDIR)/x11.o: $(SDIR)/x11.c $(LDIR)/icon.h
	$(LCC) $(CFLAGS) $(INCLUDES) -I$(LDIR) -c $< -o $@
$(LDIR)/%.o: $(SDIR)/%.c
	$(LCC) $(CFLAGS) $(INCLUDES) -c $< -o $@
gui: $(LOBJS)
	$(LCC) $^ $(LLIBS) -o $@


# generate the Apple Icon file from src/Icon.png
$(MDIR)/AppIcon.iconset/icon_512x512@2x.png: src/Icon.png
	cp $^ $@
$(MDIR)/AppIcon.iconset/icon_512x512.png: src/Icon.png
	cp $^ $@
	sips -Z 512 $@
$(MDIR)/AppIcon.icns: $(MDIR)/AppIcon.iconset/icon_512x512@2x.png $(MDIR)/AppIcon.iconset/icon_512x512.png
	iconutil -c icns $(MDIR)/AppIcon.iconset
# build the Apple binary
$(MDIR)/osx.o: $(SDIR)/osx.m
	$(MCC) $(MFLAGS) -c $< -o $@
$(MDIR)/%.o: $(SDIR)/%.c
	$(MCC) $(CFLAGS) $(INCLUDES)-c $< -o $@
gui.bin: $(MOBJS) $(MDIR)/osx.o
	$(MCC) $^ $(MLIBS) -rpath @loader_path/ -o $@
#	$(MCC) $^ $(MLIBS) -rpath @loader_path/../Frameworks -o $@
# generate the Apple .app file
gui.app/Contents/_CodeSignature/CodeResources: gui.bin src/Info.plist $(MDIR)/AppIcon.icns
	rm -rf gui.app
	mkdir -p gui.app/Contents/MacOS
	mkdir gui.app/Contents/Resources
	cp lib/mac/libopenvr_api.dylib gui.app/Contents/MacOS
#	mkdir gui.app/Contents/Frameworks
#	cp -av OpenVR.framework gui.app/Contents/Frameworks
	cp gui.bin gui.app/Contents/MacOS/gui
	cp src/Info.plist gui.app/Contents
	cp $(MDIR)/AppIcon.icns gui.app/Contents/Resources
#	codesign --force --sign - gui.app/Contents/Frameworks/OpenVR.framework
	codesign --force --deep --sign - gui.app
gui.app: gui.app/Contents/_CodeSignature/CodeResources

# build a zip of the windows exe
voxel.zip: gui.exe
	zip opengl.zip gui.exe README.md LICENSE

# Housekeeping
clean:
	@rm -rf build gui gui.exe gui.bin gui.app opengl.zip src/version.h openvr_api.dll

# for QtCreator
all: default

# Create build directories
$(shell	mkdir -p build/lin/GL build/win/GL build/mac/AppIcon.iconset)

# create the version info
$(shell echo "#define GIT_REV \"`git rev-parse --short HEAD`\"" > src/version.h)
$(shell echo "#define GIT_TAG \"`git name-rev --tags --name-only \`git rev-parse HEAD\``\"" >> src/version.h)

