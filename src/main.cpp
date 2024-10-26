// Signal K application template file.
//
// This application demonstrates core SensESP concepts in a very
// concise manner. You can build and upload the application as is
// and observe the value changes on the serial port monitor.
//
// You can use this source file as a basis for your own projects.
// Remove the parts that are not relevant to you, and add your own code
// for external hardware libraries.

#include <memory>

#include "sensesp.h"
#include "sensesp/sensors/analog_input.h"
#include "sensesp/sensors/digital_input.h"
#include "sensesp/sensors/sensor.h"
#include "sensesp/signalk/signalk_output.h"
#include "sensesp/system/lambda_consumer.h"
#include "sensesp_app_builder.h"

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

using namespace sensesp;

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

unsigned long delayTime;


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

    unsigned status;
    
  // default settings
  status = bme.begin();  
  // You can also pass in a Wire library object like &Wire2
  // status = bme.begin(0x76, &Wire2)
  if (!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
      Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
      Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
      Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
      Serial.print("        ID of 0x60 represents a BME 280.\n");
      Serial.print("        ID of 0x61 represents a BME 680.\n");
      while (1) delay(10);
  }
  
  Serial.println("-- Default Test --");
  delayTime = 1000;


  // GPIO number to use for the analog input
  const uint8_t kAnalogInputPin = 36;
  // Define how often (in milliseconds) new samples are acquired
  const unsigned int kAnalogInputReadInterval = 500;
  // Define the produced value at the maximum input voltage (3.3V).
  // A value of 3.3 gives output equal to the input voltage.
  const float kAnalogInputScale = 3.3;

  // Create a new Analog Input Sensor that reads an analog input pin
  // periodically.
  auto analog_input = std::make_shared<AnalogInput>(
      kAnalogInputPin, kAnalogInputReadInterval, "", kAnalogInputScale);

  // Add an observer that prints out the current value of the analog input
  // every time it changes.
  analog_input->attach([analog_input]() {
    debugD("Analog input value: %f", analog_input->get());
  });

  // Set GPIO pin 15 to output and toggle it every 650 ms

  const uint8_t kDigitalOutputPin = 15;
  const unsigned int kDigitalOutputInterval = 650;
  pinMode(kDigitalOutputPin, OUTPUT);
  event_loop()->onRepeat(kDigitalOutputInterval, [kDigitalOutputPin]() {
    digitalWrite(kDigitalOutputPin, !digitalRead(kDigitalOutputPin));
  });

  // Read GPIO 14 every time it changes

  const uint8_t kDigitalInput1Pin = 14;
  auto digital_input1 = std::make_shared<DigitalInputChange>(
      kDigitalInput1Pin, INPUT_PULLUP, CHANGE);

  // Connect the digital input to a lambda consumer that prints out the
  // value every time it changes.

  // Test this yourself by connecting pin 15 to pin 14 with a jumper wire and
  // see if the value changes!

  auto digital_input1_consumer = std::make_shared<LambdaConsumer<bool>>(
      [](bool input) { debugD("Digital input value changed: %d", input); });

  digital_input1->connect_to(digital_input1_consumer);

  // BME280 Temperature, Pressure, Altitude and Humidty Sensor
  //
  // Create another digital input, this time with RepeatSensor. This approach
  // can be used to connect external sensor library to SensESP!

  const uint8_t kBME280Pin = 13;
  const unsigned int kBME280Interval = 1000;

  // Configure the pin. Replace this with your custom library initialization
  // code!
  pinMode(kDigitalInput2Pin, INPUT_PULLUP);

  // Define a new RepeatSensor that reads the pin every 100 ms.
  // Replace the lambda function internals with the input routine of your custom
  // library.  // Create another digital input, this time with RepeatSensor. This approach
  // can be used to connect external sensor library to SensESP!

  const uint8_t kDigitalInput2Pin = 13;
  const unsigned int kDigitalInput2Interval = 1000;

  // Configure the pin. Replace this with your custom library initialization
  // code!
  pinMode(kDigitalInput2Pin, INPUT_PULLUP);

  // Define a new RepeatSensor that reads the pin every 100 ms.
  // Replace the lambda function internals with the input routine of your custom
  // library.

  // Again, test this yourself by connecting pin 15 to pin 13 with a jumper
  // wire and see if the value changes!

  auto digital_input2 = std::make_shared<RepeatSensor<bool>>(
      kDigitalInput2Interval,
      [kDigitalInput2Pin]() { return digitalRead(kDigitalInput2Pin); });

  // Create another digital input, this time with RepeatSensor. This approach
  // can be used to connect external sensor library to SensESP!

  const uint8_t kDigitalInput2Pin = 13;
  const unsigned int kDigitalInput2Interval = 1000;

  // Configure the pin. Replace this with your custom library initialization
  // code!
  pinMode(kDigitalInput2Pin, INPUT_PULLUP);

  // Define a new RepeatSensor that reads the pin every 100 ms.
  // Replace the lambda function internals with the input routine of your custom
  // library.

  // Again, test this yourself by connecting pin 15 to pin 13 with a jumper
  // wire and see if the value changes!

  auto digital_input2 = std::make_shared<RepeatSensor<bool>>(
      kDigitalInput2Interval,
      [kDigitalInput2Pin]() { return digitalRead(kDigitalInput2Pin); });

  // Connect the analog input to Signal K output. This will publish the
  // analog input value to the Signal K server every time it changes.
  auto aiv_metadata = std::make_shared<SKMetadata>("V", "Analog input voltage");
  auto aiv_sk_output = std::make_shared<SKOutput<float>>(
      "sensors.analog_input.voltage",   // Signal K path
      "/Sensors/Analog Input/Voltage",  // configuration path, used in the
                                        // web UI and for storing the
                                        // configuration
      aiv_metadata
  );

  ConfigItem(aiv_sk_output)
      ->set_title("Analog Input Voltage SK Output Path")
      ->set_description("The SK path to publish the analog input voltage")
      ->set_sort_order(100);

  analog_input->connect_to(aiv_sk_output);

  // Connect digital input 2 to Signal K output.
  auto di2_metadata = std::make_shared<SKMetadata>("", "Digital input 2 value");
  auto di2_sk_output = std::make_shared<SKOutput<bool>>(
      "sensors.digital_input2.value",    // Signal K path
      "/Sensors/Digital Input 2/Value",  // configuration path
      di2_metadata
  );

  ConfigItem(di2_sk_output)
      ->set_title("Digital Input 2 SK Output Path")
      ->set_sort_order(200);

  digital_input2->connect_to(di2_sk_output);

  // To avoid garbage collecting all shared pointers created in setup(),
  // loop from here.
  while (true) {
    loop();
  }
}

void loop() { event_loop()->tick(); }
