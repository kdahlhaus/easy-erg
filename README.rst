easy-erg - a C++ Wrapper for the Concept II Rowing Machine Interface
====================================================================
Copyright 2014 by Kevin Dahlhausen (kpd@powertwenty.com)

Overview
--------
This package contains a C++ library and demonstration files that make working with the Concept 2 Software Development Kit (http://www.concept2.com/service/software/software-development-kit) easy.  This library runs against the (lastest as of writing) version of the API - the one with documents labeled version 0.14

    ErgNet net = ErgNet();
    int numErgs = net.discoverErgs();
    Pm3Erg erg = Pm3Erg(0); // could actually be a PM3, PM4, or PM5
    ErgState state;
    while (1)
    {
        erg.updateState(&state);
        cout << "power: " << state.watts << " (watts)  " << "numStrokes: " << state.numStrokesIn << "  meters/stroke: " << state.metersPerStroke << "\n"; 
    }      


License
-------
This software is licensed under the GNU GPL.  Please see the file 'LICENSE' for details.  Note that this license requires any software using this library to make source code available. 

Should you not wish to use the software under the GPL license, please contact Kevin Dahlhausen (kpd@powertwenty.com) to discuss alternative licensing.


Building and running the demo programs
--------------------------------------

PREREQS:
.......
1. version 1.24 of the Concept 2 Software Development Kit
2. cygwin with g++ 
3. dlltool installed
4. pexports installed

Steps
.....
1. Extract the concept 2 SDK and set the environment variable 'CONCEPT2SDK' to point to the directory that contains the DLL's and .h files.  
2. run "make"
3. connect a Concept 2 ergometer via USB
4. cd to demo
5. run demo.exe or any of the other demonstration files there.


Runnng the Tests
----------------
1. Install GoogleMock (https://code.google.com/p/googlemock/) somewhere.
2. Point GMOCK_DIR (test/Makefile.test around line 24) and GTEST_DIR (test/Makefile.test around line 20) to the installation.
3. make all tests



Using in your own code
----------------------
The command line would be something like this:

    g++ -mno-cygwin -I../easy_erg/build -I../easy_erg/lib/sdk/PC -L../easy_erg/build  my_app.cpp -lEasyErg -lRPPM3USB -lRPPM3DDI
    
You also have to copy the DLL's from the demo directory to some directory on the path or where the new program resides.


ToDo / Notes
------------
* Average watts is calculated by sampling power at a fixed rate independent of the position or timing in the stroke.  We might want to tie it to a position in the stroke.
