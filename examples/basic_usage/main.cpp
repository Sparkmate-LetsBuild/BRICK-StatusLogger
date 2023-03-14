
namespace StatusLogger
{
    // -- EXAMPLE BRICKS LOGGER HANDLER
    const String NAME_ACCEL = "ACCEL"; // Log something to do with the accelerometer
    const String NAME_ESP32 = "ESP32"; // '' specifically with the ESP32
}

#include <StatusLogger.h>

// Using an example sensor, this really could be anything you want.
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
Adafruit_MPU6050 accel;

void()
{
    if (accel.begin())
    {
        StatusLogger::setBrickStatus(StatusLogger::NAME_ACCEL, StatusLogger::FUNCTIONALITY_PARTIAL, "Accelerometer is found and connected!");
    }
    else
    {
        StatusLogger::setBrickStatus(StatusLogger::NAME_ACCEL, StatusLogger::FUNCTIONALITY_OFFLINE, "Could not find the accelerometer!");
    }
}

void setup()
{
    Serial.begin(115200);
    StatusLogger::setBrickStatus(StatusLogger::NAME_ESP32, StatusLogger::FUNCTIONALITY_FULL, "Session has been started.");
    connectToAccel();
}

void loop()
{
    if (
        StatusLogger::evalBrickStatus(StatusLogger::NAME_ACCEL, StatusLogger::FUNCTIONALITY_FULL) == 1 or
        StatusLogger::evalBrickStatus(StatusLogger::NAME_ACCEL, StatusLogger::FUNCTIONALITY_PARTIAL) == 1)
    // If the accelerometer was set up correctly
    {
        // Declare working vars
        int acc[3];
        sensors_event_t a, g, temp;
        bool read_success = true;

        // Check if we can *actually* read values from the accelerometer
        read_success = read_success and accel.getEvent(&a, &g, &temp);
        // We were able to read *something* from the Accelerometer
        read_success = read_success and (!(a.acceleration.x == 0) and (a.acceleration.y == 0) and (a.acceleration.z));
        // And not ALL values were exactly zero (which is statistically unlikely, given we should normally have rms of 9.81m/s2).

        if (read_success)
        // The accelerometer worked this time
        {
            if (StatusLogger::evalBrickStatus(StatusLogger::NAME_ACCEL, StatusLogger::FUNCTIONALITY_FULL) != 1)
            // I don't want this repeating again and again, != 1 means that the device does not yet equal FULL so let's set that status
            {
                StatusLogger::setBrickStatus(StatusLogger::NAME_ACCEL, StatusLogger::FUNCTIONALITY_FULL, "Acceleration is functioning as expected.");
            }
        }
        else
        {
            StatusLogger::setBrickStatus(StatusLogger::NAME_ACCEL, StatusLogger::FUNCTIONALITY_PARTIAL, "We weren't able to get a good reading...");
        }
    }
    else
    {
        StatusLogger::log(StatusLogger::LEVEL_WARNING, StatusLogger::NAME_ACCEL, "Still not connected to accelerometer....");
        connectToAccel();
    }
    delay(1000);
}