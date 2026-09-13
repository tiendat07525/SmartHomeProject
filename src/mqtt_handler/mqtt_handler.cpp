#include "mqtt_handler.h"
#include "config.h"
#include "actuators/actuators.h"
#include <WiFi.h>
#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient mqttClient(espClient);
void connectWiFi(){
    Serial.print("Connecting to WiFi");
    WiFi.begin(WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected!");
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());
}
void connectMQTT(){
    while (!mqttClient.connected()){
        Serial.print("Connecting to MQTT... ");
        String clientId = "ESP32-SmartHome-";
        clientId += String(random(0xffff), HEX);
        if (mqttClient.connect(clientId.c_str())){
            Serial.println("PASS");
            mqttClient.subscribe(TOPIC_CONTROL_LIGHT);
            mqttClient.subscribe(TOPIC_CONTROL_FAN);
            mqttPublish(TOPIC_ACTUATOR_LIGHT, getLightState() ? "ON" : "OFF", true);
            mqttPublish( TOPIC_ACTUATOR_FAN, getFanState() ? "ON" : "OFF", true);
        } else{
            Serial.print("FAILED, state=");
            Serial.println(mqttClient.state());
            delay(3000);
        }
    }
}

void mqttCallback(char* topic, byte* payload, unsigned int length){
    String message;
    for (unsigned int i = 0; i < length; i++){
        message += (char)payload[i];
    }
    message.trim();
    Serial.println();
    Serial.println("========== MQTT COMMAND ==========");
    Serial.print("Topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    Serial.println(message);
    Serial.println("==================================");
    if (String(topic) == TOPIC_CONTROL_LIGHT){
        if (message.equalsIgnoreCase("ON")){
            setLight(true);
        }
        else if (message.equalsIgnoreCase("OFF")){
            setLight(false);
        }
    }
    if (String(topic) == TOPIC_CONTROL_FAN){
        if (message.equalsIgnoreCase("ON")){
            setFan(true);
        }
        else if (message.equalsIgnoreCase("OFF")){
            setFan(false);
        }
    }
}
void mqttInit(){
    connectWiFi();
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
}
void mqttLoop(){
    if (!mqttClient.connected()){
        connectMQTT();
    }
    mqttClient.loop();
}
bool mqttConnected(){
    return mqttClient.connected();
}
void mqttPublish(const char* topic, const char* message,bool retained){
    if (mqttClient.connected()){
        mqttClient.publish(topic, message, retained);
    }
}