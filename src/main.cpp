/* 
 * This example sends data from an Adafruit-compatible BMP280 
 * temperature and pressure sensor (located in the engine room)
 * to a Signal K Server. Comments here in the program file are
 * minimal - for a full explanation, see
 * https://github.com/SignalK/SensESP/pages/tutorials/bmp280
 * 
 * Here's the sensor: https://www.adafruit.com/product/2651
 *
 * NOTE: Use this as a template for almost any other Arduino-compatible
 * physical sensor.
*/

// Boilerplate #includes:
#include <Arduino.h>
#include "sensesp_app.h"
#include "sensesp_app_builder.h"
#include "sensesp/sensors/sensor.h"
#include "sensesp/signalk/signalk_output.h"

// Sensor-specific #includes go here:
#include <Adafruit_BMP280.h>
#include <Wire.h>

using namespace sensesp;

// SensESP builds upon the ReactESP framework. Every ReactESP application
// must instantiate the "app" object.
reactesp::ReactESP app;

// (Specific to the BMP280, and I2C. Replace this with similar code to create an instance
// of whatever sensor you may be using in your project.)
// Create an instance of the sensor using its I2C interface.
Adafruit_BMP280 bmp280;

// (Replace this with whatever function you need to read whatever value you want
// to read from any other sensor you're using in your project.)
// Define the function that will be called every time we want
// an updated temperature value from the sensor. The sensor reads degrees
// Celsius, but all temps in Signal K are in Kelvin, so add 273.15.
float read_temp_callback() { return (bmp280.readTemperature() + 273.15); }

// The setup function performs one-time application initialization
void setup() {

// Some initialization boilerplate when in debug mode
#ifndef SERIAL_DEBUG_DISABLED
  SetupSerialDebug(115200);
#endif

  // Create the global SensESPApp() object
  SensESPAppBuilder builder;
  sensesp_app = builder.get_app();

  // Initialize the BMP280 using the default address
  // (Do whatever is required to "start" your project's sensor here)
  bmp280.begin();

  // Read the sensor every 2 seconds
  unsigned int read_interval = 2000;

  // Create a RepeatSensor with float output that reads the temperature
  // using the function defined above.
  auto* engine_room_temp =
      new RepeatSensor<float>(read_interval, read_temp_callback);

  // Set the Signal K Path for the output
  const char* sk_path = "propulsion.engineRoom.temperature";

  // Send the temperature to the Signal K server as a Float
  engine_room_temp->connect_to(new SKOutputFloat(sk_path));

  // Start the SensESP application running
  sensesp_app->start();

} //setup

// loop simply calls `app.tick()` which will then execute all reactions as needed
void loop() {
  
  app.tick();
  
}
