#ifndef OpenDashSensor_h
#define OpenDashSensor_h

#include "Arduino.h"
#include <ArduinoJson.h>

typedef enum {
        NUMBER,
        BOOL
    }
    OPENDASH_TYPE;

template<typename T> class OpenDashSensor
{
    public:

        OpenDashSensor()
        {

        }

        OpenDashSensor(const char* SensorID, const char* SensorName, const char* ValueTypeName, const char* ValueTypeUnit, OPENDASH_TYPE Type, const char* MQTTTopic)
        {
            _id = SensorID;
            _sensorName = SensorName;
            _valueTypeName = ValueTypeName;
            _valueTypeUnit = ValueTypeUnit;
            _valueTypeType = Type;
            _mQTTTopic = MQTTTopic;

            _type = new char[10];
            for (size_t i = 0; i < 10; i++)
            {
                _type[i] = 0;
            }
            switch (Type)
            {
                case NUMBER:
                    strncpy(_type, "Number", 7);
                    break;
                case BOOL:
                    strncpy(_type, "Bool", 7);
                    break;
                default:
                    break;
            }
        }

        void SetValue(T value)
        {
            _value = value;
        }
        
        void GetJson(char* json, char* ms, const char* openDashUserName)
        {
            DynamicJsonDocument doc(500);
            
            doc["id"] = _id;
            doc.createNestedArray("parent");
            doc.createNestedObject("meta");
            doc["name"] = _sensorName;
            JsonArray valueTypes = doc.createNestedArray("valueTypes");
            JsonObject valueTypes_0 = valueTypes.createNestedObject();
            valueTypes_0["name"] = _valueTypeName;
            valueTypes_0["unit"] = _valueTypeUnit;
            valueTypes_0["type"] = _type;
            doc["user"] = openDashUserName;
            JsonArray values = doc.createNestedArray("values");
            JsonObject values_0 = values.createNestedObject();
            values_0["date"] = ms;
            JsonArray values_0_value = values_0.createNestedArray("value");
            values_0_value.add(_value);

            serializeJson(doc, json,500);
        }

        const char* GetTopic()
        {
            return _mQTTTopic;
        }

    private:
        const char* _sensorName;
        const char* _id;
        const char* _valueTypeName;
        const char* _valueTypeUnit;
        char* _type;
        OPENDASH_TYPE _valueTypeType;
        const char* _mQTTTopic;
        T _value;
};

#endif
