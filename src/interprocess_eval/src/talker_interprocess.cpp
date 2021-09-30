// #include "ros/ros.h"
#include <rclcpp/rclcpp.hpp>
// #include "std_msgs/String.h"
#include <std_msgs/msg/string.hpp>

#include <sstream>
#include <fstream>
#include <iostream>		// file io
#include <string>
#include <stdio.h>

#include <time.h>		// clock
#include <unistd.h>		// clock

#include <sys/mman.h>			// mlock
#include <sched.h>				// sched

#define EVAL_NUM 120		   // evaluation number for each data size
#define PUBLISH_Hz 10
#define QoS_Policy 1 // 1 means "reliable", 0 means "best effort", 3 means "history"

/*
static const rmw_qos_profile_t rmw_qos_profile_reliable = {
  RMW_QOS_POLICY_KEEP_ALL_HISTORY,
  100,
  RMW_QOS_POLICY_RELIABLE,
  RMW_QOS_POLICY_TRANSIENT_LOCAL_DURABILITY
};

static const rmw_qos_profile_t rmw_qos_profile_best_effort = {
  RMW_QOS_POLICY_KEEP_LAST_HISTORY,
  1,
  RMW_QOS_POLICY_BEST_EFFORT,
  RMW_QOS_POLICY_VOLATILE_DURABILITY
};

static const rmw_qos_profile_t rmw_qos_profile_history = {
  RMW_QOS_POLICY_KEEP_LAST_HISTORY,
  100,							// depth option for HISTORY
  RMW_QOS_POLICY_RELIABLE,
  RMW_QOS_POLICY_TRANSIENT_LOCAL_DURABILITY
};
*/

static const rmw_qos_profile_t rmw_qos_profile_reliable = {
  RMW_QOS_POLICY_HISTORY_KEEP_ALL,
  100,
  RMW_QOS_POLICY_RELIABILITY_RELIABLE,
  RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL
};

static const rmw_qos_profile_t rmw_qos_profile_best_effort = {
  RMW_QOS_POLICY_HISTORY_KEEP_LAST,
  1,
  RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT,
  RMW_QOS_POLICY_DURABILITY_VOLATILE
};

static const rmw_qos_profile_t rmw_qos_profile_history = {
  RMW_QOS_POLICY_HISTORY_KEEP_LAST,
  100,                                                  // depth option for HISTORY
  RMW_QOS_POLICY_RELIABILITY_RELIABLE,
  RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL
};


struct timespec tp1;
int i, count = -1;	// count is current evaluation number (< EVAL_NUM)
double publish_time[EVAL_NUM];

std::string s, bytedata;

FILE *fp;						// for file io

std::string read_datafile(std::string message_filename){

  std::ifstream ifs(message_filename.c_str());
  if(ifs.fail()) {
	std::cerr << "data_*byte.txt do not exist.\n";
	exit(0);
  }

  std::string bytedata;
  getline(ifs, bytedata);

  return bytedata;
}

int eval_ros2(std::string message_filename, std::string output_filename, rclcpp::Publisher<std_msgs::msg::String>::SharedPtr chatter_pub){
  if( -1 < count ){

	auto msg = std::make_shared<std_msgs::msg::String>();
	std::stringstream ss;
  
	ss << count;		
	s = ss.str() + bytedata;
	msg->data = s;
  
	if(clock_gettime(CLOCK_REALTIME,&tp1) < 0){
	  perror("clock_gettime begin");
	  return 0;
	}
	publish_time[count] = (double)tp1.tv_sec + (double)tp1.tv_nsec/ (double)1000000000L;

//	printf("%18.9lf\n",publish_time[count]);
//	printf("publish_time[%2d]:\t%18.9lf\n", count, publish_time[count]);
//	printf("I say: [%s]\n", msg->data.c_str());
//	printf("Time Span:\t%ld.%09ld\n", tp1.tv_sec, tp1.tv_nsec);

	// chatter_pub.publish(msg);
	chatter_pub->publish(msg); // publish message
  
  }
  else if(count == -1){
	 
	bytedata = read_datafile(message_filename.c_str());
  
  }
  
  if( count >= EVAL_NUM){
        
	if((fp = fopen(output_filename.c_str(), "w")) != NULL){
	  for(i=0; i<EVAL_NUM; i++){

		if(fprintf(fp, "%18.9lf\n", publish_time[i]) < 0){
		  printf("error : can't output publish_time.txt");
		  break;
		}
	  }
	  fclose(fp);
	}else{
	  printf("error : can't output publish_time.txt");	
	}
	
        printf("  output data\n");
	count = -2;					// initilize for nexy date size
	
  }
  
  count++;
  
  return 0;

}

