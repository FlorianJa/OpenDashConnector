# OpenDashConnector

Basic structure for bringing am ESP8266 into WiFi, connecting to a MQTT broker and sending data to open.DASH.

## Usage:
See the src/main.cpp for example.  

### Create a OpenDashConnector object

`OpenDashConnector odc = OpenDashConnector(MQTTServerAddress, MQTTPort, MQTTUserName, MQTTPassword, OpenDashUserName, enableResetButton, ResetButtonPin);`

The [WiFiManager](https://github.com/tzapu/WiFiManager) will create an AP if no WiFi credentials are stored in EEPROM or if the stored WiFi is not available. When connecting to this AP, there is a web-interface to insert all important data.
Stored data can be reset with the reset pin (high-active).

![WifiManager](https://github.com/FlorianJa/OpenDashConnector/blob/master/WiFiManager.png)


### Creating an OpenDashSensor:
`OpenDashSensor<TYPE> Temperature = OpenDashSensor<TYPE>("TESTID","TEST TEMPERATUR","Temperatur","°C", OPENDASH_TYPE::NUMBER,"MQTT/TOPIC");`

First, specify which datatype is used internally to store the sensor value ( TYPE = int, long, float, double, String). Second, define the open.DASH parameters.

### Sending data to open.DASH:
Measure the value for your sensor. This is depending on your sensor.  
Store the value in the open.DASH sensor -> `Temperature.SetValue(value)`;  
Send the Data to open.DASH -> `odc.Send(Temperature)`;  

### Keep connection alive:
In Arduino loop, call `odc.update();` This function checks the WiFi connection and reconnects to WiFi and MQTT if necessary. This funcation should be called regularly.


## Dependencies
[WifiManager](https://github.com/tzapu/WiFiManager)  
[ArduinoJSON](https://github.com/bblanchon/ArduinoJson)  
[LittleFS](https://github.com/esp8266/Arduino/tree/master/libraries/LittleFS)  
[PubSubClient](https://github.com/knolleary/pubsubclient)
