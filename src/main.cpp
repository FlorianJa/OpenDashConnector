#include "Arduino.h"
#include "OpenDashConnector.h"
#include "DHTesp.h"


OpenDashSensor<long> Temperature;
OpenDashConnector odc;
DHTesp dht;
long lastMsg = 0;

void setup()
{
    Serial.begin(9600);

    odc = OpenDashConnector(NULL, 1883, NULL, NULL, NULL, true, D0);//Change parameter for default values
    Temperature = OpenDashSensor<long>("TESTID","TEST TEMPERATUR","Temperatur","°C", OPENDASH_TYPE::NUMBER,"florian-home/Test");
}

void loop()
{
    // keep mqtt connection alive
    odc.update();
    long now = millis();
    if (abs(now - lastMsg) > 2000) 
    {
        lastMsg = now;
        Temperature.SetValue(millis());
        odc.Send(Temperature);
    }
}