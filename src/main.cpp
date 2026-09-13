#include <Arduino.h>

#include "config.h"
#include "sensors/sensors.h"
#include "actuators/actuators.h"
#include "mqtt_handler/mqtt_handler.h"

unsigned long lastSensorRead = 0;

unsigned long lastMotionTime = 0;

const float FAN_ON_TEMPERATURE = 30.0;

void handleLightAutomation(
    const SensorData& data
)
{
    // Có người + trời tối
    if (data.motion == HIGH && data.isDark)
    {
        lastMotionTime = millis();

        if (!getLightState())
        {
            setLight(true);

            mqttPublish(
                TOPIC_ACTUATOR_LIGHT,
                "ON",
                true
            );
        }
    }

    // Không còn chuyển động
    else if (data.motion == LOW)
    {
        if (getLightState() && millis() - lastMotionTime >= LIGHT_OFF_DELAY)
        {
            setLight(false);

            mqttPublish(
                TOPIC_ACTUATOR_LIGHT,
                "OFF",
                true
            );
        }
    }
}

void publishSensorData(const SensorData& data)
{
    char buffer[20];

    // Temperature
    if (!isnan(data.temperature))
    {
        dtostrf(data.temperature, 1, 2, buffer);

        mqttPublish(TOPIC_TEMPERATURE, buffer);
    }

    // Humidity
    if (!isnan(data.humidity))
    {
        dtostrf(data.humidity, 1, 2, buffer);

        mqttPublish(TOPIC_HUMIDITY, buffer);
    }

    // Motion
    snprintf(
        buffer,
        sizeof(buffer),
        "%d",
        data.motion
    );

    mqttPublish(TOPIC_MOTION, buffer);

    // Light
    snprintf(
        buffer,
        sizeof(buffer),
        "%d",
        data.lightValue
    );

    mqttPublish(TOPIC_LIGHT, buffer);
}

void handleFanAutomation(
    const SensorData& data
)
{
    // Không điều khiển tự động nếu DHT22 lỗi
    if (isnan(data.temperature))
    {
        return;
    }

    // Nhiệt độ >= 30°C → bật quạt
    if (data.temperature >= FAN_ON_TEMPERATURE)
    {
        if (!getFanState())
        {
            setFan(true);

            mqttPublish(
                TOPIC_ACTUATOR_FAN,
                "ON",
                true
            );
        }
    }

    // Nhiệt độ < 30°C → tắt quạt
    else
    {
        if (getFanState())
        {
            setFan(false);

            mqttPublish(
                TOPIC_ACTUATOR_FAN,
                "OFF",
                true
            );
        }
    }
}


void printSensorData(
    const SensorData& data
)
{
    Serial.println();

    Serial.println(
        "========== SENSOR DATA =========="
    );

    Serial.print(
        "PIR Motion: "
    );

    Serial.println(
        data.motion
    );

    Serial.print(
        "LDR Value: "
    );

    Serial.println(
        data.lightValue
    );

    Serial.print(
        "Dark: "
    );

    Serial.println(
        data.isDark
            ? "YES"
            : "NO"
    );

    if (isnan(data.temperature) || isnan(data.humidity))
    {
        Serial.println("DHT22: READ ERROR");
    }
    else
    {
        Serial.print("Temperature: ");

        Serial.print(data.temperature);

        Serial.println(" °C");

        Serial.print("Humidity: ");

        Serial.print(data.humidity);

        Serial.println(" %");
    }

    Serial.println("==================================");
}


void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();

    Serial.println("==================================");

    Serial.println(" SMART HOME IoT");

    Serial.println(" MODULAR VERSION");

    Serial.println("==================================");

    // Sensors
    sensorsInit();

    // Actuators
    actuatorsInit();

    // MQTT + WiFi
    mqttInit();

    Serial.println();

    Serial.println("System initialized.");
}


void loop()
{
    // MQTT
    mqttLoop();

    // Sensor timer
    unsigned long now = millis();

    if (now - lastSensorRead >= SENSOR_INTERVAL)
    {
        lastSensorRead = now;

        // Read sensors
        SensorData data = readSensors();

        // Serial
        printSensorData(data);

        // MQTT
        publishSensorData(data);

        // Automatic light
        handleLightAutomation(data);

        // Automatic fan
        handleFanAutomation(data);
    }
}