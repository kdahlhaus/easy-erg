
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
LIBS        = -lRPPM3DDI -lRPPM3USB 

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

c2libs: build_dir $(BUILD)/libRPPM3DDI.a $(BUILD)/libRPPM3CSAFE.a $(BUILD)/libRPPM3USB.a c2inis


$(BUILD)/libRPPM3DDI.a:  demo/RPPM3DDI.DLL
	$(PEXPORTS) $(C2LIBDIR)/RPPM3DDI.DLL > $(BUILD)/RPPM3DDI.def
	$(DLLTOOL) --dllname RPPM3DDI.dll --def $(BUILD)/RPPM3DDI.def --output-lib $(BUILD)/libRPPM3DDI.a

$(BUILD)/libRPPM3USB.a:  demo/RPPM3USB.DLL
	$(PEXPORTS) $(C2LIBDIR)/RPPM3USB.DLL > $(BUILD)/RPPM3USB.def
	$(DLLTOOL) --dllname RPPM3USB.dll --def $(BUILD)/RPPM3USB.def --output-lib $(BUILD)/libRPPM3USB.a

$(BUILD)/libRPPM3CSAFE.a:  demo/RPPM3CSAFE.DLL
	$(PEXPORTS) -o $(C2LIBDIR)/RPPM3CSAFE.DLL > $(BUILD)/RPPM3CSAFE.def
	$(DLLTOOL) --dllname RPPM3CSAFE.dll --def $(BUILD)/RPPM3CSAFE.def --output-lib $(BUILD)/libRPPM3CSAFE.a

demo/%.DLL:  
	cp $(C2LIBDIR)/$*.DLL demo
	chmod 555 demo/$*.DLL

c2inis: demo/RPecodesPM3DDI.ini demo/RPPM3CSAFE.ini 

demo/%.ini:
	cp $(C2LIBDIR)/$*.ini demo


