cmake_minimum_required(VERSION 3.5)

project(interprocess_remote_eval)

if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
endif()

find_package(ament_cmake REQUIRED)
find_package(example_interfaces REQUIRED)
find_package(rcutils)
find_package(rclcpp REQUIRED)
find_package(rmw_implementation_cmake REQUIRED)
find_package(rmw REQUIRED)
find_package(std_msgs REQUIRED)


add_executable(talker_client_interprocess src/talker_client_interprocess.cpp)
ament_target_dependencies(talker_client_interprocess example_interfaces rclcpp rcutils rmw_implementation std_msgs)

add_executable(listener_server_interprocess src/listener_server_interprocess.cpp)
ament_target_dependencies(listener_server_interprocess example_interfaces rclcpp rcutils rmw_implementation std_msgs)

install(TARGETS
  talker_client_interprocess
  listener_server_interprocess
  DESTINATION lib/${PROJECT_NAME}
)

install(TARGETS
  talker_client_interprocess
  listener_server_interprocess
  DESTINATION bin/${PROJECT_NAME}
)

ament_package()
