FILE(GLOB SOURCE_LOCAL RELATIVE ${PROJECT_SOURCE_DIR}
    "*.cpp"
)

FILE(GLOB HEADERS_LOCAL RELATIVE ${PROJECT_SOURCE_DIR}
    "*.h"
)

if(NOT BUILD_WITH_FILEBROWSER)
	list(REMOVE_ITEM HEADERS_LOCAL imgui/imfilebrowser.h)
endif()

add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLEW)

add_subdirectory(impl)
add_subdirectory(Dirent)

SET(imgui_SOURCES
	${imgui_SOURCES}
	${HEADERS_LOCAL}
    ${SOURCE_LOCAL}	
	PARENT_SCOPE
)