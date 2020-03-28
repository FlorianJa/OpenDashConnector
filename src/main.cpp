#include "Arduino.h"
#include "OpenDashConnector.h"
#include "DHTesp.h"


OpenDashSensor<float> dhtTemperature, dhtHumidity;
OpenDashConnector odc;
DHTesp dht;
long lastMsg = 0;

void setup()
{
    Serial.begin(9600);

    odc = OpenDashConnector(NULL, 1883, NULL, NULL, NULL, true, D0);//Change parameter for default values
    dhtTemperature = OpenDashSensor<float>("ODCDHT11Temp","DHT11 Wohnzimmer","Temperatur","°C","Number","florian-home/Wohnzimmer");
    dhtHumidity = OpenDashSensor<float>("ODCDHT11Hum","DHT11 Wohnzimmer","Luftfeuchtigkeit","%","Number","florian-home/Wohnzimmer");
    dht.setup(D1, DHTesp::DHT11); 
}

void loop()
{
    //keep mqtt connection alive
    odc.update();

    long now = millis();
    if (abs(now - lastMsg) > 2000) 
    {
        if(dht.getStatusString() == "OK")
        {
            lastMsg = now;
            dhtTemperature.SetValue(dht.getTemperature());  
            dhtHumidity.SetValue(dht.getHumidity());  

            odc.Send(dhtTemperature);
            odc.Send(dhtHumidity);
        }
    }
}