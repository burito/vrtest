define GET_HELP

Please read the instructions at...
	http://danpburke.blogspot.com.au/2017/06/fresh-system-install.html
...for help
endef


# Build rules

LCC = clang
MCC = clang
CC = clang -g
MFLAGS = -Wall


WIN_LIBS = openvr_api.dll -lshell32 -luser32 -lgdi32 -lopengl32 -lwinmm -lws2_32 -lxinput9_1_0
LIN_LIBS = deps/openvr/bin/linux64/libopenvr_api.so -lm -lGL -lX11 -lGLU -lXi -ldl
MAC_LIBS = deps/openvr/bin/osx32/libopenvr_api.dylib -framework OpenGL -framework CoreVideo -framework Cocoa -framework IOKit -rpath .

WIN_DIR = build/win
LIN_DIR = build/lin
MAC_DIR = build/mac

_WIN_OBJS = glew.o win32.o win32.res $(OBJS)
_LIN_OBJS = glew.o x11.o $(OBJS)
_MAC_OBJS = osx.o $(OBJS)

WIN_OBJS = $(patsubst %,$(WIN_DIR)/%,$(_WIN_OBJS))
LIN_OBJS = $(patsubst %,$(LIN_DIR)/%,$(_LIN_OBJS))
MAC_OBJS = $(patsubst %,$(MAC_DIR)/%,$(_MAC_OBJS))


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


OBJS = main.o version.o text.o fast_atof.o mesh.o image.o log.o global.o stb_image.o 3dmaths.o shader.o glerror.o vr.o
CFLAGS = -std=c11 -Ideps/include -Ideps/dpb/src
VPATH = src build deps deps/dpb/src


$(WIN_DIR)/Icon.ico: Icon.png
	$(MAGICK) -resize 256x256 $^ $@
$(WIN_DIR)/win32.res: win32.rc $(WIN_DIR)/Icon.ico
	$(WINDRES) -I $(WIN_DIR) -O coff src/win32.rc -o $@
$(WIN_DIR)/%.o: %.c
	$(WCC) $(DEBUG) $(CFLAGS) $(INCLUDES)-c $< -o $@
openvr_api.dll:
	cp deps/win/openvr_api.dll .

gui.exe: $(WIN_OBJS)
	$(WCC) $^ $(WIN_LIBS) -o $@

gui: $(LIN_OBJS)
	$(LCC) $(CFLAGS) $^ $(LIN_LIBS) -o $@

gui.bin: $(MAC_OBJS) libopenvr_api.dylib
	$(CC) $(CFLAGS) $^ $(MAC_LIBS) -o $@


# crazy stuff to get icons on x11
$(LIN_DIR)/x11icon: x11icon.c
	$(LCC) $^ -o $@
$(LIN_DIR)/icon.rgba: Icon.png
	$(MAGICK) -resize 256x256 $^ $@
#	magick convert -resize 256x256 $^ $@
$(LIN_DIR)/icon.argb: $(LIN_DIR)/icon.rgba $(LIN_DIR)/x11icon
	./build/lin/x11icon < $(LIN_DIR)/icon.rgba > $@
$(LIN_DIR)/icon.h: $(LIN_DIR)/icon.argb
	bin2h 13 < $^ > $@
$(LIN_DIR)/x11.o: x11.c $(LIN_DIR)/icon.h
	$(LCC) $(CFLAGS) $(INCLUDES) -I$(LIN_DIR) -c $< -o $@
$(LIN_DIR)/%.o: %.c
	$(LCC) $(DEBUG) $(CFLAGS) $(INCLUDES) -c $< -o $@



$(MAC_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(MAC_DIR)/%.o: %.m
	$(CC) $(CFLAGS) -c $< -o $@

libopenvr_api.dylib: deps/openvr/bin/osx32/libopenvr_api.dylib
	cp $< $@




# start build the App Bundle (apple)
MAC_BUNDLE = gui


# generate the Apple Icon file from src/Icon.png
$(MAC_DIR)/AppIcon.iconset:
	mkdir $@
$(MAC_DIR)/AppIcon.iconset/icon_512x512@2x.png: Icon.png $(MAC_DIR)/AppIcon.iconset
	cp $< $@
$(MAC_DIR)/AppIcon.iconset/icon_512x512.png: Icon.png $(MAC_DIR)/AppIcon.iconset
	sips -Z 512 $< --out $@ 1>/dev/null
$(MAC_DIR)/AppIcon.icns: $(MAC_DIR)/AppIcon.iconset/icon_512x512@2x.png $(MAC_DIR)/AppIcon.iconset/icon_512x512.png
	iconutil -c icns $(MAC_DIR)/AppIcon.iconset

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

$(MAC_CONTENTS)/Resources/AppIcon.icns: $(MAC_DIR)/AppIcon.icns
	@mkdir -p $(MAC_CONTENTS)/Resources
	cp $< $@


$(MAC_CONTENTS)/Frameworks/libopenvr_api.dylib: libopenvr_api.dylib
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
