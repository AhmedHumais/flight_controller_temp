#include <iostream>
#include <vector>
#include "../include/UM8E.hpp"
#include "../include/OptiTrack.hpp"
#include "../include/ROSUnit_Optitrack.hpp"
#include "../include/PIDController.hpp"
#include "../include/ControlSystem.hpp"
#include "../include/PID_values.hpp"
#include "../include/ProcessVariableReference.hpp"
#include "../include/ActuationSystem.hpp"
#include "../include/looper.hpp"
#include "../include/std_logger.hpp"
#include "../include/HexaActuationSystem.hpp"
#include "../include/esc_motor.hpp"
#include "../include/NavioMPU9250Sensor.hpp"
#include "../include/AccGyroAttitudeObserver.hpp"
#include "../include/GyroMagHeadingObserver.hpp"
#include "../include/ComplementaryFilter.hpp"
#include "../include/X_UserReference.hpp"
#include "../include/Y_UserReference.hpp"
#include "../include/Z_UserReference.hpp"
#include "../include/Yaw_UserReference.hpp"
#include "../include/ROSUnit_Arm.hpp"
#include "../include/ROSUnit_UpdateReference.hpp"
#include "../include/ROSUnit_UpdateController.hpp"
#include "../include/ROSUnit_ResetController.hpp"
#include "../include/ROSUnit_BroadcastData.hpp"
#include "../include/PID_ParametersMsg.hpp"
#include "../include/ROSUnit_SwitchBlock.hpp"
#include "../include/MRFTController.hpp"
#include "../include/MRFT_values.hpp"
#include "../include/MRFT_ParametersMsg.hpp"
#include "../include/ControllerMessage.hpp"

void performCalibration(NAVIOMPU9250_sensor*);

