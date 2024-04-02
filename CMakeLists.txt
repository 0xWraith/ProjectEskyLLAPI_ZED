cmake_minimum_required(VERSION 3.7.2)
project(libProjectEskyLLAPI)

# Global configs 
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MT")

#if(NOT LINK_SHARED_ZED AND MSVC)
 #   message(FATAL_ERROR "LINK_SHARED_ZED OFF : ZED SDK static libraries not available on Windows")
#endif()


include(FetchContent)
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)
FetchContent_MakeAvailable(glm)


include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

set(CMAKE_VERBOSE_MAKEFILE ON)

if(WIN32)
  if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
    set(LIBRARY_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\lib\\x64")          # TODO: Update this variable to correct path - folder where realsense2.lib is found
    set(DLL_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\bin\\x64")              # TODO: Update this variable to correct path - folder where realsense2.dll is found
  else()
    set(LIBRARY_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\lib\\x86")          # TODO: Update this variable to correct path - folder where realsense2.lib is found
    set(DLL_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\bin\\x86")              # TODO: Update this variable to correct path - folder where realsense2.dll is found
  endif()

  set(PROJECT_BINARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/\$\(Configuration\)")                # TODO: Update this variable to correct path - folder to which your project will compile
  set(ADDITIONAL_INCLUDE_DIRS "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\include")   # TODO: Update this variable to correct path - folder where librealsense2 folder is found
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  #find_package (GLM REQUIRED)
  find_package(ZED 3)
  find_package(CUDA ${ZED_CUDA_VERSION} EXACT) 


  include_directories(${ZED_INCLUDE_DIRS})
  link_directories(${ZED_LIBRARY_DIR})
  include_directories(${CUDA_INCLUDE_DIRS})
endif()


set(ZED_LIBS ${ZED_LIBRARIES} ${CUDA_CUDA_LIBRARY} ${CUDA_CUDART_LIBRARY})
  

set(xvsdk_DIR "C:\\tools")
find_package( xvsdk REQUIRED )
set(xvsdk_INCLUDE ${xvsdk_INCLUDE_DIRS}xvsdk)

include_directories( ${xvsdk_INCLUDE} )
include_directories(${glm_SOURCE_DIR})

# find_package(realsense2 REQUIRED)

if(NOT REALSENSE2_FOUND)
  # set(REALSENSE2_FOUND "realsense2")
  message(WARN "Failed to find_library(realsense2)")
endif()

# OpenCV
set(OpenCV_STATIC ON)
find_package(OpenCV 4.5.0 REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

# includes
include_directories(${ADDITIONAL_INCLUDE_DIRS})
if(WIN32)
  include_directories(${GLM_INCLUDE_DIRS})
endif()
include_directories(./include/)

if(WIN32)  # Windows
  set(CMAKE_EXE_LINKER_FLAGS "-static")
  add_library(libProjectEskyLLAPIIntel SHARED
    ./src/Tracker/IntelT265/main.cpp
    ./src/Tracker/IntelT265/Tracker.h
    ./src/Tracker/IntelT265/Serial.cpp
    ./include/DollaryDooFilter.h
    ./include/Serial.h)
  set_target_properties(libProjectEskyLLAPIIntel PROPERTIES COMPILE_DEFINITIONS BUILDER_STATIC_DEFINE)
  target_link_libraries(libProjectEskyLLAPIIntel ${DEPENDENCIES} ${REALSENSE2_FOUND}  ${OpenCV_LIBS})

  #the secret sensor
  add_library(libProjectEskyLLAPIX SHARED
    ./src/Tracker/X/main.cpp
    ./src/Tracker/X/Tracker.h
    ./include/DollaryDooFilter.h)
  set_target_properties(libProjectEskyLLAPIX PROPERTIES COMPILE_DEFINITIONS BUILDER_STATIC_DEFINE)
  target_link_libraries(libProjectEskyLLAPIX ${DEPENDENCIES} ${xvsdk_LIBRARIES} ${OpenCV_LIBS})

  add_library(libProjectEskyLLAPIZED SHARED
    ./src/Tracker/ZED/main.cpp
    ./src/Tracker/ZED/Tracker_Zed.cpp
    ./src/UnityHooks.def)
  set_source_files_properties(./src/UnityHooks.def PROPERTIES HEADER_FILE_ONLY TRUE)
  set_target_properties(libProjectEskyLLAPIZED PROPERTIES COMPILE_DEFINITIONS BUILDER_STATIC_DEFINE)
  target_link_libraries(libProjectEskyLLAPIZED ${DEPENDENCIES} ${ZED_LIBS} ${D3DX11_LIBRARY} ${OpenCV_LIBS} "-static")

else()  # Linux
  # Error message if not on Linux
  message(FATAL_ERROR "This project is only supported on Windows")
endif()

if(WIN32)
  message(STATUS "Adding Post build script to copy realsense2.dll to project's binary folder")
  message(STATUS "Will try to copy from ${DLL_DIR} to ${PROJECT_BINARY_OUTPUT_PATH}")
  add_custom_command(
    TARGET libProjectEskyLLAPIIntel POST_BUILD     # Adds a post-build event to rs-getting-started
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
    "${DLL_DIR}/realsense2.dll"                    # <--this is in-file
    "\${PROJECT_BINARY_OUTPUT_PATH}")                 # <--this is out-file path
endif()
