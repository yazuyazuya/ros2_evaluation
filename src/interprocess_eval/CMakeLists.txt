cmake_minimum_required(VERSION 3.5)
project(interprocess_eval)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

#if(NOT WIN32)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
#endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#find_package(catkin REQUIRED COMPONENTS roscpp std_msgs)
find_package(ament_cmake REQUIRED)
find_package(example_interfaces REQUIRED)
find_package(rcutils)
find_package(rclcpp REQUIRED)
find_package(rmw_implementation_cmake REQUIRED)
find_package(rmw REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(talker_interprocess src/talker_interprocess.cpp)
ament_target_dependencies(talker_interprocess example_interfaces rclcpp rcutils rmw_implementation std_msgs)

add_executable(talker_interprocess_single src/talker_interprocess_single.cpp)
ament_target_dependencies(talker_interprocess_single example_interfaces rclcpp rcutils rmw_implementation std_msgs)

add_executable(listener_interprocess src/listener_interprocess.cpp)
ament_target_dependencies(listener_interprocess example_interfaces rclcpp rcutils rmw_implementation std_msgs)

add_executable(listener_interprocess_single src/listener_interprocess_single.cpp)
ament_target_dependencies(listener_interprocess_single example_interfaces rclcpp rcutils rmw_implementation std_msgs)

install(TARGETS
  talker_interprocess
  talker_interprocess_single
  listener_interprocess
  listener_interprocess_single
  DESTINATION lib/${PROJECT_NAME}
)

install(TARGETS
  talker_interprocess
  talker_interprocess_single
  listener_interprocess
  listener_interprocess_single  
  DESTINATION bin/${PROJECT_NAME}
)

ament_package()
