
message(STATUS "trying to find glew")

FIND_PATH(GLEW_INCLUDE_DIR GL/glew.h)

find_library(GLEW_SHARED_LIBRARY_RELEASE
             NAMES GLEW glew glew32
             NAMES_PER_DIR
             PATH_SUFFIXES lib lib64 libx32 lib/Release/${_arch}
             PATHS ENV GLEW_ROOT)
			 
find_library(GLEW_SHARED_LIBRARY_DEBUG
             NAMES GLEWd glewd glew32d
             NAMES_PER_DIR
             PATH_SUFFIXES lib lib64
             PATHS ENV GLEW_ROOT)