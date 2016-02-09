Neutral Image (working title)
Interactive Photo Installation (c) 2016
Concept: Marianne Holm Hansen
Software Develoment: Marinos Koutsomichalis <marinos@marinoskoutsomichalis.com>

This code is distributed?? under the Attribution-NonCommercial-ShareAlike 3.0 Unported Creative Commons Licence (details here: http://creativecommons.org/licenses/by-nc-sa/3.0/deed.en_US).???

> THIS SOFTWARE IS DISTRIBUTED AS IS WITH NO GUARANTEE OF FUNCTIONALITY OR RELEVANCE TO THIRD PARTY PROJECTS - THE DEVELOPERS ARE THUS NOT TO BE HELD RESPONSIBLE FOR PERSONAL INJURIES, HARDWARE, FINANCIAL, OR ANY OTHER LOSSES AND IN GENERAL FOR ANY MISFORTUNATE EVENT CAUSED BY THE DESCRIBED SOFTWARE/HARDWARE OR IN THE PROCESS OF ATTEMPTING TO IMPLEMENT IT. IF YOU DON'T AGREE WITH THE AFOREMENTIONED YOUR ARE NOT ALLOWED TO USE THE MATERIAL PROVIDED HEREIN

DESCRIPTION:
Software :

General Information:
* designed and implemented by Marinos Koutsomichalis in C++14 using STL, Boost and openCV.
* the application is tested and reported to work in Mac Os X 10.11 operating system with clang
* in its current form the application is not cross platform.

To Compile:
Given that xcode, xcode-tools, cmake, doxygen, openCV, Boost and clang is installed, try:

cd oiko-threads app
mkdir build
cd build
cmake <OPTIONS> ..
make

this will create an Install folder with the executable, the data assets, some tests and the auto-generated documentation. Options are as follows:

    * -DCMAKE_BUILD_MODE=Debug or =Release -> to switch between debug/release builds,default it Release
    * -DTEST_MODE=OFF -> do not compile/run the tests
	* -DCMAKE_LINKAGE=Static or =Dynamic -> link against dynamic/static libraries

more, there are a number of important constants that can be defined in compile time

    * -DCAPTURE_WIDTH			-> default width of the capture window
	* -DCAPTURE_HEIGHT			-> default height of the capture window
	* -DPROJECTION_WIDTH	   	-> default width of the projection window
	* -DPROJECTION_HEIGHT  		-> default height of the projection window
	* -DANIMATION_SPEED			-> default animation speed
	* -DCAPTURE_WINDOW_NAME	   	-> the default name of the capture window  	   
	* -DCAMERA	                -> default camera to use
	* etc
	
To Use:
