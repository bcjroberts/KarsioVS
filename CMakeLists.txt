cmake_minimum_required(VERSION 3.8)
project(KarsioVS)
set(PROJECT_NAME Karsio) #Macro the project name

set(CMAKE_CXX_STANDARD 11)

#Indicate that OpenCL is needed
find_package(OpenGL REQUIRED)

set(INCLUDE_DIRS deps/include deps/physx/Include deps/physx/PxShared/include)
include_directories(${INCLUDE_DIRS})

set(SOURCE_FILES src)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${INCLUDE_DIRS})


set_target_properties(${PROJECT_NAME} PROPERTIES  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
