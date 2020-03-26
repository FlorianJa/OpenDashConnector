#ifndef OpenDashConnector_h
#define OpenDashConnector_h

#include "Arduino.h"
#include "OpenDashSensor.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> 

#define TZ              0       // (utc+) TZ in hours
#define DST_MN          60      // use 60mn for summer time in some countries
#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)


class OpenDashConnector
{
    public:
        OpenDashConnector()
        {

        }
        OpenDashConnector(const char* MQTTServerAddress, int MQTTPort, const char* MQTTUserName, const char* MQTTPassword, char* OpenDashUserName)
        {
            _mqttServerAddress = MQTTServerAddress;
            _mqttPort = MQTTPort;
            _mqttUserName = MQTTUserName;
            _mqttMQTTPassword = MQTTPassword;
            _openDashUserName = OpenDashUserName;

            configTime(TZ_SEC, DST_SEC, "pool.ntp.org");

            WiFiManager wifiManager;
            wifiManager.autoConnect("AutoConnectAP");

            _client = PubSubClient (_wiFiClient);
            _client.setServer(MQTTServerAddress, MQTTPort);
            
        }

        void update()
        {
            reconnect();
            sprintf(timestampInMs, "%lld", getTime()); //get time and convert to char[]
            
        }

        template<typename T> void Send(OpenDashSensor<T> Sensor)
        {
            Sensor.GetJson(message, timestampInMs, _openDashUserName);
            _client.publish(Sensor.GetTopic(), message);
        }

    private:
        WiFiClient _wiFiClient;
        PubSubClient _client;
        char message[500];
        const char* _mqttServerAddress;
        int _mqttPort;
        const char* _mqttUserName;
        const char* _mqttMQTTPassword;
        char* _openDashUserName;
        char timestampInMs[64];
        timeval tv;

        uint64_t getTime()
        {
            time(nullptr);
            gettimeofday(&tv, nullptr);
            uint64_t s = tv.tv_sec*1000ULL;
            uint64_t us = tv.tv_usec/1000ULL;
            return (uint64_t)(s+us);
        }

        void reconnect()
        {
            // Loop until we're reconnected
            while (!_client.connected()) 
            {
                Serial.print("Attempting MQTT connection...");
                // Create a random client ID
                String clientId = "ESP8266Client-";
                clientId += String(random(0xffff), HEX);
                // Attempt to connect
                if (_client.connect(clientId.c_str(), _mqttUserName,_mqttMQTTPassword)) 
                {
                    Serial.println("connected");
                } 
                else 
                {
                    Serial.print("failed, rc=");
                    Serial.print(_client.state());
                    Serial.println(" try again in 5 seconds");
                    // Wait 5 seconds before retrying
                    delay(5000);
                }
            }
        }



};
#endif