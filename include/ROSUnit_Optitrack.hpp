#pragma once
#include "ros/ros.h"
#include <geometry_msgs/PoseStamped.h>
#include "common_srv/ROSUnit.hpp"
#include "common_srv/Vector3D.hpp"
#include "OptitrackMessage.hpp"

class ROSUnit_Optitrack : public ROSUnit{

    private:
        static ROSUnit_Optitrack* _instance_ptr;
        ros::Subscriber _sub_attitude;
        static OptitrackMessage optitrack_msg; 
        static void callbackOptitrack(const geometry_msgs::PoseStamped& msg);
        void receiveMsgData(DataMessage* t_msg);  
        
    public:
        
        ROSUnit_Optitrack(ros::NodeHandle&);
        ~ROSUnit_Optitrack();

};