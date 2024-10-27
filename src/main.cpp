// Signal K application template file.
//
// This application demonstrates core SensESP concepts in a very
// concise manner. You can build and upload the application as is
// and observe the value changes on the serial port monitor.
//
// You can use this source file as a basis for your own projects.
// Remove the parts that are not relevant to you, and add your own code
// for external hardware libraries.

// #include <memory>

#include "sensesp.h"
#include "sensesp/sensors/analog_input.h"
#include "sensesp/sensors/digital_input.h"
#include "sensesp/sensors/sensor.h"
#include "sensesp/signalk/signalk_output.h"
#include "sensesp/system/lambda_consumer.h"
#include "sensesp/system/led_blinker.h"
#include "sensesp_app_builder.h"

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


using namespace sensesp;


// BME280

Adafruit_BME280 bme280;

#define SEALEVELPRESSURE_HPA (1013.25)

float read_temp_callback() {
  float bmeTemp = bme280.readTemperature();
  debugD("BME280 Temperature: %4.2fC", bmeTemp);
  debugD("BME280 Temperature: %4.2fF", 1.8 * bmeTemp + 32);
  return (bmeTemp + 273.15);
}
float read_pressure_callback() {
  float bmePressure = bme280.readPressure() / 100.0F;
  debugD("BME280 Pressure: %4.2fhPa", bmePressure);
  return (bmePressure);
}
float read_humidity_callback() {
  float bmeHumidity = bme280.readHumidity();
  debugD("BME280 Humidity: %4.2f", bmeHumidity);
  return (bmeHumidity);
}
float read_altitude_callback() {
  float bmeAltitude = bme280.readAltitude(SEALEVELPRESSURE_HPA);
  debugD("BME280 Altitude: %4.2fm", bmeAltitude);
  return (bmeAltitude);
}


// The setup function performs one-time application initialization.
void setup() {
  SetupLogging(ESP_LOG_DEBUG);

  // Construct the global SensESPApp() object
  SensESPAppBuilder builder;
  sensesp_app = (&builder)
                    // Set a custom hostname for the app.
                    ->set_hostname("my-sensesp-project")
                    // Optionally, hard-code the WiFi and Signal K server
                    // settings. This is normally not needed.
                    //->set_wifi_client("My WiFi SSID", "my_wifi_password")
                    //->set_wifi_access_point("My AP SSID", "my_ap_password")
                    //->set_sk_server("192.168.10.3", 80)
                    ->get_app();


  /// BME280 SENSOR CODE - Temp/Humidity/Altitude/Pressure Sensor ////

  // 0x77 is the default address. Some chips use 0x76, which is shown here.
  // If you need to use the TwoWire library instead of the Wire library, there
  // is a different constructor: see bmp280.h

  bme280.begin();
  // Create a RepeatSensor with float output that reads the temperature
  // using the function defined above.
  auto* bme280_temp =
      // new RepeatSensor<float>(5000, read_temp_callback);
      new RepeatSensor<float>(10000, read_temp_callback);

  auto* bme280_pressure =
      // new RepeatSensor<float>(60000, read_pressure_callback);
      new RepeatSensor<float>(10000, read_pressure_callback);

  auto* bme280_humidity =
      // new RepeatSensor<float>(60000, read_humidity_callback);
      new RepeatSensor<float>(10000, read_humidity_callback);

  auto* bme280_altitude =
      // new RepeatSensor<float>(60000, read_humidity_callback);
      new RepeatSensor<float>(10000, read_altitude_callback);

  // Send the temperature to the Signal K server as a Float
  bme280_temp->connect_to(new SKOutputFloat("environment.temperature"));

  bme280_pressure->connect_to(new SKOutputFloat("environment.pressure"));

  bme280_humidity->connect_to(new SKOutputFloat("environment.relativeHumidity"));

  bme280_altitude->connect_to(new SKOutputFloat("environment.altitude"));


  // Heartbeat LED

  const int hbLedPin = 25;
  const unsigned int kHBledInterval = 1000;

  EvenBlinker hbLED(hbLedPin, kHBledInterval);  // Enabled by default
  // hbLED.set_enabled(true);


  // To avoid garbage collecting all shared pointers created in setup(),
  // loop from here.
  while (true) {
    loop();
  }
}

void loop() { event_loop()->tick(); }
