/**
 * July 2020
 * 
 * Code to make DECIM8 response to tilt with the gyro sensor.
 **/

#include "ev3dev.h"

/**
 * Simple check that hardware is available (from GitHub demo code)
 **/ 
void checkHardwareConnected(bool const condition, std::string const & errorMessage){
    if (!condition) throw std::runtime_error(errorMessage);
}

int main(){

    ev3dev::medium_motor mediumMotor;
    ev3dev::gyro_sensor gyroSensor;

    checkHardwareConnected(mediumMotor.connected(), 
        "Expected medium motor to be connected.");
    checkHardwareConnected(gyroSensor.connected(), 
        "Needs gyro sensor to be connected.");

    // Put the gyro sensor into G&A mode which captures two values:
    //      [angle, rate and direction of change]
    // Values can be negative or positive depending on which direction the tilt 
    // or change is in. From DECIM8's perspective, tilting left is positive and
    // tilting right is negative.
    gyroSensor.set_mode(gyroSensor.mode_gyro_g_a);

    // Calibrate gyro sensor; let the sensor settle first (2 seconds without 
    // change), just in case the program has been started before DECIM8 has 
    // been put down

    // Get GYRO-G&A sensor data, which contains [angle:rate of change]

    // When tilted, speak; change what is said depending on speed and tilt angle 

    // Move medium motor in relation to how quick DECIM8 is being shook

    return 0;
}
