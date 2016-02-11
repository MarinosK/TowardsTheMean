## TowardsThemean ##

Interactive Photo Installation (c) 2016
Concept: Marianne Holm Hansen
Software Develoment: Marinos Koutsomichalis <marinos@marinoskoutsomichalis.com>

This code is distributed under the Attribution-NonCommercial-ShareAlike 3.0 Unported Creative Commons Licence (details here: http://creativecommons.org/licenses/by-nc-sa/3.0/deed.en_US).

> THIS SOFTWARE IS DISTRIBUTED AS IS WITH NO GUARANTEE OF FUNCTIONALITY OR RELEVANCE TO THIRD PARTY PROJECTS - THE DEVELOPER IS NOT TO BE HELD RESPONSIBLE FOR PERSONAL INJURIES, HARDWARE, FINANCIAL, OR ANY OTHER LOSSES AND IN GENERAL FOR ANY MISFORTUNATE EVENT CAUSED BY THE DESCRIBED SOFTWARE/HARDWARE OR IN THE PROCESS OF ATTEMPTING TO IMPLEMENT IT. IF YOU DON'T AGREE WITH THE AFOREMENTIONED YOUR ARE NOT ALLOWED TO USE THE MATERIAL PROVIDED HEREIN

# To Compile #

You need the following dependencies:
    * xcode
    * xcode-tools
    * cmake
    * boost  1.59
    * openCV 3.0
    * glfw-devel (as of February 2016)
    * glew
	* freetype
	* ftgl
(you might have to edit CMakeLists.txt to update certain paths, etc)

then try:

	cd TowardsTheMean
	mkdir build
	cd build
	cmake <OPTIONS> ..
	make

this will create an Install folder with the executable, the data assets, and the tests. Options are as follows:

    * -DCMAKE_BUILD_MODE=Debug or =Release -> to switch between debug/release builds,default it Release
    * -DTEST_MODE=OFF -> do not compile/run the tests
	* -DCMAKE_LINKAGE=Static or =Dynamic -> link against dynamic/static libraries

more, there are a number of properties that can be defined in compile time (check properties.h)

	* -DANIMATION_SPEED			-> default animation speed
	* -DCAMERA	                -> default camera to use
	* -DCAPTURED_IMAGE_WIDTH    -> the horizontal resolution of the photo 
	* -DCAPTURED_IMAGE_HEIGHT   -> the vertical resolution of the photo 
	* -DUNSAFE_OPTIMISATIONS    -> If defined turns on some unsafe optimisations
	* etc
	
## To calibrate ##

> It is very important that the CAPTURED_IMAGE_WIDTH/HEIGHT properties are set correctly and with respect to the camera's resolution---they don't necessarily have to equal the latter, bare in mind however that the scaling carried out internally is not proportional and may distort images if these values are not correctly set. Bare in mind that large values require greater memory overhead. 

# To Use #

run ./TowardsTheMean --help to see a list of all available configurations and then call again the appropriate flags. You may consider creating some script and an application wrapper. Note that this software is meant for the purposes of Marianne Holm Hansen artistic installation only and should not be used as is in any other occasion. 
