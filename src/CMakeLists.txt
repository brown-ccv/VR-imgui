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
  KeyBoardWidget
  )

#set(PROJECT_SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLEW)

add_subdirectory(imgui)
 
set(KEYBOARD_SOURCE 
    KeyBoardWidget/KeyBoard/ButtonCommand.cpp
	KeyBoardWidget/KeyBoard/DeleteCharCommand.cpp
	KeyBoardWidget/KeyBoard/KeyBoardImGuiHandler.cpp
	KeyBoardWidget/KeyBoard/KeyBoardModel.cpp
    KeyBoardWidget/KeyBoard/KeyBoardUIElement.cpp
	KeyBoardWidget/KeyBoard/KeyBordInputText.cpp
	KeyBoardWidget/KeyBoard/OpenKeyBoardCommand.cpp
	KeyBoardWidget/KeyBoard/PrintCharCommand.cpp
	KeyBoardWidget/KeyBoard/SendKeyBoardEventCommand.cpp
	KeyBoardWidget/KeyBoard/ShiftCharsCommand.cpp
	KeyBoardWidget/KeyBoard/VRInputTextKeyBoard.cpp
	KeyBoardWidget/KeyBoard/VRKeyBoard.cpp
	KeyBoardWidget/KeyBoard/VRKeyBoardButton.cpp
	KeyBoardWidget/KeyBoard/VRKeyBoardCharacterButton.cpp
	KeyBoardWidget/KeyBoard/VRKeyBoardPrintDelButton.cpp
	KeyBoardWidget/KeyBoard/VRKeyBoardPrintSpaceButton.cpp
	KeyBoardWidget/KeyBoard/VRKeysPanel.cpp
	KeyBoardWidget/UI/VR_UIButton.cpp
	KeyBoardWidget/UI/VR_UIInputTextField.cpp
	KeyBoardWidget/UI/VR_UIPanel.cpp
	KeyBoardWidget/UI/VR_UIWindow.cpp
	
	)

set(KEYBOARD_COMMON_INCLUDE
   KeyBoardWidget/common/common.h 
)

set(KEYBOARD_COMMAND_INCLUDE
    KeyBoardWidget/Command/Command.h  
)

set(KEYBOARD_UI_INCLUDE
    KeyBoardWidget/UI/UIWidget.h
	KeyBoardWidget/UI/VR_UIButton.h
	KeyBoardWidget/UI/VR_UIInputTextField.h
	KeyBoardWidget/UI/VR_UIPanel.h
	KeyBoardWidget/UI/VR_UIWindow.h
)

set(KEYBOARD_INCLUDE
     
	
	KeyBoardWidget/KeyBoard/ButtonCommand.h
	KeyBoardWidget/KeyBoard/DeleteCharCommand.h
	KeyBoardWidget/KeyBoard/KeyBoardImGuiHandler.h
	KeyBoardWidget/KeyBoard/KeyBoardModel.h
	KeyBoardWidget/KeyBoard/KeyBoardUIElement.h
	KeyBoardWidget/KeyBoard/KeyBordInputText.h
	KeyBoardWidget/KeyBoard/OpenKeyBoardCommand.h
	KeyBoardWidget/KeyBoard/PrintCharCommand.h
	KeyBoardWidget/KeyBoard/SendKeyBoardEventCommand.h
	KeyBoardWidget/KeyBoard/ShiftCharsCommand.h
	KeyBoardWidget/KeyBoard/VRInputTextKeyBoard.h
	KeyBoardWidget/KeyBoard/VRKeyBoard.h
	KeyBoardWidget/KeyBoard/VRKeyBoardButton.h
	KeyBoardWidget/KeyBoard/VRKeyBoardCharacterButton.h
	KeyBoardWidget/KeyBoard/VRKeyBoardPrintDelButton.h
	KeyBoardWidget/KeyBoard/VRKeyBoardPrintSpaceButton.h
	KeyBoardWidget/KeyBoard/VRKeysPanel.h
	
	
)
    
ADD_LIBRARY(ImguiVR STATIC
	#main.cpp
	VRMenu.cpp
	VRMenu.h
	VRMenuHandler.cpp
	VRMenuHandler.h
	${imgui_SOURCES} 
    #${KEYBOARD_SOURCE}
	#${KEYBOARD_INCLUDE}
	#${KEYBOARD_UI_INCLUDE}
	#${KEYBOARD_COMMAND_INCLUDE}
	#${KEYBOARD_COMMON_INCLUDE}
)

 message(STATUS  "GLEW0 : ${GLEW_SHARED_LIBRARY_DEBUG}")


target_link_libraries(ImguiVR
  ${OPENGL_LIBRARY}
  ${GLM_LIBRARIES}
)

target_link_libraries( ImguiVR debug ${GLEW_SHARED_LIBRARY_DEBUG})
target_link_libraries( ImguiVR optimized ${GLEW_SHARED_LIBRARY_RELEASE})

install(FILES VRMenu.h VRMenuHandler.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
#install(FILES ${KEYBOARD_INCLUDE} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/KeyBoardWidget/KeyBoard)
#install(FILES ${KEYBOARD_UI_INCLUDE} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/KeyBoardWidget/UI)
#install(FILES ${KEYBOARD_COMMAND_INCLUDE} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/KeyBoardWidget/Commmand)
#install(FILES ${KEYBOARD_COMMON_INCLUDE} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/KeyBoardWidget/common)

		 
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

    message(STATUS  "GLEW : ${GLEW_LIBRARIES}")

	TARGET_LINK_LIBRARIES( ImguiVR_test
				PUBLIC MinVR::MinVR
				ImguiVR
				${OPENGL_LIBRARY}
			)
    
    TARGET_LINK_LIBRARIES( ImguiVR_test PUBLIC debug ${GLEW_SHARED_LIBRARY_DEBUG})
    TARGET_LINK_LIBRARIES( ImguiVR_test PUBLIC optimized ${GLEW_SHARED_LIBRARY_RELEASE})
			
	install(TARGETS ImguiVR_test DESTINATION bin)

endif(BUILD_WITH_EXAMPLE)


if(BUILD_WITH_VIRTUALKEYBOARD)
   add_subdirectory(KeyBoardWidget)
endif(BUILD_WITH_VIRTUALKEYBOARD)