## Towards The Mean ##

Interactive Photo Installation (c) 2016
Concept: Marianne Holm Hansen
Software Develoment: Marinos Koutsomichalis <marinos@marinoskoutsomichalis.com>

This code is distributed under the Attribution-NonCommercial-ShareAlike 3.0 Unported Creative Commons Licence (details here: http://creativecommons.org/licenses/by-nc-sa/3.0/deed.en_US).

> THIS SOFTWARE IS DISTRIBUTED AS IS WITH NO GUARANTEE OF FUNCTIONALITY OR RELEVANCE TO THIRD PARTY PROJECTS - THE DEVELOPER IS NOT TO BE HELD RESPONSIBLE FOR PERSONAL INJURIES, HARDWARE, FINANCIAL, OR ANY OTHER LOSSES AND IN GENERAL FOR ANY MISFORTUNATE EVENT CAUSED BY THE DESCRIBED SOFTWARE OR IN THE PROCESS OF ATTEMPTING TO IMPLEMENT IT. IF YOU DON'T AGREE WITH THE AFOREMENTIONED YOUR ARE NOT ALLOWED TO USE THE MATERIAL PROVIDED HEREIN

# To Compile #

You need the following dependencies:

    * xcode
    * xcode-tools
    * cmake
    * boost  1.59
    * openCV 3.1 (the master from github (February 2016))
    * glfw-devel (as of February 2016)
    * glew
	* freetype
	* ftgl
	* POCO
	
(you might have to edit CMakeLists.txt to update certain paths, etc)

then try:

	cd TowardsTheMean
	mkdir build
	cd build
	cmake <OPTIONS> ..
	make

this will create an Install folder with the executable, the data assets, and the tests. Options are as follows:

    * -DCMAKE_BUILD_MODE=Debug or =Release -> to switch between debug/release builds, default is Debug
    * -DTEST_MODE=OFF -> do not compile/run the tests
	* -DCMAKE_LINKAGE=Dynamic or =Static -> switch between dynamic/static linkage. Note that static mode will still link dynamically against libc++ and a number of other system-specific libraries

more, there are a number of properties that can be defined in compile time (check properties.h)

	* -DANIMATION_SPEED			-> default animation speed
	* -DCAMERA	                -> default camera to use
	* -DCAPTURED_IMAGE_WIDTH    -> the horizontal resolution of the photo 
	* -DCAPTURED_IMAGE_HEIGHT   -> the vertical resolution of the photo 
	* -DUNSAFE_OPTIMISATIONS    -> If defined turns on some unsafe optimisations
	* etc
	
## To calibrate ##

> It is very important that the CAPTURED_IMAGE_WIDTH/HEIGHT properties are set correctly and with respect to the camera's resolution---they don't necessarily have to equal the latter, bare in mind however that the scaling carried out internally is not proportional and may distort images if these values are not correctly set. Bare in mind that large values require greater memory overhead. 

## To Use ##

Once you’ve have installed everything you will end up in the build directory which will incude a `Install` folder, this is where you will find three executables: TowardsTheMean, ttm_tests, average. (The first is the actual application, the second is just some unit-tests you can ignore this and the last is the command-line I promised to automatically create session averages on your own). You will also find a number of folders with auxilary files. Note that once you run the program additional folders will be created to hold the session photos (photos) and averages (session_averages) and some log files (logs).
To use enter the Install directory: 

	*cd Install
	
Now you can simply invoke the application like this (you need two screens and at least a camera else you will get an error):

	*./TowardsTheMean
	
It might be necessary to properly calibrate the software, in particular the captured_image_width, captured_image_height, and camera properties. To do this when running the software pass values as arguments, e.g.:

	*./TowardsTheMean --camera 1 --captured_image_width 640 --captured_image_width 480 

(this will change the size of the captured images and select camera number 1, rather than the default number 0)

There more properties to play with such as desired anti-alliasing, the maximum number of photos in memory, the minutes after which to quit, the fade_in/out loops, etc. All these are documented in the README.rd file but you can also run this command for an overview: 

	*./TowardsTheMean --help
	
To quit either hit escape, or cmd-Q or go back to the terminal and type ‘q’ and press enter.  
Once you quit you will find a folder with the session photos inside ‘photos’ and a session average inside ’session_average'
You can use the average command like this:

	*./average --input XXXX
	
(XXXX should be the name of the folder with the photos to average, not that it will destructively CHANGE the original photos = to allign them - so do make a copy before you proceed)
