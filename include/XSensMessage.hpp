#pragma once
#include "DataMessage.hpp"
#include "Vector3D.hpp"
#include "Quaternion.hpp"

class XSensMessage : public DataMessage{


private:
    Vector3D<float> _angular_velocity, _orientation_euler;
    Quaternion _orientation; 
    msg_type _type;

public:
    msg_type getType();
    const int getSize();
    DataMessage* Clone(){ return new XSensMessage(*this); }
    Vector3D<float> getAngularVelocity();
    Vector3D<float> getOrientationEuler();
    Quaternion getOrientation();
    void setXSensMessage(Vector3D<float>, Quaternion);
    void setAngularVelocity(Vector3D<float>);
    void setOrientation(Quaternion);
    void setOrientationEuler(Vector3D<float>);

    XSensMessage();
    ~XSensMessage();
};