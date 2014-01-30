easy_erg - a C++ Wrapper for the Concept II Rowing Machine Interface

This package contains a library and demonstration files that make working with the Concept 2 Software Development Kit (http://www.concept2.com/service/software/software-development-kit).  This library was developed against version 1.24 of the API.

* This software is licensed under the GNU GPL.  Please see the file 'LICENSE' for details.  Note that this license requires any software using this library to make source code available. 

Should you not wish to use the software under a different license, please contact Kevin Dahlhausen (kpd@powertwenty.com) to discuss alternative licensing.


Building and running the demo programs:

PREREQS:
    cygwin with G++
    dlltool installed
    pexports installed

1. Extract the concept 2 API libraries to lib/SDK. The DLL's should be in lib/SDK/PC.
2. run "make"
3. connect a Concept 2 ergometer via USB
4. cd to build
5. demo.exe
