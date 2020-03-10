#pragma once
#include "MsgEmitter.hpp"
#include "MsgReceiver.hpp"
#include "cmath"
#include "RotationMatrix3by3.hpp"
#include <atomic>
#include "Vector3DMessage.hpp"
#include "FloatMsg.hpp"
#include "ControlSystem.hpp"

class Transform_InertialToBody : public MsgEmitter, public MsgReceiver {

private:
    static std::atomic<float>  _inertial_command_x;
    static std::atomic<float>  _inertial_command_y;
    static std::atomic<float>  _inertial_command_z;
    bool _opti_x_received = false, _opti_y_received = false, _current_yaw_received = false;
    UpdatePoseMessage _body_xy;
    RotationMatrix3by3 _rotation_matrix;
    ControlSystemMessage m_output_msg;
    control_system _source;

public:

    void receiveMsgData(DataMessage*);
    void receiveMsgData(DataMessage*, int);
    Transform_InertialToBody(control_system);
    ~Transform_InertialToBody();
};