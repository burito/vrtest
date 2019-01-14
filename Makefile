
# Build rules
BINARY_NAME = vrtest
OBJS = main.o version.o text.o fast_atof.o mesh.o image.o log.o global.o stb_image.o 3dmaths.o shader.o glerror.o vr.o
CFLAGS = -std=c11 -Ideps/include -Ideps/dpb/src
VPATH = src build deps deps/dpb/src

WIN_LIBS = -lshell32 -luser32 -lgdi32 -lopengl32 -lwinmm -lws2_32 -lxinput9_1_0
LIN_LIBS = -lm -lGL -lX11 -lGLU -lXi -ldl -rpath .
MAC_LIBS = deps/openvr/bin/osx32/libopenvr_api.dylib -framework OpenGL -framework CoreVideo -framework Cocoa -framework IOKit -rpath .

_WIN_OBJS = glew.o win32.o gfx_gl_win.o win32.res $(OBJS)
_LIN_OBJS = glew.o linux_xlib.o gfx_gl_lin.o $(OBJS)
_MAC_OBJS = osx.o gfx_gl_osx.o $(OBJS)


include deps/dpb/Makefile

$(BINARY_NAME).exe: $(WIN_OBJS) openvr_api.dll

$(BINARY_NAME): $(LIN_OBJS) libopenvr_api.so

$(BINARY_NAME).bin: $(MAC_OBJS) libopenvr_api.dylib


openvr_api.dll: deps/openvr/bin/win64/openvr_api.dll
	cp $< $@

libopenvr_api.dylib: deps/openvr/bin/osx32/libopenvr_api.dylib
	cp $< $@

libopenvr_api.so: deps/openvr/bin/linux64/libopenvr_api.so
	cp $< $@


# this has to list everything inside the app bundle
$(MAC_CONTENTS)/_CodeSignature/CodeResources : \
	$(MAC_CONTENTS)/MacOS/$(BINARY_NAME) \
	$(MAC_CONTENTS)/Resources/AppIcon.icns \
	$(MAC_CONTENTS)/Frameworks/libopenvr_api.dylib \
	$(MAC_CONTENTS)/Info.plist
	codesign --force --deep --sign - $(BINARY_NAME).app

$(MAC_CONTENTS)/Frameworks/libopenvr_api.dylib: libopenvr_api.dylib
	@mkdir -p $(MAC_CONTENTS)/Frameworks
	cp $< $@

# copies the binary, and tells it where to find libraries
$(MAC_CONTENTS)/MacOS/$(BINARY_NAME): $(BINARY_NAME).bin
	@mkdir -p $(MAC_CONTENTS)/MacOS
	cp $< $@
	install_name_tool -change @loader_path/libopenvr_api.dylib @loader_path/../Frameworks/libopenvr_api.dylib $@
	install_name_tool -add_rpath "@loader_path/../Frameworks" $@
# end build the App Bundle

# Housekeeping
clean:
	@rm -rf build $(BINARY_NAME) $(BINARY_NAME).exe $(BINARY_NAME).bin $(BINARY_NAME).app libMoltenVK.dylib libopenvr_api.dylib openvr_api.dll libopenvr_api.so


# Create build directories
$(shell	mkdir -p build/lin/GL build/win/GL build/mac/AppIcon.iconset)


