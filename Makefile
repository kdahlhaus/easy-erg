
#point to location of concept II API
C2INC=lib/sdk/PC
C2LIBDIR=lib/sdk/PC

# tools that create gcc libraries from DLL's
PEXPORTS=pexports
DLLTOOL=dlltool


CFLAGS      =-mno-cygwin
SWIGFLAGS	=-strict 0 -c++ -shadow -dhtml
INCLUDES    = -I$(C2INC) -Isrc
LDFLAGS     = -L$(C2LIBDIR) -Lsrc -L$(BUILD)
LIBS        = -lPM3DDICP -lPM3USBCP 

BUILD       = build

all:  build_dir c2libs demos 



.PHONY: clean
clean:
	rm -rf $(BUILD) src/*.o demo/*.o demo/*.ini demo/*.DLL demo/*.exe demo/*.def demo/*.a
 
COMMON_OBJECTS = src/Pm3Erg.o src/ErgNet.o src/CSafeDll.o src/CIIHeaders.o src/ErgState.o src/SlideRatio.o


lib: build_dir $(BUILD)/libEasyErg.a

build/libEasyErg.a: $(COMMON_OBJECTS)
	$(AR) rcs $@ $^
	cp src/*.h $(BUILD)

src/%.o: src/%.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

 


demos: c2libs demo/demo.exe demo/demoIntervals.exe demo/demoState.exe
	@echo demo executables are in demo/

demo/%.exe: demo/%.o build/libEasyErg.a
	$(CXX)  $(CFLAGS)  $(INCLUDES) $(LDFLAGS) -o $@ $^ $(LIBS) -lEasyErg



build_dir:  
	@mkdir -p $(BUILD)

c2libs: build_dir $(BUILD)/libPM3DDICP.a $(BUILD)/libPM3CsafeCP.a $(BUILD)/libPM3USBCP.a


$(BUILD)/libPM3DDICP.a:  demo/PM3DDICP.DLL
	$(PEXPORTS) $(C2LIBDIR)/PM3DDICP.DLL > $(BUILD)/PM3DDICP.def
	$(DLLTOOL) --dllname PM3DDICP.dll --def $(BUILD)/PM3DDICP.def --output-lib $(BUILD)/libPM3DDICP.a

$(BUILD)/libPM3USBCP.a:  demo/PM3USBCP.DLL
	$(PEXPORTS) $(C2LIBDIR)/PM3USBCP.DLL > $(BUILD)/PM3USBCP.def
	$(DLLTOOL) --dllname PM3USBCP.dll --def $(BUILD)/PM3USBCP.def --output-lib $(BUILD)/libPM3USBCP.a

$(BUILD)/libPM3CsafeCP.a:  demo/PM3CsafeCP.DLL
	$(PEXPORTS) -o $(C2LIBDIR)/PM3CsafeCP.DLL > $(BUILD)/PM3CsafeCP.def
	$(DLLTOOL) --dllname PM3CsafeCP.dll --def $(BUILD)/PM3CsafeCP.def --output-lib $(BUILD)/libPM3CsafeCP.a

demo/%.DLL:  
	cp $(C2LIBDIR)/$*.DLL demo
	chmod 555 demo/$*.DLL

c2inis: demo/RPecodesPM3DDI.ini demo/PM3CsafeCP.ini 

demo/%.ini:
	cp $(C2LIBDIR)/$*.ini demo


