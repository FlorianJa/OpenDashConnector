#include "Arduino.h"
#include "OpenDashConnector.h"
#include "DHTesp.h"


OpenDashSensor<float> dhtTemperature;
OpenDashSensor<int> dhtHumidity, Moisture;
OpenDashConnector odc;
DHTesp dht;
long lastMsgTH = 0;
long lastMsgM = 0;

void setup()
{
    Serial.begin(9600);

    odc = OpenDashConnector(NULL, 1883, NULL, NULL, NULL, true, D0);//Change parameter for default values
    dhtTemperature = OpenDashSensor<float>("ODCDHT11Temp","DHT11 Wohnzimmer","Temperatur","°C",OPENDASH_TYPE::NUMBER,"florian-home/Wohnzimmer");
    dhtHumidity = OpenDashSensor<int>("ODCDHT11Hum","DHT11 Wohnzimmer","Luftfeuchtigkeit","%",OPENDASH_TYPE::NUMBER,"florian-home/Wohnzimmer");
    Moisture = OpenDashSensor<int>("ODCDHT11Moi","Friedenslilie","Bodenfeuchtigkeit","RAW",OPENDASH_TYPE::NUMBER,"florian-home/Wohnzimmer");
    dht.setup(D1, DHTesp::DHT11); 
}

void loop()
{
    // keep mqtt connection alive
    odc.update();
    long now = millis();
    if (abs(now - lastMsgTH) > 2000) 
    {

        if(dht.getStatusString() == "OK")
        {
            lastMsgTH = now;
            dhtTemperature.SetValue(dht.getTemperature());  
            dhtHumidity.SetValue(dht.getHumidity());  

            odc.Send(dhtTemperature);
            odc.Send(dhtHumidity);
        }
    }

    if (abs(now - lastMsgM) > 2000) 
    {
        lastMsgM = now;
        Moisture.SetValue(analogRead(A0));
        odc.Send(Moisture);
    }
}