cmake_minimum_required(VERSION 2.8)
project( squares )

set(DEP_ROOT_DIR "C:/MinGW/msys/1.0/home/William/dependencies")

set(OpenCV_DIR "${DEP_ROOT_DIR}/OpenCV/build")

find_package( OpenCV REQUIRED )

link_directories("${DEP_ROOT_DIR}/lib")
include_directories("${DEP_ROOT_DIR}/include")

configure_file("${DEP_ROOT_DIR}/bin/opencv_core310d.dll" "Debug/opencv_core310d.dll" COPYONLY)
configure_file("${DEP_ROOT_DIR}/bin/opencv_imgproc310d.dll" "Debug/opencv_imgproc310d.dll" COPYONLY)
configure_file("${DEP_ROOT_DIR}/bin/opencv_imgcodecs310d.dll" "Debug/opencv_imgcodecs310d.dll" COPYONLY)
configure_file("${DEP_ROOT_DIR}/bin/opencv_highgui310d.dll" "Debug/opencv_highgui310d.dll" COPYONLY)
configure_file("${DEP_ROOT_DIR}/bin/opencv_videoio310d.dll" "Debug/opencv_videoio310d.dll" COPYONLY)
configure_file("${DEP_ROOT_DIR}/bin/opencv_video310d.dll" "Debug/opencv_video310d.dll" COPYONLY)

add_executable(squares squares.cpp)
target_link_libraries(squares ${OpenCV_LIBS})
