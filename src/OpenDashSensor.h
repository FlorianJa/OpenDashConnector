#ifndef OpenDashSensor_h
#define OpenDashSensor_h

#include "Arduino.h"
#include <ArduinoJson.h>

template<typename T> class OpenDashSensor
{
    public:
    OpenDashSensor()
    {

    }
    OpenDashSensor(const char* SensorID, const char* SensorName, const char* ValueTypeName, const char* ValueTypeUnit, const char* ValueTypeType, const char* MQTTTopic)
    {
        _id = SensorID;
        _sensorName = SensorName;
        _valueTypeName = ValueTypeName;
        _valueTypeUnit = ValueTypeUnit;
        _valueTypeType = ValueTypeType;
        _mQTTTopic = MQTTTopic;
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
        valueTypes_0["type"] = _valueTypeType;
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
        const char* _valueTypeType;
        const char* _mQTTTopic;
        T _value;
};

#endif
