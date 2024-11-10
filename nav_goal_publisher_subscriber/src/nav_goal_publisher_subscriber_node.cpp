#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <std_msgs/String.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

std::string recognizedText;  // 存储识别结果的全局变量

// 回调函数，处理识别结果
void textCallback(const std_msgs::String::ConstPtr& msg) {
    recognizedText = msg->data;
    ROS_INFO("Text recognition result: '%s'", recognizedText.c_str());
}

// 发布导航目标点并执行导航
void publishAndNavigateGoal(ros::Publisher& pub, MoveBaseClient& ac, double x, double y) {
    // 创建一个PoseStamped消息，用于发送导航目标点信息
    geometry_msgs::PoseStamped goal_msg;

    // 设置导航目标点的header信息
    goal_msg.header.seq = 0;
    goal_msg.header.stamp = ros::Time::now();
    goal_msg.header.frame_id = "map";

    // 设置导航目标点的位置信息
    goal_msg.pose.position.x = x;
    goal_msg.pose.position.y = y;
    goal_msg.pose.position.z = 0.0;

    // 设置导航目标点的朝向信息
    goal_msg.pose.orientation.x = 0.0;
    goal_msg.pose.orientation.y = 0.0;
    goal_msg.pose.orientation.z = 0.0;
    goal_msg.pose.orientation.w = 30;

    // 发布导航目标点消息
    ROS_INFO("Publishing navigation target point message");
    pub.publish(goal_msg);

    // 创建一个导航目标点消息
    move_base_msgs::MoveBaseGoal goal;

    // 设置导航目标点的位置信息
    goal.target_pose = goal_msg;

    // 发送导航目标点消息
    ROS_INFO("Sending navigation target point message to move_base");
    ac.sendGoal(goal);

    // 等待导航完成
    ac.waitForResult();

    // 输出导航结果
    if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED) {
        ROS_INFO("Navigation succeeded");
    } else {
        ROS_WARN("Navigation failed");
    }
}

int main(int argc, char** argv) {
    // 初始化ROS节点
    ros::init(argc, argv, "nav_goal_publisher_subscriber");

    // 创建一个NodeHandle
    ros::NodeHandle nh;

    // 创建一个Publisher，将导航目标点消息发布到"/move_base_simple/goal"话题上
    ros::Publisher pub = nh.advertise<geometry_msgs::PoseStamped>("/move_base_simple/goal", 10);

    // 创建一个导航行为客户端
    MoveBaseClient ac("move_base", true);

    // 等待move_base服务器启动
    ROS_INFO("Waiting for move_base to start");
    ac.waitForServer();

    // 创建一个Subscriber，订阅识别结果话题
    ros::Subscriber text_subscriber = nh.subscribe("/recognized_text", 10, textCallback);

    while (ros::ok()) {
        // 等待识别结果
        ROS_INFO("Waiting for recognition result...");

        // 处理回调函数
        ros::spinOnce();

        // 判断识别结果并执行相应的导航
            if (recognizedText == "A") {
            ROS_INFO("Recognized target 'A'");
            double x = 0.92;
            double y = 1.75;
            publishAndNavigateGoal(pub, ac, x, y);
               
            } 
         else   if (recognizedText == "B") {
            ROS_INFO("Recognized target 'B' ");
            double x = 0.16;
            double y = 0.11;
            publishAndNavigateGoal(pub, ac, x, y);
             
            } 
        else  if (recognizedText == "C") {
            ROS_INFO("Recognized target 'C'");
            double x = 4.39;
            double y = -0.60;
            publishAndNavigateGoal(pub, ac, x, y);
              
            }
        // 清空识别结果
        recognizedText = "";

        // 休眠一段时间
        ros::Duration(1.0).sleep();
    }

    return 0;
}