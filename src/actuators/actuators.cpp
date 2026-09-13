#include "actuators/actuators.h"
#include "config.h"

bool lightState = false;
bool fanState = false;

void writeRelay(
    int pin,
    bool state
)
{
    if (RELAY_ACTIVE_HIGH)
    {
        digitalWrite(
            pin,
            state ? HIGH : LOW
        );
    }
    else
    {
        digitalWrite(
            pin,
            state ? LOW : HIGH
        );
    }
}

void actuatorsInit()
{
    pinMode(RELAY_LIGHT_PIN, OUTPUT);

    pinMode( RELAY_FAN_PIN, OUTPUT);

    setLight(false);
    setFan(false);
}

void setLight(bool state)
{
    lightState = state;

    writeRelay(RELAY_LIGHT_PIN, state);

    Serial.print("LIGHT: ");

    Serial.println(
        state ? "ON" : "OFF"
    );
}

bool getLightState()
{
    return lightState;
}

void setFan(bool state)
{
    fanState = state;

    writeRelay(RELAY_FAN_PIN, state);

    Serial.print("FAN: ");

    Serial.println(
        state ? "ON" : "OFF"
    );
}

bool getFanState()
{
    return fanState;
}