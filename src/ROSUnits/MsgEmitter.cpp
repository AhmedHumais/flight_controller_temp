// Version: 3.0
// Author: P. Silva
// Date: 09 Mar 2020
// Release note: Removed broadcast channels

#include "MsgEmitter.hpp"
using namespace std;

void msg_emitter::add_callback_msg_receiver(msg_receiver* _callback_msg_receiver, int associated_publishing_channel){
    _list_of_msg_receivers.push_back(_callback_msg_receiver);
    _list_of_receivers_mask_unicast.push_back(associated_publishing_channel);
}

void msg_emitter::emit_message_unicast(DataMessage* t_msg,int t_unicast_mask){
    for (int i = 0; i < _list_of_msg_receivers.size(); ++i){
        if (t_unicast_mask==_list_of_receivers_mask_unicast[i]){
             _list_of_msg_receivers[i]->receive_msg_data(t_msg);
        }
    }
}
void msg_emitter::emit_message_unicast(DataMessage* t_msg,int t_unicast_mask,int t_channel_id){
    for (int i = 0; i < _list_of_msg_receivers.size(); ++i){
        if (t_unicast_mask==_list_of_receivers_mask_unicast[i]){
             _list_of_msg_receivers[i]->receive_msg_data(t_msg,t_channel_id);
        }
    }
}
msg_emitter::msg_emitter(){
}

void msg_emitter::setEmittingChannel(int t_ch){
    this->emitting_channel=t_ch;
}

int msg_emitter::getEmittingChannel(){
    return emitting_channel;
}