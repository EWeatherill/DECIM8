/**
 * July 2020
 * 
 * Code to make DECIM8 response to tilt with the gyro sensor.
 **/
#include <algorithm>
#include <chrono>
#include <thread>
#include <deque>

// For debugging
#include <iostream>

#include "ev3dev.h"

using std::chrono::milliseconds;

int constexpr TWO_SECONDS = 2000;
int constexpr MILSEC_INTERVAL = 100;

/**
 * Simple check that hardware is available (from GitHub demo code)
 **/ 
void checkHardwareConnected(bool const condition, std::string const & errorMessage){
    if (!condition) throw std::runtime_error(errorMessage);
}

bool checkAllElementsAreTheSame(std::deque<int> deck){

    if (std::adjacent_find(deck.begin(), deck.end(), std::not_equal_to<>()) == 
        deck.end()) 
        return true;

    return false;
}

int main(){

    ev3dev::medium_motor mediumMotor;
    ev3dev::gyro_sensor gyroSensor;

    checkHardwareConnected(mediumMotor.connected(), 
        "Expected medium motor to be connected.");
    checkHardwareConnected(gyroSensor.connected(), 
        "Needs gyro sensor to be connected.");

    // Keep a thread open to check if the back button is pressed
    bool exit = false;
    std::thread exitThread( [&] () {
        if (ev3dev::button::back.pressed()) exit = true;
    } );

    // Initial set the gyro sensor to just capture angle
    gyroSensor.set_mode(gyroSensor.mode_gyro_ang);

    // Before calibrating the sensor let the robot settle first (2 seconds 
    // without an angle change), just in case the program has been started 
    // before DECIM8 has been put down
    std::deque<int> settleReadings;
    do {

        settleReadings.clear();

        for (auto i = TWO_SECONDS; i > 0; i -= MILSEC_INTERVAL){
            settleReadings.push_back(gyroSensor.value());
            std::this_thread::sleep_for(milliseconds(MILSEC_INTERVAL));
        }
                
    } while (!exit && !checkAllElementsAreTheSame(settleReadings));

    if (!exit) {

        // Setting the gyro sensor to cal(ibrate) mode immediately calibrates it
        // to be 0 degrees.
        gyroSensor.set_mode(gyroSensor.mode_gyro_cal);

        // Put the gyro sensor into G&A mode which captures two values:
        //      [angle, rotational speed]
        // Values can be negative or positive depending on which direction the tilt 
        // or change is in. From DECIM8's perspective, tilting left is positive and
        // tilting right is negative.
        gyroSensor.set_mode(gyroSensor.mode_gyro_g_a);
        ev3dev::sound::speak("Ready to go!");
    }

    // Get GYRO-G&A sensor data, which contains [angle:rate of change]

    // When tilted, speak; change what is said depending on speed and tilt angle 

    // Move medium motor in relation to how quick DECIM8 is being shook

    
    exitThread.join();
    return 0;
}
