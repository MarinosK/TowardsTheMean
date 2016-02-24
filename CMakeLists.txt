#by Marinos Koutsomichalis, Heraklion 2015

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project("Towards The Mean")

# set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /opt/local/lib) 

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -stdlib=libc++ -DBOOST_LOG_USE_NATIVE_SYSLOG")
set (SOURCES       src/helper.cpp
		   src/capture.cpp
		   src/projection.cpp
		   )
set (APP_SOURCES   src/main.cpp	)
set (AVERAGE_SOURCES   src/average.cpp	)
set (TEST_SOURCES  src/tests.cpp
                   test_lib/gmock-all.cc
		   test_lib/gtest-all.cc)
include_directories(/opt/local/include
		    /usr/local/include
		    /usr/local/include/gmock
		    /usr/local/include/gtest
		    /opt/local/include/freetype2
		    src
		    )
set (DYN_LINK       /opt/local/lib
                    /usr/local/lib)
set (STATIC_LINK    static)
		  
set(CMAKE_INSTALL_PREFIX  "${CMAKE_BINARY_DIR}/Install")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Install")

find_package(Boost 1.59.0 COMPONENTS
	            log
		    log_setup
		    thread
		    date_time
		    filesystem
		    system
		    program_options
		    REQUIRED)	  
find_package(OpenCV 3.0 REQUIRED)
if (OpenCV_FOUND)
  message( "-- OpenCV found")
endif()

find_package(OpenGL REQUIRED)
if (OpenGL_FOUND)
  message( "-- OpenGL found")
endif()

find_library(POCO_FOUNDATION PocoFoundation)
if (PocoFoundation_FOUND)
  message( "-- POCO FOUNDATION found")
endif()
find_library(POCO_NET PocoNet)
if (PocoNet_FOUND)
  message( "-- POCO NET found")
endif()

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PocoFoundation_INCLUDE_DIRS})
include_directories(${PocoNet_INCLUDE_DIRS})

#user configuration
if(${CMAKE_LINKAGE} MATCHES Static)
  message("-- STATIC LINKAGE SELECTED")
  link_directories(${STATIC_LINK})
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_USE_STATIC_RUNTIME OFF)
  set(CMAKE_FIND_LIBRARY_SUFFIXES	".a")
  set(BUILD_SHARED_LIBRARIES OFF)
else()
  message("-- DYNAMIC LINKAGE SELECTED")
  #find libraries
  link_directories(${DYN_LINK})
  link_directories(${Boost_LIBRARY_DIR})
  link_directories(${Boost_LIBRARY_DIR})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_ALL_DYN_LINK")
  set(Boost_USE_STATIC_LIBS OFF)
endif()

if(${CMAKE_BUILD_MODE} MATCHES Release)
  message("-- RELEASE MODE ON")
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O3")
else()
  message("-- DEBUG MODE ON")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DDEBUG")
endif()

# core
add_library(core ${SOURCES})
target_link_libraries(core ${OpenCV_LIBS})
target_link_libraries(core ${Boost_LIBRARIES})
target_link_libraries(core ${OPENGL_LIBRARIES})
target_link_libraries(core GLEW)
target_link_libraries(core glfw)
target_link_libraries(core ftgl)
target_link_libraries(core freetype)
target_link_libraries(core ${POCO_FOUNDATION} ${POCO_NET})
#target_link_libraries(core GLEW_1130)

# average command line tool
add_executable(average ${AVERAGE_SOURCES})
target_link_libraries(average core)

# TowardsTheMean app
add_executable(TowardsTheMean ${APP_SOURCES})
target_link_libraries(TowardsTheMean core)
#target_link_libraries(TowardsTheMean ${Boost_LIBRARIES})
#target_link_libraries(oiko-threads ${OpenCV_LIBS})

add_custom_command(TARGET TowardsTheMean PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:TowardsTheMean>/assets)
add_custom_command(TARGET TowardsTheMean PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory
  $<TARGET_FILE_DIR:TowardsTheMean>/logs)
add_custom_command(TARGET TowardsTheMean PRE_BUILD #this is a hack around a bug really, it shouldn't be needed
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/lib $<TARGET_FILE_DIR:TowardsTheMean>/lib)

# configure and run tests		 
option(TEST_MODE "Compile and run tests" ON)
if(TEST_MODE)
      message("-- TEST_MODE ON")
add_executable(ttm_tests ${TEST_SOURCES})
target_link_libraries(ttm_tests pthread)
target_link_libraries(ttm_tests core)
add_custom_command( OUTPUT run_tests.lic #run tests
  COMMAND cd $<TARGET_FILE_DIR:TowardsTheMean> && ./ttm_tests && cd ${CMAKE_SOURCE_DIR} #need to change directory to get the correct path
  DEPENDS ttm_tests
  COMMENT "Running Tests"
  VERBATIM )
add_custom_target( RunTests ALL DEPENDS run_tests.lic)
else()
    message("-- TEST_MODE OFF")
endif()



