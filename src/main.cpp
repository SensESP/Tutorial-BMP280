/* 
 * This example sends data from an Adafruit-compatible BMP280 
 * temperature and pressure sensor (located in the engine room)
 * to a Signal K Server. Comments here in the program file are
 * minimal - for a full explanation, see
 * https://github.com/SignalK/SensESP/pages/tutorials/bmp280
 * 
 * Here's the sensor: https://www.adafruit.com/product/2651
 * 
*/

#include <Arduino.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include "sensesp_app.h"
#include "sensesp_app_builder.h"
#include "sensesp/sensors/sensor.h"
#include "sensesp/signalk/signalk_output.h"

using namespace sensesp;

// SensESP builds upon the ReactESP framework. Every ReactESP application
// must instantiate the "app" object.
reactesp::ReactESP app;

// Create an instance of the sensor using its I2C interface.
// See the Arduino Library example to see how to use SPI.
// https://github.com/adafruit/Adafruit_BMP280_Library/blob/master/examples/bmp280test/bmp280test.ino
Adafruit_BMP280 bmp280;

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
  bmp280.begin();

  // Read the sensor every 2 seconds
  unsigned int read_interval = 2000;

  // Create a RepeatSensor with float output that reads the temperature
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
