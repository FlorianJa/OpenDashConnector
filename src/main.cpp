#include "Arduino.h"
#include "OpenDashConnector.h"

OpenDashSensor<int> intSensor;
OpenDashSensor<float> floatSensor;
OpenDashConnector odc;

void setup()
{
    Serial.begin(9600);

    odc = OpenDashConnector("MQTTSERVER", 1883, "MQTTUSER", "MQTTPASSWORD", "OPENDASHUSERNAME");
    intSensor = OpenDashSensor<int>("TESTID1","INT","TEST","TESTINT","Number","Test/Test");
    floatSensor = OpenDashSensor<float>("TESTID2","FLOAT","TEST","TESTFLOAT","Number","Test/Test");
    
    intSensor.SetValue(1);
    floatSensor.SetValue(1.234f);
}

void loop()
{
    //keep mqtt connection alive
    odc.update();

    /*
    
    update sensor values
    
    */

    //send sensor values to mqtt/opendash
    odc.Send(intSensor);
    odc.Send(floatSensor);

    delay(1000);
}