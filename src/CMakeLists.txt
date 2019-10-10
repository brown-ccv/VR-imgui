project(ImguiVR)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

include_directories(
  ${MINVR_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}
  ${GLM_INCLUDE_DIRS}
  imgui
  imgui/impl
  )

#set(PROJECT_SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR})

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
  ${OPENGL_LIBRARY}
  ${GLEW_SHARED_LIBRARIES}
  ${GLM_LIBRARIES}
)
install(FILES VRMenu.h VRMenuHandler.h  DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(FILES imgui/imgui.h imgui/imgui_internal.h imgui/imconfig.h imgui/imstb_textedit.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/imgui)
if(BUILD_WITH_FILEBROWSER)
	install(FILES imgui/imfilebrowser.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/imgui)
endif()
install(TARGETS ImguiVR EXPORT ImguiVR DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(EXPORT ImguiVR DESTINATION ${CMAKE_INSTALL_PREFIX}/)
target_include_directories(ImguiVR INTERFACE
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include> 
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include/imgui>)
install(FILES ImguiVRConfig.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/)

if(BUILD_WITH_EXAMPLE)
	add_executable(ImguiVR_test
		minvr_example.cpp 
	)

	TARGET_LINK_LIBRARIES( ImguiVR_test
				PUBLIC MinVR::MinVR
				ImguiVR
			)
			
	install(TARGETS ImguiVR_test DESTINATION bin)

endif(BUILD_WITH_EXAMPLE)