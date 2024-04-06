cmake_minimum_required(VERSION 3.7.2)
project(libProjectEskyLLAPI)

# Global configs 
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MT")

#if(NOT LINK_SHARED_ZED AND MSVC)
 #   message(FATAL_ERROR "LINK_SHARED_ZED OFF : ZED SDK static libraries not available on Windows")
#endif()



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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  set(PROJECT_BINARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/\$\(Configuration\)")
  
  find_package(ZED 4)
  link_directories(${ZED_LIBRARY_DIR}) 
  include_directories(${ZED_INCLUDE_DIRS})
  set(ZED_LIBS ${ZED_LIBRARIES} ${CUDA_CUDA_LIBRARY} ${CUDA_CUDART_LIBRARY})

  find_package(CUDA ${ZED_CUDA_VERSION} EXACT) 
  include_directories(${CUDA_INCLUDE_DIRS})

  include_directories(${ADDITIONAL_INCLUDE_DIRS})
  include_directories(./include/)

  set(CMAKE_EXE_LINKER_FLAGS "-static")

  add_library(libProjectEskyLLAPIZED SHARED
    ./src/main.cpp
    ./src/Tracker_ZED.h
    ./src/Tracker_ZED.cpp
    ./src/UnityHooks.def)
  set_source_files_properties(./src/UnityHooks.def PROPERTIES HEADER_FILE_ONLY TRUE)
  set_target_properties(libProjectEskyLLAPIZED PROPERTIES COMPILE_DEFINITIONS BUILDER_STATIC_DEFINE)
  target_link_libraries(libProjectEskyLLAPIZED ${DEPENDENCIES} ${ZED_LIBS} ${D3DX11_LIBRARY} "-static")
else()
  message(FATAL_ERROR "This project is only supported on Windows")
endif()
