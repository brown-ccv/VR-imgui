project(ImguiVR)

list(APPEND CMAKE_PREFIX_PATH ../../build/install)

# Using 3.9 to get a modern version of FindOpenGL.cmake
cmake_minimum_required (VERSION 3.9) 

# Dependencies that are auto-downloaded, built, and installed for you will go here.  So, you 
# may use this even if not planning to install this particular project on your system.
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR "${CMAKE_INSTALL_PREFIX}" STREQUAL "")
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
endif()

list(APPEND CMAKE_MODULE_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_INSTALL_PREFIX})
list(INSERT CMAKE_PREFIX_PATH 0 ${CMAKE_INSTALL_PREFIX})

set(CMAKE_CXX_STANDARD 11)
    if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)

#---------------------- Find & Add Dependencies ----------------------

set(EXTERNAL_CMAKE_SCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# MinVR (linked with an imported cmake target so no need to specify include dirs)
find_package(MinVR REQUIRED)	
	
add_subdirectory(src)