// int main(int argc, char **argv)
int main(int argc, char * argv[])
{
  mlockall(MCL_FUTURE);		// lock all cached memory into RAM and prevent future dynamic memory allocations
  
  usleep(1000);
  sched_param  pri = {94}; 
  if (sched_setscheduler(0, SCHED_FIFO, &pri) == -1) { // set FIFO scheduler
  	perror("sched_setattr");
  	exit(EXIT_FAILURE);
  }
  
  // ros::init(argc, argv, "talker");
  rclcpp::init(argc, argv);
  
//  auto node = rclcpp::node::Node::make_shared("talker");
  // auto node = std::make_shared<rclcpp::node::Node>("talker"); // test
  auto node = std::make_shared<rclcpp::Node>("talker"); // test
  // std::shared_ptr<rclcpp::node::Node> node(new rclcpp::node::Node("talker")); // test
  
  // QoS settings
  rmw_qos_profile_t custom_qos_profile;
  if( QoS_Policy == 1){
	custom_qos_profile = rmw_qos_profile_reliable;
  }
  else if( QoS_Policy == 2 ){
	custom_qos_profile = rmw_qos_profile_best_effort;
  }
  else if( QoS_Policy == 3){
	custom_qos_profile = rmw_qos_profile_history;
  }
  
  // ros::Publisher chatter_pub = n.advertise<std_msgs::String>("chatter", 1000);
  auto chatter_pub = node->create_publisher<std_msgs::msg::String>("chatter", custom_qos_profile);
  
  // ros::Rate loop_rate(10);
  rclcpp::WallRate loop_rate(PUBLISH_Hz);
  
  printf("start evaluation 256byte \n");
  // while (ros::ok())
  while (rclcpp::ok()) {
    eval_ros2("/home/b1014084/ros2_evaluation/evaluation/byte_data/data_256byte.txt", "/home/b1014084/ros2_evaluation/evaluation/publish_time/publish_time_256byte.txt", chatter_pub);
    if(count == -1){
      printf("end this data size evaluation \n");
      break;
    }
    // ros::spinOnce();
    rclcpp::spin_some(node);
    // loop_rate.sleep();
    loop_rate.sleep();
  }  

  usleep(5000000);

  printf("start evaluation 512byte \n");
  rclcpp::spin_some(node);
  while (rclcpp::ok()) {
    eval_ros2("/home/b1014084/ros2_evaluation/evaluation/byte_data/data_512byte.txt", "/home/b1014084/ros2_evaluation/evaluation/publish_time/publish_time_512byte.txt", chatter_pub);
    if(count == -1){
      printf("end this data size evaluation \n");
      break;
    }
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }
  
  usleep(5000000);

  printf("start evaluation 1Kbyte \n");
  while (rclcpp::ok()) {
    eval_ros2("/home/b1014084/ros2_evaluation/evaluation/byte_data/data_1Kbyte.txt", "/home/b1014084/ros2_evaluation/evaluation/publish_time/publish_time_1Kbyte.txt", chatter_pub);
    if(count == -1){
      printf("end this data size evaluation \n");
      break;
    }
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  printf("start evaluation 2Kbyte \n");
  while (rclcpp::ok()) {
    eval_ros2("/home/b1014084/ros2_evaluation/evaluation/byte_data/data_2Kbyte.txt", "/home/b1014084/ros2_evaluation/evaluation/publish_time/publish_time_2Kbyte.txt", chatter_pub);
    if(count == -1){
      printf("end this data size evaluation \n");
      break;
    }
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  usleep(5000000);
  
  printf("start evaluation 4Kbyte \n");
  while (rclcpp::ok()) {
    eval_ros2("/home/b1014084/ros2_evaluation/evaluation/byte_data/data_4Kbyte.txt", "/home/b1014084/ros2_evaluation/evaluation/publish_time/publish_time_4Kbyte.txt", chatter_pub);
    if(count == -1){
      printf("end this data size evaluation \n");
      break;
    }
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  usleep(5000000);

  printf("start evaluation 8Kbyte \n");
  while (rclcpp::ok()) {
    eval_ros2("/home/b1014084/ros2_evaluation/evaluation/byte_data/data_8Kbyte.txt", "/home/b1014084/ros2_evaluation/evaluation/publish_time/publish_time_8Kbyte.txt", chatter_pub);
    if(count == -1){
      printf("end this data size evaluation \n");
      break;
    }
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  usleep(5000000);

  printf("start evaluation 16Kbyte \n");
  while (rclcpp::ok()) {
    eval_ros2("/home/b1014084/ros2_evaluation/evaluation/byte_data/data_16Kbyte.txt", "/home/b1014084/ros2_evaluation/evaluation/publish_time/publish_time_16Kbyte.txt", chatter_pub);
    if(count == -1){
      printf("end this data size evaluation \n");
      break;
    }
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  usleep(5000000);
  
  printf("start evaluation 32Kbyte \n");
  while (rclcpp::ok()) {
    eval_ros2("/home/b1014084/ros2_evaluation/evaluation/byte_data/data_32Kbyte.txt", "/home/b1014084/ros2_evaluation/evaluation/publish_time/publish_time_32Kbyte.txt", chatter_pub);
    if(count == -1){
      printf("end this data size evaluation \n");
      break;
    }
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  usleep(5000000);
  
  printf("start evaluation 64Kbyte \n");
  while (rclcpp::ok()) {
    eval_ros2("/home/b1014084/ros2_evaluation/evaluation/byte_data/data_64Kbyte.txt", "/home/b1014084/ros2_evaluation/evaluation/publish_time/publish_time_64Kbyte.txt", chatter_pub);
    if(count == -1){
      printf("end this data size evaluation \n");
      break;
    }
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  usleep(5000000);

  printf("start evaluation 128Kbyte \n");
  while (rclcpp::ok()) {
    eval_ros2("/home/b1014084/ros2_evaluation/evaluation/byte_data/data_128Kbyte.txt", "/home/b1014084/ros2_evaluation/evaluation/publish_time/publish_time_128Kbyte.txt", chatter_pub);
    if(count == -1){
      printf("end this data size evaluation \n");
      break;
    }
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  usleep(5000000);

  printf("start evaluation 256Kbyte \n");
  while (rclcpp::ok()) {
    eval_ros2("/home/b1014084/ros2_evaluation/evaluation/byte_data/data_256Kbyte.txt", "/home/b1014084/ros2_evaluation/evaluation/publish_time/publish_time_256Kbyte.txt", chatter_pub);
    if(count == -1){
      printf("end this data size evaluation \n");
      break;
    }
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  usleep(5000000);

  printf("start evaluation 512Kbyte \n");
  while (rclcpp::ok()) {
    eval_ros2("/home/b1014084/ros2_evaluation/evaluation/byte_data/data_512Kbyte.txt", "/home/b1014084/ros2_evaluation/evaluation/publish_time/publish_time_512Kbyte.txt", chatter_pub);
    if(count == -1){
      printf("end this data size evaluation \n");
      break;
    }
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  usleep(5000000);
  
  printf("start evaluation 1Mbyte \n");
  while (rclcpp::ok()) {
    eval_ros2("/home/b1014084/ros2_evaluation/evaluation/byte_data/data_1Mbyte.txt", "/home/b1014084/ros2_evaluation/evaluation/publish_time/publish_time_1Mbyte.txt", chatter_pub);
    if(count == -1){
      printf("end this data size evaluation \n");
      break;
    }
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }
  
  usleep(5000000);
  
  printf("start evaluation 2Mbyte \n");
  while (rclcpp::ok()) {
    eval_ros2("/home/b1014084/ros2_evaluation/evaluation/byte_data/data_2Mbyte.txt", "/home/b1014084/ros2_evaluation/evaluation/publish_time/publish_time_2Mbyte.txt", chatter_pub);
    if(count == -1){
      printf("end this data size evaluation \n");
      break;
    }
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  usleep(10000000);
  
  printf("start evaluation 4Mbyte \n");
  while (rclcpp::ok()) {
    eval_ros2("/home/b1014084/ros2_evaluation/evaluation/byte_data/data_4Mbyte.txt", "/home/b1014084/ros2_evaluation/evaluation/publish_time/publish_time_4Mbyte.txt", chatter_pub);
    if(count == -1){
      printf("end this data size evaluation \n");
      break;
    }
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  // followthrough transactions
  count = 0;
  while (rclcpp::ok()) {
    auto msg = std::make_shared<std_msgs::msg::String>();	
    std::stringstream ss;
    ss << "end" << count;
    msg->data = ss.str();
    chatter_pub->publish(msg);
    if(count++ == 100){
      printf("---end evaluation---\n");
      break;
    }
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  return 0;
}

