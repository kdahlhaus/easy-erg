# $(CONCEPT2SDK) should point to location of concept 2 SDK

# tools that create gcc libraries from DLL's
PEXPORTS=pexports
DLLTOOL=dlltool


#CFLAGS      =-mno-cygwin
SWIGFLAGS	=-strict 0 -c++ -shadow -dhtml
INCLUDES    = -I$(CONCEPT2SDK) -Isrc
LDFLAGS     = -Lsrc -L$(BUILD)
LIBS        = -lPM3DDICP -lPM3USBCP 

BUILD       = build

all:  check_for_sdk build_dir c2libs build/libEasyErg.a demos 

.PHONY: check_for_sdk
check_for_sdk:
	@ # check that environment variable is set to the SDK
	@if [ "$(CONCEPT2SDK)" = "" ]; then echo "ERROR: environment variable 'CONCEPT2SDK' must point to the location of the Concept 2 SDK DLL's and headers"; false; fi
	@if [ ! -e  "$(CONCEPT2SDK)/PM3DDICP.dll" ];  then echo "ERROR: PM3DDICP.dll is not found in location pointed to by CONCEPT2SDK: $(CONCEPT2SDK)"; false; fi
	@if [ ! -e  "$(CONCEPT2SDK)/PM3USBCP.dll" ];  then echo "ERROR: PM3USBCP.dll is not found in location pointed to by CONCEPT2SDK: $(CONCEPT2SDK)"; false; fi
	@if [ ! -e  "$(CONCEPT2SDK)/PM3CSafeCP.dll" ];  then echo "ERROR: PM3CSafeCP.dll is not found in location pointed to by CONCEPT2SDK: $(CONCEPT2SDK)"; false; fi
	@if [ ! -e  "$(CONCEPT2SDK)/PM3DDICP.h" ];  then echo "ERROR: PM3DDICP.h is not found in location pointed to by CONCEPT2SDK: $(CONCEPT2SDK)"; false; fi
	@if [ ! -e  "$(CONCEPT2SDK)/PM3USBCP.h" ];  then echo "ERROR: PM3USBCP.h is not found in location pointed to by CONCEPT2SDK: $(CONCEPT2SDK)"; false; fi
	@if [ ! -e  "$(CONCEPT2SDK)/PM3CSafeCP.h" ];  then echo "ERROR: PM3CSafeCP.h is not found in location pointed to by CONCEPT2SDK: $(CONCEPT2SDK)"; false; fi



.PHONY: clean
clean:
	rm -rf $(BUILD) src/*.o demo/*.o  demo/*.DLL demo/*.exe demo/*.def demo/*.a 
	cd test; make clean
 
COMMON_OBJECTS = src/Erg.o src/ErgNet.o src/CSafeDll.o src/CIIHeaders.o src/ErgState.o src/SlideRatio.o

src/Erg.o: src/Erg.cpp src/RunningAverage.h src/CIIHeaders.h src/ErgState.h src/SlideRatio.h src/ErgNet.h

lib: build_dir $(BUILD)/libEasyErg.a

build/libEasyErg.a: $(COMMON_OBJECTS)
	$(AR) rcs $@ $^
	cp src/*.h $(BUILD)

src/%.o: src/%.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

 


demos: c2libs demo/demoDiscoverErgs.exe demo/demo.exe demo/demoIntervals.exe demo/demoState.exe
	@echo demo executables are in demo/

demo/%.exe: demo/%.o build/libEasyErg.a
	$(CXX)  $(CFLAGS)  $(INCLUDES) $(LDFLAGS) -o $@ $^ $(LIBS) -lEasyErg

tests: test/test_running_average.exe
	@echo test executables are in test/
	cd test; rm -f test*.exe; make all run

test/%.exe: test/%.o
	$(CXX)  $(CFLAGS)  $(INCLUDES) $(LDFLAGS) -o $@ $^ $(LIBS) 
 

build_dir:  
	@mkdir -p $(BUILD)

c2libs: build_dir $(BUILD)/libPM3DDICP.a $(BUILD)/libPM3CsafeCP.a $(BUILD)/libPM3USBCP.a


$(BUILD)/libPM3DDICP.a:  demo/PM3DDICP.DLL
	$(PEXPORTS) $(CONCEPT2SDK)/PM3DDICP.DLL > $(BUILD)/PM3DDICP.def
	$(DLLTOOL) --dllname PM3DDICP.dll --def $(BUILD)/PM3DDICP.def --output-lib $(BUILD)/libPM3DDICP.a

$(BUILD)/libPM3USBCP.a:  demo/PM3USBCP.DLL
	$(PEXPORTS) $(CONCEPT2SDK)/PM3USBCP.DLL > $(BUILD)/PM3USBCP.def
	$(DLLTOOL) --dllname PM3USBCP.dll --def $(BUILD)/PM3USBCP.def --output-lib $(BUILD)/libPM3USBCP.a


# NOTE:  CSafeDll.cpp loads the DLL dynamically, so if the DLL name changes it must be adjusted there as well AND the functional ordinals must be adjusted
$(BUILD)/libPM3CsafeCP.a:  demo/PM3CsafeCP.DLL
	$(PEXPORTS) -o $(CONCEPT2SDK)/PM3CsafeCP.DLL > $(BUILD)/PM3CsafeCP.def
	$(DLLTOOL) --dllname PM3CsafeCP.dll --def $(BUILD)/PM3CsafeCP.def --output-lib $(BUILD)/libPM3CsafeCP.a

demo/%.DLL:  
	cp $(CONCEPT2SDK)/$*.DLL demo
	chmod 555 demo/$*.DLL



