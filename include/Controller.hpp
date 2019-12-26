#pragma once
#include "Block.hpp"
#include "ControllerMessage.hpp"
#include "FloatMessage.hpp"
#include "common_types.hpp"

class Controller : public Block{

private:
    block_type _type;

public:

    block_type getType();
    virtual void switchIn(DataMessage*) = 0;
    virtual DataMessage* switchOut() = 0;
    virtual block_id getID() = 0;
    virtual controller_type getControllerType() = 0;
    virtual DataMessage* receive_msg_internal(DataMessage*) = 0;
    virtual void receive_msg_data(DataMessage* t_msg) = 0;
    
    Controller();
    ~Controller();
};