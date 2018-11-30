define GET_HELP

Please read the instructions at...
	http://danpburke.blogspot.com.au/2017/06/fresh-system-install.html
...for help
endef

CFLAGS = -std=c11 -Ideps/include
VPATH = src deps build
OBJS = main.o version.o text.o fast_atof.o mesh.o image.o log.o stb_image.o 3dmaths.o shader.o glerror.o vr.o

DEBUG = -g
#DEBUG =

# Build rules
WDIR = build/win
_WOBJS = $(OBJS) glew.o win32.o win32.res
WOBJS = $(patsubst %,$(WDIR)/%,$(_WOBJS))
WINLIBS = -lshell32 -luser32 -lgdi32 -lopengl32 -lwinmm -lws2_32 -lxinput9_1_0
LOCAL_LIB = deps/win/openvr_api.dll
LOCAL_DLL = deps/win/openvr_api.dll


LDIR = build/lin
LCC = clang
_LOBJS = $(OBJS) glew.o x11.o 
LOBJS = $(patsubst %,$(LDIR)/%,$(_LOBJS))
LLIBS = ./deps/linux/libopenvr_api.so -lm -lGL -lX11 -lGLU -lXi -ldl

MDIR = build/mac
MCC = clang
_MOBJS = $(OBJS) osx.o
MFLAGS = -Wall
MOBJS = $(patsubst %,$(MDIR)/%,$(_MOBJS))
MLIBS = -F/System/Library/Frameworks -F. -framework OpenGL -framework CoreVideo -framework Cocoa -framework IOKit -Ldeps/mac -lopenvr_api -rpath .


# Evil platform detection magic
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)	# if Apple
default: gui.app
else
# Windows & Linux need ImageMagick, lets check for it
ifeq (magick,$(findstring magick, $(shell which magick 2>&1))) # current ImageMagick looks like this
MAGICK = magick convert
else
	ifeq (convert,$(findstring convert, $(shell which convert 2>&1))) # Ubuntu ships a very old ImageMagick that looks like this
MAGICK = convert
	else