int main(int argc, char** argv) {
    // std::cout << "Hello Easy C++ project!" << std::endl;
    //TODO separate files on specific folders
    //TODO ROSUnit to switch blocks

    ros::init(argc, argv, "testing_node");

    ros::NodeHandle nh;
    ros::Rate rate(300);

    ROSUnit* myROSOptitrack = new ROSUnit_Optitrack(nh);
    ROSUnit* myROSUpdateReference = new ROSUnit_UpdateReference(nh);
    ROSUnit* myROSArm = new ROSUnit_Arm(nh);
    ROSUnit* myROSUpdateController = new ROSUnit_UpdateController(nh);
    ROSUnit* myROSResetController = new ROSUnit_ResetController(nh);
    ROSUnit* myROSBroadcastData = new ROSUnit_BroadcastData(nh);
    ROSUnit* myROSSwitchBlock = new ROSUnit_SwitchBlock(nh);

    //*****************************LOGGER**********************************
    Logger::assignLogger(new StdLogger());

    //***********************ADDING SENSORS********************************
    //  NAVIOMPU9250_sensor* myIMU = new NAVIOMPU9250_sensor();
    //  myIMU->setSettings(ACCELEROMETER, FSR, 16);
    //  myIMU->setSettings(GYROSCOPE, FSR, 2000);
    //  myIMU->setSettings(MAGNETOMETER, FSR, 16);

    //***********************SETTING PROVIDERS**********************************
    MotionCapture* myOptitrackSystem = new OptiTrack();
    X_PVProvider* myXPV = (X_PVProvider*)myOptitrackSystem;
    Y_PVProvider* myYPV = (Y_PVProvider*)myOptitrackSystem;
    Z_PVProvider* myZPV = (Z_PVProvider*)myOptitrackSystem;
    Roll_PVProvider* myRollPV = (Roll_PVProvider*)myOptitrackSystem;
    Pitch_PVProvider* myPitchPV = (Pitch_PVProvider*)myOptitrackSystem;
    Yaw_PVProvider* myYawPV = (Yaw_PVProvider*)myOptitrackSystem;
    
    // AccGyroAttitudeObserver myAttObserver((BodyAccProvider*) myIMU->getAcc(), 
    //                                       (BodyRateProvider*) myIMU->getGyro(),
    //                                       block_frequency::hhz200);

    
    
    //  ComplementaryFilter filter1, filter2, filter3;
    // //TODO second argument should be dt of IMU sampling rate
    //  ComplementaryFilterSettings settings(false, 0.005, 0.995);

    //  myAttObserver.setFilterType(&filter1, &filter2);
    //  myAttObserver.updateSettings(&settings, 0.1);

    //  Roll_PVProvider* myRollPV = (Roll_PVProvider*) &myAttObserver;
    //  Pitch_PVProvider* myPitchPV = (Pitch_PVProvider*) &myAttObserver;

    myROSOptitrack->add_callback_msg_receiver((msg_receiver*)myOptitrackSystem);
    

    //**************************SETTING BLOCKS**********************************

    Block* PID_x = new PIDController(block_id::PID_X);
    Block* PID_pitch = new PIDController(block_id::PID_PITCH);
    Block* PV_Ref_x = new ProcessVariableReference(block_id::REF_X);
    Block* PV_Ref_pitch = new ProcessVariableReference(block_id::REF_PITCH);
    Block* PID_y = new PIDController(block_id::PID_Y);
    Block* PID_roll = new PIDController(block_id::PID_ROLL);
    Block* PV_Ref_y = new ProcessVariableReference(block_id::REF_Y);
    Block* PV_Ref_roll = new ProcessVariableReference(block_id::REF_ROLL);
    Block* PID_z = new PIDController(block_id::PID_Z);
    Block* PID_yaw = new PIDController(block_id::PID_YAW);
    Block* PV_Ref_z = new ProcessVariableReference(block_id::REF_Z);
    Block* PV_Ref_yaw = new ProcessVariableReference(block_id::REF_YAW);

    Block* MRFT_x = new MRFTController(block_id::MRFT_X);
    Block* MRFT_y = new MRFTController(block_id::MRFT_Y);
    Block* MRFT_z = new MRFTController(block_id::MRFT_Z);
    Block* MRFT_roll = new MRFTController(block_id::MRFT_ROLL);
    Block* MRFT_pitch = new MRFTController(block_id::MRFT_PITCH);
    Block* MRFT_yaw = new MRFTController(block_id::MRFT_YAW);

    //***********************SETTING CONTROL SYSTEMS***************************

    //TODO Expose switcher to the main, add blocks to the switcher, then make connections between switcher, then add them to the Control System
    ControlSystem* X_ControlSystem = new ControlSystem(control_system::x, myXPV, block_frequency::hz100);
    X_ControlSystem->addBlock(PID_x);
    X_ControlSystem->addBlock(MRFT_x);
    X_ControlSystem->addBlock(PV_Ref_x);

    ControlSystem* Pitch_ControlSystem = new ControlSystem(control_system::pitch, myPitchPV, block_frequency::hz200);
    Pitch_ControlSystem->addBlock(PID_pitch);
    Pitch_ControlSystem->addBlock(MRFT_pitch);
    Pitch_ControlSystem->addBlock(PV_Ref_pitch);

    ControlSystem* Y_ControlSystem = new ControlSystem(control_system::y, myYPV, block_frequency::hz100);
    Y_ControlSystem->addBlock(PID_y);
    Y_ControlSystem->addBlock(MRFT_y);
    Y_ControlSystem->addBlock(PV_Ref_y);

    ControlSystem* Roll_ControlSystem = new ControlSystem(control_system::roll, myRollPV, block_frequency::hz200);
    Roll_ControlSystem->addBlock(PID_roll);
    Roll_ControlSystem->addBlock(MRFT_roll);
    Roll_ControlSystem->addBlock(PV_Ref_roll);

    ControlSystem* Z_ControlSystem = new ControlSystem(control_system::z, myZPV, block_frequency::hz100);
    Z_ControlSystem->addBlock(PID_z);
    Z_ControlSystem->addBlock(MRFT_z);
    Z_ControlSystem->addBlock(PV_Ref_z);

    //Yaw on Optitrack 100Hz
    ControlSystem* Yaw_ControlSystem = new ControlSystem(control_system::yaw, myYawPV, block_frequency::hz100);
    Yaw_ControlSystem->addBlock(PID_yaw);
    Yaw_ControlSystem->addBlock(MRFT_yaw);
    Yaw_ControlSystem->addBlock(PV_Ref_yaw);

    //*********************SETTING ACTUATION SYSTEMS************************

    int freq = 50;
    Actuator* M1 = new ESCMotor(0, freq);
    Actuator* M2 = new ESCMotor(1, freq);
    Actuator* M3 = new ESCMotor(2, freq);
    Actuator* M4 = new ESCMotor(3, freq);
    Actuator* M5 = new ESCMotor(4, freq);
    Actuator* M6 = new ESCMotor(5, freq);

    std::vector<Actuator*> actuators{M1, M2, M3, M4, M5, M6};

    ActuationSystem* myActuationSystem = new HexaActuationSystem(actuators);
    
    //***********************SETTING FLIGHT SCENARIO INPUTS****************************
    X_UserReference* myX_UserRef = new X_UserReference();
    Y_UserReference* myY_UserRef = new Y_UserReference();
    Z_UserReference* myZ_UserRef = new Z_UserReference();
    Yaw_UserReference* myYaw_UserRef = new Yaw_UserReference();

    //Forward is negative pitch, Right is positive roll, CCW is positive yaw, Upwards is positive Z

    myROSUpdateReference->add_callback_msg_receiver((msg_receiver*)myX_UserRef);
    myROSUpdateReference->add_callback_msg_receiver((msg_receiver*)myY_UserRef);
    myROSUpdateReference->add_callback_msg_receiver((msg_receiver*)myZ_UserRef);
    myROSUpdateReference->add_callback_msg_receiver((msg_receiver*)myYaw_UserRef);

    myROSUpdateController->add_callback_msg_receiver((msg_receiver*)PID_x);
    myROSUpdateController->add_callback_msg_receiver((msg_receiver*)MRFT_x);
    myROSUpdateController->add_callback_msg_receiver((msg_receiver*)PID_y);
    myROSUpdateController->add_callback_msg_receiver((msg_receiver*)PID_z);
    myROSUpdateController->add_callback_msg_receiver((msg_receiver*)PID_roll);
    myROSUpdateController->add_callback_msg_receiver((msg_receiver*)PID_pitch);
    myROSUpdateController->add_callback_msg_receiver((msg_receiver*)PID_yaw);

    //TODO after Switchers are exposed, connect ROSUnit_SwitchBlocks with them
    myROSSwitchBlock->add_callback_msg_receiver((msg_receiver*)X_ControlSystem);
    myROSSwitchBlock->add_callback_msg_receiver((msg_receiver*)Y_ControlSystem);
    myROSSwitchBlock->add_callback_msg_receiver((msg_receiver*)Z_ControlSystem);
    myROSSwitchBlock->add_callback_msg_receiver((msg_receiver*)Roll_ControlSystem);
    myROSSwitchBlock->add_callback_msg_receiver((msg_receiver*)Pitch_ControlSystem);
    myROSSwitchBlock->add_callback_msg_receiver((msg_receiver*)Yaw_ControlSystem);

    myROSArm->add_callback_msg_receiver((msg_receiver*) myActuationSystem);

    //********************SETTING FLIGHT SCENARIO OUTPUTS***************************

    X_ControlSystem->add_callback_msg_receiver((msg_receiver*)myROSBroadcastData);
    Y_ControlSystem->add_callback_msg_receiver((msg_receiver*)myROSBroadcastData);
    Z_ControlSystem->add_callback_msg_receiver((msg_receiver*)myROSBroadcastData);
    Roll_ControlSystem->add_callback_msg_receiver((msg_receiver*)myROSBroadcastData);
    Pitch_ControlSystem->add_callback_msg_receiver((msg_receiver*)myROSBroadcastData);
    Yaw_ControlSystem->add_callback_msg_receiver((msg_receiver*)myROSBroadcastData);

    myXPV->PVProvider::add_callback_msg_receiver((msg_receiver*)myROSBroadcastData);
    myYPV->PVProvider::add_callback_msg_receiver((msg_receiver*)myROSBroadcastData);
    myZPV->PVProvider::add_callback_msg_receiver((msg_receiver*)myROSBroadcastData);
    myRollPV->PVProvider::add_callback_msg_receiver((msg_receiver*)myROSBroadcastData);
    myPitchPV->PVProvider::add_callback_msg_receiver((msg_receiver*)myROSBroadcastData);
    myYawPV->PVProvider::add_callback_msg_receiver((msg_receiver*)myROSBroadcastData);

    myActuationSystem->add_callback_msg_receiver((msg_receiver*)myROSBroadcastData);

    myXPV->PositioningProvider::add_callback_msg_receiver((msg_receiver*)myROSBroadcastData);
    myRollPV->AttitudeProvider::add_callback_msg_receiver((msg_receiver*)myROSBroadcastData);
    myYawPV->HeadingProvider::add_callback_msg_receiver((msg_receiver*)myROSBroadcastData);

    //***********************SETTING PID INITIAL VALUES*****************************

    //TODO remove this after adding to FlightScenario
    //TODO find a better way to pass dt to the controllers
    ControllerMessage ctrl_msg;
    PID_parameters pid_para_init;

    pid_para_init.id = block_id::PID_X;
    ctrl_msg.setPIDParam(pid_para_init);
    ctrl_msg.set_dt(X_ControlSystem->get_dt());
    myROSUpdateController->emit_message((DataMessage*) &ctrl_msg);

    pid_para_init.id = block_id::PID_Y;
    ctrl_msg.setPIDParam(pid_para_init);
    ctrl_msg.set_dt(Y_ControlSystem->get_dt());
    myROSUpdateController->emit_message((DataMessage*) &ctrl_msg);

    pid_para_init.id = block_id::PID_Z;
    ctrl_msg.setPIDParam(pid_para_init);
    ctrl_msg.set_dt(Z_ControlSystem->get_dt());
    myROSUpdateController->emit_message((DataMessage*) &ctrl_msg);

    pid_para_init.id = block_id::PID_ROLL;
    ctrl_msg.setPIDParam(pid_para_init);
    ctrl_msg.set_dt(Roll_ControlSystem->get_dt());
    myROSUpdateController->emit_message((DataMessage*) &ctrl_msg);

    pid_para_init.id = block_id::PID_PITCH;
    ctrl_msg.setPIDParam(pid_para_init);
    ctrl_msg.set_dt(Pitch_ControlSystem->get_dt());
    myROSUpdateController->emit_message((DataMessage*) &ctrl_msg);

    pid_para_init.id = block_id::PID_YAW;
    ctrl_msg.setPIDParam(pid_para_init);
    ctrl_msg.set_dt(Yaw_ControlSystem->get_dt());
    myROSUpdateController->emit_message((DataMessage*) &ctrl_msg);

    //***********************SETTING MRFT INITIAL VALUES*****************************

    MRFT_parameters mrft_para_init;

    mrft_para_init.id = block_id::MRFT_X;
    ctrl_msg.setMRFTParam(mrft_para_init);
    ctrl_msg.set_dt(X_ControlSystem->get_dt());
    myROSUpdateController->emit_message((DataMessage*) &ctrl_msg);

    //****************************SETTING CONNECTIONS********************************
    //========                                                      =============
    //|      |----->X_Control_System----->Roll_Control_System------>|           |
    //| USER |----->Y_Control_System----->Pitch_Control_System----->| Actuation |      
    //|      |----->Z_Control_System------------------------------->|  System   |
    //|      |----->Yaw_Control_System----------------------------->|           |
    //========                                                      =============
    
    myX_UserRef->add_callback_msg_receiver((msg_receiver*)X_ControlSystem);
    myY_UserRef->add_callback_msg_receiver((msg_receiver*)Y_ControlSystem);
    myZ_UserRef->add_callback_msg_receiver((msg_receiver*)Z_ControlSystem);
    myYaw_UserRef->add_callback_msg_receiver((msg_receiver*)Yaw_ControlSystem);
    X_ControlSystem->add_callback_msg_receiver((msg_receiver*)Roll_ControlSystem);
    Roll_ControlSystem->add_callback_msg_receiver((msg_receiver*)myActuationSystem);
    Y_ControlSystem->add_callback_msg_receiver((msg_receiver*)Pitch_ControlSystem);
    Pitch_ControlSystem->add_callback_msg_receiver((msg_receiver*)myActuationSystem);
    Z_ControlSystem->add_callback_msg_receiver((msg_receiver*)myActuationSystem);
    Yaw_ControlSystem->add_callback_msg_receiver((msg_receiver*)myActuationSystem);
    
    //******************************LOOP***********************************
    //TODO  move to looper constructor
    pthread_t loop1khz_func_id, loop100hz_func_id, hwloop1khz_func_id;
    struct sched_param params;

    Looper* myLoop = new Looper();
    myLoop->addTimedBlock((TimedBlock*)X_ControlSystem);
    myLoop->addTimedBlock((TimedBlock*)Y_ControlSystem);
    myLoop->addTimedBlock((TimedBlock*)Z_ControlSystem);
    myLoop->addTimedBlock((TimedBlock*)Roll_ControlSystem);
    myLoop->addTimedBlock((TimedBlock*)Pitch_ControlSystem);
    myLoop->addTimedBlock((TimedBlock*)Yaw_ControlSystem);
    //  myLoop->addTimedBlock((TimedBlock*) &myAttObserver);

    // Creating a new thread 
    pthread_create(&loop1khz_func_id, NULL, &Looper::Loop1KHz, NULL);
    //  pthread_create(&hwloop1khz_func_id, NULL, &Looper::hardwareLoop1KHz, NULL);
    pthread_create(&loop100hz_func_id, NULL, &Looper::Loop100Hz, NULL); 

    //Setting priority
    params.sched_priority = sched_get_priority_max(SCHED_FIFO);
    int ret = pthread_setschedparam(loop1khz_func_id, SCHED_FIFO, &params);
    //  ret += pthread_setschedparam(hwloop1khz_func_id, SCHED_FIFO, &params);

    params.sched_priority = sched_get_priority_max(SCHED_FIFO) - 1;
    ret += pthread_setschedparam(loop100hz_func_id, SCHED_FIFO, &params);

    if (ret != 0) {
         // Print the error
         // change
         std::cout << "Unsuccessful in setting thread realtime prior " << ret << std::endl;
     }

     //performCalibration(myIMU);

    while(ros::ok()){

        ros::spinOnce();
        rate.sleep();
    }
    
    return 0;

}

