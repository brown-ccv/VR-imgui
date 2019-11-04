project(ImguiVR)

# Using 3.9 to get a modern version of FindOpenGL.cmake
cmake_minimum_required (VERSION 3.9) 

# Dependencies that are auto-downloaded, built, and installed for you will go here.  So, you 
# may use this even if not planning to install this particular project on your system.
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR "${CMAKE_INSTALL_PREFIX}" STREQUAL "")
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
endif()

list(APPEND CMAKE_MODULE_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_INSTALL_PREFIX})

option(BUILD_WITH_FILEBROWSER "If enabled, the imgui-filebrowser will be included. However it requires C++17. If disabled it will be excluded and only requires C++11." OFF)
option(BUILD_WITH_EXAMPLE "If enabled, a MinVR example program will be also compiled and installed. It requires MinVR to work." OFF)

if(BUILD_WITH_FILEBROWSER)
	set(CMAKE_CXX_STANDARD 17)
	#if(LINUX)
	#  add_compile_options(-lstdc++fs -std=c++17)
	#endif()
else()
	set(CMAKE_CXX_STANDARD 11)
endif()

add_compile_options(-lstdc++fs)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_definitions(-D_HAS_STD_BYTE=0)
#---------------------- Find & Add Dependencies ----------------------

set(EXTERNAL_CMAKE_SCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# MinVR (linked with an imported cmake target so no need to specify include dirs)
if(BUILD_WITH_EXAMPLE)
	find_package(MinVR REQUIRED)	
endif(BUILD_WITH_EXAMPLE)

add_subdirectory(src)
