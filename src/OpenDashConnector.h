#ifndef OpenDashConnector_h
#define OpenDashConnector_h

#include "Arduino.h"
#include <FS.h>
#include <LittleFS.h>

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
        
        OpenDashConnector(const char* MQTTServerAddress, int MQTTPort, const char* MQTTUserName, const char* MQTTPassword, const char* OpenDashUserName, bool enableResetButton, int ResetButtonPin)
        {
            initParameters(MQTTServerAddress, MQTTPort, MQTTUserName, MQTTPassword, OpenDashUserName, enableResetButton, ResetButtonPin);
            
            configTime(TZ_SEC, DST_SEC, "pool.ntp.org");

            SetupWiFiManager();
                        
            _client = PubSubClient (_wiFiClient);
            _client.setServer(_mqttServerAddress, _mqttPort);
            
        }

        void update()
        {
            reconnect();
            _client.loop();
        }

        template<typename T> void Send(OpenDashSensor<T> Sensor)
        {
            sprintf(timestampInMs, "%lld", getTime()); //get time and convert to char[]
            Sensor.GetJson(message, timestampInMs, _openDashUserName);
            _client.publish(Sensor.GetTopic(), message);
        }

    private:
        WiFiClient _wiFiClient;
        PubSubClient _client;
        char message[500];
        char* _mqttServerAddress;
        int _mqttPort;
        char _mqttPortAsChar[5];
        char* _mqttUserName;
        char* _mqttMQTTPassword;
        char* _openDashUserName;
        char timestampInMs[64];
        timeval tv;
        int _resetButtonPin;
        bool _resetButtonEnabled = false;

        void initParameters(const char* MQTTServerAddress, int MQTTPort, const char* MQTTUserName, const char* MQTTPassword, const char* OpenDashUserName, bool enableResetButton, int ResetButtonPin)
        {
            _mqttServerAddress = new char[100+1];
            for (int i = 0; i < 100 + 1; i++) 
            {
                _mqttServerAddress[i] = 0;
            }

            if (MQTTServerAddress != NULL) 
            {
                strncpy(_mqttServerAddress, MQTTServerAddress, 100);
            }

            _mqttUserName = new char[50+1];
            for (int i = 0; i < 50 + 1; i++) 
            {
                _mqttUserName[i] = 0;
            }

            if (MQTTUserName != NULL) 
            {
                strncpy(_mqttUserName, MQTTUserName, 50);
            }
            
            _mqttMQTTPassword = new char[30+1];
            for (int i = 0; i < 30 + 1; i++) 
            {
                _mqttMQTTPassword[i] = 0;
            }

            if (MQTTPassword != NULL) 
            {
                strncpy(_mqttMQTTPassword, MQTTPassword, 30);
            }
            
            _openDashUserName = new char[50+1];
            for (int i = 0; i < 50 + 1; i++) 
            {
             _openDashUserName[i] = 0;
            }

            if (OpenDashUserName != NULL) 
            {
                strncpy(_openDashUserName, OpenDashUserName, 50);
            }

            _mqttPort = MQTTPort;
            itoa(_mqttPort, _mqttPortAsChar, 10);

            _resetButtonPin = ResetButtonPin;
            _resetButtonEnabled = enableResetButton;
        }

        void SetupWiFiManager()
        {
            WiFiManager _wifiManager;
            WiFiManagerParameter custom_mqtt_server("server", "mqtt server", _mqttServerAddress, 100);
            WiFiManagerParameter custom_mqtt_port("port", "mqtt port", _mqttPortAsChar, 6);
            WiFiManagerParameter custom_mqtt_user("user", "mqtt user", _mqttUserName, 34);
            WiFiManagerParameter custom_mqtt_password("password", "mqtt password", _mqttMQTTPassword, 34);
            WiFiManagerParameter custom_openDash_user("openDashUser", "openDash user", _openDashUserName, 50);
            bool saveConfigs = false;
            bool loadConfig = false;

            if(_resetButtonEnabled)
            {
                pinMode(_resetButtonPin, INPUT_PULLUP);

                if(digitalRead(_resetButtonPin) == LOW)
                {
                    delay(50);

                    if (digitalRead(_resetButtonPin) == LOW)
                    {
                        _wifiManager.resetSettings();
                        _wifiManager.addParameter(&custom_mqtt_server);
                        _wifiManager.addParameter(&custom_mqtt_port);
                        _wifiManager.addParameter(&custom_mqtt_user);
                        _wifiManager.addParameter(&custom_mqtt_password);
                        _wifiManager.addParameter(&custom_openDash_user);
                        saveConfigs = true;
                    }
                    else
                    {
                        loadConfig = true;
                    }
                }
                else
                {
                    loadConfig= true;
                }
            }
            else
            {
                loadConfig= true;
            }

            if(loadConfig)
            {
                if(loadMQTTConfig() == false)
                {
                    _wifiManager.resetSettings();
                    _wifiManager.addParameter(&custom_mqtt_server);
                    _wifiManager.addParameter(&custom_mqtt_port);
                    _wifiManager.addParameter(&custom_mqtt_user);
                    _wifiManager.addParameter(&custom_mqtt_password);
                    _wifiManager.addParameter(&custom_openDash_user);
                    saveConfigs = true;
                }
            }

            _wifiManager.autoConnect();

            if (saveConfigs)
            {
                Serial.println("save parameters");
                //read updated parameters
                strcpy((char*)_mqttServerAddress, custom_mqtt_server.getValue());
                strcpy((char*)_mqttPortAsChar, custom_mqtt_port.getValue());
                strcpy((char*)_mqttUserName, custom_mqtt_user.getValue());
                strcpy((char*)_mqttMQTTPassword, custom_mqtt_password.getValue());
                strcpy((char*)_openDashUserName, custom_openDash_user.getValue());
                saveMQTTConfig();
            }
        }
        
        void saveMQTTConfig()
        {
            Serial.println("saving config");
            DynamicJsonDocument doc(1024);
            doc["mqtt_server"] = _mqttServerAddress;
            doc["mqtt_port"] = _mqttPortAsChar;
            doc["mqtt_user"] = _mqttUserName;
            doc["mqtt_password"] = _mqttMQTTPassword;
            doc["openDash_user"] = _openDashUserName;

            if (!LittleFS.begin()) {
                Serial.println("LittleFS mount failed");
                return;
            }
            File configFile = LittleFS.open("/config.json", "w");
            if (!configFile) {
                Serial.println("Failed to open file for writing");
                return;
            }
            serializeJson(doc, Serial);
            serializeJson(doc, configFile);
            configFile.close();
        }

        bool loadMQTTConfig()
        {
            Serial.println("mounting FS...");

            if (LittleFS.begin())
            {
                Serial.println("mounted file system");
                File configFile = LittleFS.open("/config.json", "r");
                if (!configFile) 
                {
                    Serial.println("Failed to open file for reading");
                    return false;
                }

                Serial.println("opened config file");
                size_t size = configFile.size();
                // Allocate a buffer to store contents of the file.
                std::unique_ptr<char[]> buf(new char[size]);

                configFile.readBytes(buf.get(), size);
                DynamicJsonDocument doc(1024);
                DeserializationError err = deserializeJson(doc, buf.get());

                if (err)
                {
                    Serial.println("failed to load json config");
                    return false;
                }
                else
                {
                    Serial.println("\nparsed json");

                    strcpy((char*)_mqttServerAddress, doc["mqtt_server"]);
                    strcpy((char*)_mqttPortAsChar, doc["mqtt_port"]);
                    strcpy((char*)_mqttUserName, doc["mqtt_user"]);
                    strcpy((char*)_mqttMQTTPassword, doc["mqtt_password"]);
                    strcpy((char*)_openDashUserName, doc["openDash_user"]);
                }
                Serial.println("json parsed");
                configFile.close();
                serializeJson(doc, Serial);
                return true;
            }
            else
            {
                Serial.println("failed to mount FS");
                return false;
            }
        }

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