void performCalibration(NAVIOMPU9250_sensor* t_imu){
    Timer* _calib_timer = new Timer();
    int consumed_time = 0;
    //1 second of garbage
    _calib_timer->tick();
    while(consumed_time < 1000){
        consumed_time = _calib_timer->tockMilliSeconds();
        t_imu->getAcc()->getCalibratedData();
        t_imu->getGyro()->getCalibratedData();
    }

    std::cout << "1 Second passed" << std::endl;
    
    //t_imu->getAcc()->startCalibration();
    t_imu->getGyro()->startCalibration(); 
    std::cout << "CALIBRATION STARTED..." << std::endl;

    _calib_timer->tick();
    consumed_time = 0;
    std::cout << "CALIBRATION RUNNING....................................................................." << std::endl;
    while(consumed_time < 5000){
        consumed_time = _calib_timer->tockMilliSeconds();     
        //t_imu->getAcc()->getCalibratedData();
        t_imu->getGyro()->getCalibratedData(); 
    }


    std::cout << "5 Second passed" << std::endl;
    //t_imu->getAcc()->stopCalibration();
    t_imu->getGyro()->stopCalibration();
    std::cout << "CALIBRATION ENDED" << std::endl;

    _calib_timer->tick();
    consumed_time = 0;
    while(consumed_time < 5000){
        consumed_time = _calib_timer->tockMilliSeconds();    
    }

}
