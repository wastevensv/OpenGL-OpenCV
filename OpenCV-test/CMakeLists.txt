cmake_minimum_required(VERSION 2.8)
project( cv-test )

set(DEP_ROOT_DIR "C:/MinGW/msys/1.0/home/William")
include_directories("${DEP_ROOT_DIR}/OpenCV/build/include")
link_directories("${DEP_ROOT_DIR}/OpenCV/build/x64/vc14/lib")
#find_package(OpenGL REQUIRED)

#set(GL_DEPENDS "C:/MinGW/msys/1.0/home/William/OpenGL/dependencies")

#set(GLFW_DIR "${GL_DEPENDS}/glfw-3.1.2/install")
#include_directories(${GLFW_DIR}/include)
#link_directories(${GLFW_DIR}/lib)
#
#set(GLEW_DIR "${GL_DEPENDS}/glew-1.13.0")
#include_directories("${GLEW_DIR}/include")
#add_definitions(-DGLEW_STATIC)
#link_directories("${GLEW_DIR}/lib")
#
#include_directories("${GL_DEPENDS}/glm")
#
#include_directories("${GL_DEPENDS}/soil/src")
#link_directories("${GL_DEPENDS}/soil/lib")
#link_directories("C:/MinGW/msys/1.0/home/William/OpenGL/lib")

add_custom_command(OUTPUT shaders PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${DEP_ROOT_DIR}/OpenCV/build/x64/vc14/bin"
        "${PROJECT_BINARY_DIR}/$<CONFIGURATION>")

add_executable(cv-test opencvtest.cpp)
target_link_libraries(cv-test opencv_world310)

add_custom_target(support DEPENDS lib)
add_dependencies(cv-test support)
