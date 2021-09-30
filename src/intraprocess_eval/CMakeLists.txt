cmake_minimum_required(VERSION 3.5)

project(intraprocess_eval)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rmw REQUIRED)
find_package(rmw_implementation_cmake REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)

include_directories(include)

  add_executable(chat_intraprocess
    src/chat_intraprocess.cpp)
  ament_target_dependencies(chat_intraprocess
    "rclcpp"
    "std_msgs")

  install(TARGETS
    chat_intraprocess
    DESTINATION lib/${PROJECT_NAME})

ament_package()
