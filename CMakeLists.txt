project (Assignment2)
cmake_minimum_required(VERSION 2.8)

# configure OpenCV
find_package(OpenCV REQUIRED)

# create create individual projects
add_executable(Assignment2 Assignment2.cpp)
target_link_libraries(Assignment2 ${OpenCV_LIBS})