$(error Can't find ImageMagick installation $(GET_HELP))	
	endif
endif # ImageMagick check done!

ifeq ($(UNAME), Linux)	# if Linux
default: gui
# this entry is for building windows binaries on linux with mingw64
WCC = x86_64-w64-mingw32-gcc
WINDRES = x86_64-w64-mingw32-windres
WLIBS = $(LOCAL_DLL) $(WINLIBS)
#WCC = i686-w64-mingw32-gcc
#WINDRES = i686-w64-mingw32-windres
else	# if Windows

ifdef WCC	# allow user to override compiler choice
$(info "$(WCC)")
	ifeq (clang,$(WCC))
$(info this was called)
WCC := clang -target x86_64-pc-windows-gnu
	else
	WCC = $(WCC)
	endif
	WLIBS = $(LOCAL_DLL) $(WINLIBS)
else
	ifdef VS120COMNTOOLS # we have MSVC 2017 installed
		WLIBS = $(LOCAL_LIB) $(WINLIBS)
		WCC = gcc
	else
		WLIBS = $(LOCAL_DLL) $(WINLIBS)
		ifeq (,$(findstring which, $(shell which clang 2>&1))) # clang present?
			WCC = clang -target x86_64-pc-windows-gnu
		else
			ifeq (,$(findstring which, $(shell which gcc 2>&1))) # gcc present?
				WCC = gcc
			else
$(error Can't find a compiler. $(GET_HELP))
			endif
		endif
	endif
endif
# end compiler detection
WINDRES = windres
default: gui.exe
endif
endif


$(WDIR)/Icon.ico: Icon.png
	$(MAGICK) -resize 256x256 $^ $@
$(WDIR)/win32.res: win32.rc $(WDIR)/Icon.ico
	$(WINDRES) -I $(WDIR) -O coff src/win32.rc -o $@
$(WDIR)/%.o: %.c
	$(WCC) $(DEBUG) $(CFLAGS) $(INCLUDES)-c $< -o $@
openvr_api.dll:
	cp deps/win/openvr_api.dll .
gui.exe: openvr_api.dll $(WOBJS)
	$(WCC) $(DEBUG) $(WOBJS) $(WLIBS) -o $@

# crazy stuff to get icons on x11
$(LDIR)/x11icon: x11icon.c
	$(LCC) $^ -o $@
$(LDIR)/icon.rgba: Icon.png
	$(MAGICK) -resize 256x256 $^ $@
#	magick convert -resize 256x256 $^ $@
$(LDIR)/icon.argb: $(LDIR)/icon.rgba $(LDIR)/x11icon
	./build/lin/x11icon < $(LDIR)/icon.rgba > $@
$(LDIR)/icon.h: $(LDIR)/icon.argb
	bin2h 13 < $^ > $@
$(LDIR)/x11.o: x11.c $(LDIR)/icon.h
	$(LCC) $(CFLAGS) $(INCLUDES) -I$(LDIR) -c $< -o $@
$(LDIR)/%.o: %.c
	$(LCC) $(DEBUG) $(CFLAGS) $(INCLUDES) -c $< -o $@
gui: $(LOBJS)
	$(LCC) $(DEBUG) $^ $(LLIBS) -o $@



$(MDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(MDIR)/%.o: %.m
	$(CC) $(CFLAGS) -c $< -o $@

libopenvr_api.dylib: deps/mac/libopenvr_api.dylib
	cp $< $@


gui.bin: $(MOBJS) libopenvr_api.dylib
	$(CC) $(CFLAGS) $(MLIBS) $^ -o $@


# start build the App Bundle (apple)
MAC_BUNDLE = gui


# generate the Apple Icon file from src/Icon.png
$(MDIR)/AppIcon.iconset:
	mkdir $@
$(MDIR)/AppIcon.iconset/icon_512x512@2x.png: Icon.png $(MDIR)/AppIcon.iconset
	cp $< $@
$(MDIR)/AppIcon.iconset/icon_512x512.png: Icon.png $(MDIR)/AppIcon.iconset
	sips -Z 512 $< --out $@ 1>/dev/null
$(MDIR)/AppIcon.icns: $(MDIR)/AppIcon.iconset/icon_512x512@2x.png $(MDIR)/AppIcon.iconset/icon_512x512.png
	iconutil -c icns $(MDIR)/AppIcon.iconset

MAC_CONTENTS = $(MAC_BUNDLE).app/Contents

.PHONY: $(MAC_BUNDLE).app
$(MAC_BUNDLE).app : $(MAC_CONTENTS)/_CodeSignature/CodeResources

# this has to list everything inside the app bundle
$(MAC_CONTENTS)/_CodeSignature/CodeResources : \
	$(MAC_CONTENTS)/MacOS/$(MAC_BUNDLE) \
	$(MAC_CONTENTS)/Resources/AppIcon.icns \
	$(MAC_CONTENTS)/Frameworks/libopenvr_api.dylib \
	$(MAC_CONTENTS)/Info.plist
	codesign --force --deep --sign - $(MAC_BUNDLE).app

$(MAC_CONTENTS)/Info.plist: src/Info.plist
	@mkdir -p $(MAC_CONTENTS)
	cp $< $@

$(MAC_CONTENTS)/Resources/AppIcon.icns: $(MDIR)/AppIcon.icns
	@mkdir -p $(MAC_CONTENTS)/Resources
	cp $< $@


$(MAC_CONTENTS)/Frameworks/libopenvr_api.dylib: deps/mac/libopenvr_api.dylib
	@mkdir -p $(MAC_CONTENTS)/Frameworks
	cp $< $@

# copies the binary, and tells it where to find libraries
$(MAC_CONTENTS)/MacOS/$(MAC_BUNDLE): $(MAC_BUNDLE).bin
	@mkdir -p $(MAC_CONTENTS)/MacOS
	cp $< $@
	install_name_tool -change @loader_path/libopenvr_api.dylib @loader_path/../Frameworks/libopenvr_api.dylib $@
	install_name_tool -add_rpath "@loader_path/../Frameworks" $@

.DELETE_ON_ERROR :
# end build the App Bundle

# build a zip of the windows exe
voxel.zip: gui.exe
	zip opengl.zip gui.exe README.md LICENSE

# Housekeeping
clean:
	@rm -rf build gui gui.exe gui.bin gui.app opengl.zip src/version.h openvr_api.dll libopenvr_api.so gui.pdb gui.ilk

help:
	echo Possible targets are...
	echo 	make 		# build the default target for the current platform
	echo 	make clean	# remove intermediate build files
	echo 	make help	# display this message

# for QtCreator
all: default

# Create build directories
$(shell	mkdir -p build/lin/GL build/win/GL build/mac/AppIcon.iconset)

# create the version info
GIT_VERSION:=$(shell git describe --dirty --always --tags)
VERSION:=const char git_version[] = "$(GIT_VERSION)";
SRC_VERSION:=$(shell cat build/version.c 2>/dev/null)
ifneq ($(SRC_VERSION),$(VERSION))
$(shell echo '$(VERSION)' > build/version.c)
endif
