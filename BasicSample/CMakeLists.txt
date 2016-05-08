cmake_minimum_required(VERSION 2.8)
project( BasicSample )

find_package(OpenGL REQUIRED)

set(GL_DEPENDS "C:/MinGW/msys/1.0/home/William/OpenGL/dependencies")

set(GLFW_DIR "${GL_DEPENDS}/glfw-3.1.2/install")
include_directories(${GLFW_DIR}/include)
link_directories(${GLFW_DIR}/lib)

set(GLEW_DIR "${GL_DEPENDS}/glew-1.13.0")
include_directories(${GLEW_DIR}/include)
add_definitions(-DGLEW_STATIC)
link_directories(${GLEW_DIR}/lib)

include_directories("${GL_DEPENDS}/glm")

include_directories("${GL_DEPENDS}/soil/src")
link_directories(${MY_DEPENDS}/soil/lib)


add_executable( BasicSample TestMain.cpp )
target_link_libraries(BasicSample ${OPENGL_LIBRARY} glfw3 glew32s SOIL)
