#include "HexaActuationSystem.hpp"


HexaActuationSystem::HexaActuationSystem(std::vector<Actuator*> t_actuators) : ActuationSystem(t_actuators){
    _actuators = t_actuators;
}

HexaActuationSystem::~HexaActuationSystem() {

}


void HexaActuationSystem::command(){

    for(int i = 0; i < 6; i++){
        _commands[i] = 0;
    }
    
    //Update pulse values
    for(int i = 0; i < 6; i++){
        for(int j = 0; j < 4; j++){
            _commands[i] += _geometry[i][j] * _movements[j];
        }
    }

    //_movements (PID outputs) should be between 0 and 1. Thus, we have to adjust for the range 1000 to 2000 on _commands.
    //Normalize
    for(int i = 0; i < 6; i++){
        _commands[i] = (_commands[i] * 1000) + 1000;
    }

    //Actuate with constrains
    for(int i = 0; i < 6; i++){
        _commands[i] = this->constrain(_commands[i], _escMin, _escMax);
        _actuators[i]->applyCommand(_commands[i]);
    }

    ros_msg.setActuation(&_commands[0]);
    this->emit_message((DataMessage*) &ros_msg);
}

int HexaActuationSystem::constrain(float value, int min_value, int max_value) {
    
    if(_armed){
        int min_value_armed = min_value + 150;

        if (value > max_value) {
            value = max_value;
        } else if (value < min_value_armed) {
            value = min_value_armed;
        }
    }else{
        if (value > max_value) {
            value = max_value;
        } else if (value < min_value) {
            value = min_value;
        }
    }
        
    return int(value);
}

void HexaActuationSystem::receive_msg_data(DataMessage* t_msg){

    if(t_msg->getType() == msg_type::control_system){
        ControlSystemMessage* control_system_msg = (ControlSystemMessage*)t_msg;
        if(control_system_msg->getControlSystemMsgType() == control_system_msg_type::to_system){
            
            if(_armed){
                switch (control_system_msg->getSource())
                {
                case control_system::roll:
                {
                    _movements[0] = control_system_msg->getData();
                    break;
                }
                case control_system::pitch:
                {
                    _movements[1] = control_system_msg->getData();
                    break;
                }
                case control_system::yaw:
                {
                    _movements[2] = control_system_msg->getData();
                    break;
                }
                case control_system::z:
                {
                    _movements[3] = control_system_msg->getData();
                    break;
                }
                default:
                    break;
                }
            }else{
                _movements[0] = 0.0;
                _movements[1] = 0.0;
                _movements[2] = 0.0;
                _movements[3] = 0.0;
            }
            
            this->command();
            
        }
          
    }else if(t_msg->getType() == msg_type::BOOLEAN){

        BoolMessage* bool_msg = (BoolMessage*)t_msg;
        _armed = bool_msg->getData();
    }
}