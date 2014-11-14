easy-erg - a C++ Wrapper for the Concept II Rowing Machine Interface
====================================================================
Copyright 2014 by Kevin Dahlhausen (kpd@powertwenty.com)

Overview
--------
This package contains a library and demonstration files that make working with the Concept 2 Software Development Kit (http://www.concept2.com/service/software/software-development-kit).  This library runs against the (lastest as of writing) version of the API - the one with documents labeled version 0.14

License
-------
This software is licensed under the GNU GPL.  Please see the file 'LICENSE' for details.  Note that this license requires any software using this library to make source code available. 

Should you not wish to use the software under the GPL license, please contact Kevin Dahlhausen (kpd@powertwenty.com) to discuss alternative licensing.


Building and running the demo programs
--------------------------------------

PREREQUISITES
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
4. cd to build
5. demo.exe


Using in your own code
----------------------
The command line would be something like this:

    g++ -mno-cygwin -I../easy_erg/build -I../easy_erg/lib/sdk/PC -L../easy_erg/build  my_app.cpp -lEasyErg -lRPPM3USB -lRPPM3DDI
    
You also have to copy the DLL's from the demo directory to some directory on the path or where the new program resides.
