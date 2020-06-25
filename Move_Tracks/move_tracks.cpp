/**
 * May 2020
 * 
 * Move DECIM8 tracks with the IR controller.
 * 
 * Assumptions: 
 * - IR SENSOR is connected
 * - Right motor is in port A
 * - Left motor is in port B
**/

#include <chrono>
#include <thread>

#include "ev3dev.h"

// A distance of -128 returned by the IR sensor when in SEEK mode indicates
// that the beacon is not on
constexpr int BEACON_OFF = -128;
constexpr int CLOSE_TO_BEACON = 30;

constexpr int MOTOR_SPEED = 900;
enum DIRECTION {
    FORWARD = -1,
    BACKWARD = 1
};

// The duty cycles (the percentage of effort of the motor I believe) for the 
// large motors.  Valid values are between -100 to 100 with negative 
// values causing the motor to run in reverse.
constexpr int MOTOR_DUTY_FORWARD =  100 * DIRECTION::FORWARD;
constexpr int MOTOR_DUTY_SLOWER_FOR_TURN = 50 * DIRECTION::FORWARD;

/**
 * Simple check that hardware is available (from GitHub demo code)
 **/ 
void checkHardwareConnected(bool const condition, std::string const & errorMessage){
    if (!condition) throw std::runtime_error(errorMessage);
}

/**
 *  Function for the remote control button presses (from GitHub demo code)
 **/
template <class Motor>
std::function<void(bool)> move(Motor & motor, DIRECTION const direction){
    return [&, direction] (bool state) {

        // If button pressed
        if (state) {
            motor.set_speed_sp(MOTOR_SPEED * direction).run_forever();
        } else {
            motor.set_stop_action("brake").stop();
        }
    };
}

/**
 *  Function to follow the remote control beacon
 **/
template <class Motor, class IRSensor>
std::function<void(bool)> follow(
    IRSensor & irSensor,
    Motor    & leftMotor, 
    Motor    & rightMotor){
        
    return [&](bool state) {

        // If the beacon is on
        if (state){

            // The IR sensor in SEEK mode returns an array of 8 values when 
            // processed; values are a pair of angle and distance for each 
            // beacon channel.
            //
            // The first value is the angle: 
            //      is an int
            //      a negative number to the left 
            //      a positive number to the right.
            //      Range: -25 to 25
            //
            // The second value is distance: 
            //      is an int
            //      for decimate, less than 40 is practically ontop of the beacon.  
            //      Range: 0 to 100
            //      -128 signifies the beacon isn't on

            // Set the IR sensor to SEEK mode
            if (irSensor.mode() != irSensor.mode_ir_seek){

                irSensor.set_mode(irSensor.mode_ir_seek);
            }

            // We don't want DECIM8 to continually speak when at the beacon.
            // Therefore, capture whether DECIM8 has already spoken.
            bool spokeAtBeacon = false;

            // The first sensor reading is usual -128 irrespective of where the
            // beacon is in relation to the IR sensor.  Therefore, do one 
            // manditory loop to "flush" that value.
            do {

                // Retrieve both angle and distance on channel 1
                int angle = irSensor.value(0);
                int distance = irSensor.value(1);

                if (distance > CLOSE_TO_BEACON){
                    spokeAtBeacon = false;

                    if (angle < 0) {            // Left
                        leftMotor.set_duty_cycle_sp(MOTOR_DUTY_SLOWER_FOR_TURN);
                        rightMotor.set_duty_cycle_sp(MOTOR_DUTY_FORWARD);
                    } else if ( angle > 0){     // Right
                        leftMotor.set_duty_cycle_sp(MOTOR_DUTY_FORWARD);
                        rightMotor.set_duty_cycle_sp(MOTOR_DUTY_SLOWER_FOR_TURN);
                    } else {                    // Straight ahead
                        leftMotor.set_duty_cycle_sp(MOTOR_DUTY_FORWARD);
                        rightMotor.set_duty_cycle_sp(MOTOR_DUTY_FORWARD);
                    }

                    leftMotor.run_direct();
                    rightMotor.run_direct();

                } else {

                    leftMotor.set_stop_action("brake").stop();
                    rightMotor.set_stop_action("brake").stop();

                    if (!spokeAtBeacon && distance > 0 && 
                        distance < CLOSE_TO_BEACON){

                        ev3dev::sound::speak("That's close enough!", true);
                        spokeAtBeacon = true;
                    }
                }

                // To combat some sensor flucutations, wait a little.
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

            } while (irSensor.value(1) != BEACON_OFF);

            // When the beacon is turned off, we want to stop DECIM8 and return
            // the IR sensor mode back to remote control
            leftMotor.set_stop_action("brake").stop();
            rightMotor.set_stop_action("brake").stop();

            if (irSensor.mode() != irSensor.mode_ir_remote){

                irSensor.set_mode(irSensor.mode_ir_remote);
            }
        } 
    };    
}

/**
 * The majority of this is taken from the ev3dev-lang-cpp GitHub demo code.
 * The beacon behaviour is my own though.
 **/
int main(){

    // Links to all the necessary hardware
    ev3dev::large_motor rightMotor(ev3dev::OUTPUT_A);
    ev3dev::large_motor leftMotor(ev3dev::OUTPUT_B);
    ev3dev::infrared_sensor irSensor;
    ev3dev::remote_control remoteController;

    // Make sure the IR SENSOR and motors are connected
    checkHardwareConnected(rightMotor.connected(), 
        "Right motor not connected to port A.");
    checkHardwareConnected(leftMotor.connected(), 
        "Left motor not connected to port B.");
    checkHardwareConnected(irSensor.connected(), "IR sensor not connected.");
    checkHardwareConnected(remoteController.channel() == 1,
        "IR remote needs to be on channel 1");

    // Program what the remote control will do when buttons are pressed
    remoteController.on_red_up = move(leftMotor, DIRECTION::FORWARD);
    remoteController.on_red_down = move(leftMotor, DIRECTION::BACKWARD);
    remoteController.on_blue_up = move(rightMotor, DIRECTION::FORWARD);
    remoteController.on_blue_down = move(rightMotor, DIRECTION::BACKWARD);

    remoteController.on_beacon = follow(irSensor, leftMotor, rightMotor);
    
    ev3dev::sound::speak("On the move!");

    // Press the back button on the Brick to exit
    while (!ev3dev::button::back.pressed()){

        // Need to call the process command to get the remote button state
        // and call the assigned button functions
        remoteController.process();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}

