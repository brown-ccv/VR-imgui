project(ImguiVR)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

include_directories(
  ${MINVR_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIRS}
  ${GLM_INCLUDE_DIRS}
  imgui
  imgui/impl
  )

set(PROJECT_SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLEW)


add_subdirectory(imgui)
  
  
  
ADD_LIBRARY(ImguiVR STATIC
	#main.cpp
	VRMenu.cpp
	VRMenu.h
	VRMenuHandler.cpp
	VRMenuHandler.h
	${imgui_SOURCES} 
)

target_link_libraries(ImguiVR
  PUBLIC MinVR::MinVR
  ${OPENGL_LIBRARY}
  ${GLEW_LIBRARY}
  ${GLM_LIBRARIES}
)

add_executable(ImguiVR_test
	main.cpp 
)

TARGET_LINK_LIBRARIES( ImguiVR_test
				ImguiVR )