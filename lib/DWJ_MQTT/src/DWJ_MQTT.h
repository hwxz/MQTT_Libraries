/**
 * @FilePath     : /MQTT_Demo/lib/DWJ_MQTT/src/DWJ_MQTT.h
 * @Description  : ESP32 MQTT客户端封装库 - 支持MQTTs和TCP协议
 * @Author       : 董文捷
 * @Version      : 1.0.0
 * @LastEditors  : 董文捷
 * @LastEditTime : 2025-06-18 12:00:00
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2025.
**/

#ifndef DWJ_MQTT_H
#define DWJ_MQTT_H

#include <ArduinoJson.h>
#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <functional>

// 协议类型枚举
enum MQTT_Protocol {
    MQTT_PROTOCOL_TCP = 0,    // TCP协议（无SSL）
    MQTT_PROTOCOL_SSL = 1     // SSL协议（MQTTs）
};

// 配置结构体
struct MQTT_Config {
    // WiFi配置
    const char* wifi_ssid;        // WiFi名称
    const char* wifi_password;    // WiFi密码
    
    // MQTT服务器配置
    const char* mqtt_broker;      // MQTT服务器地址
    int mqtt_port;                // MQTT端口
    const char* mqtt_client_id;   // 客户端ID
    const char* mqtt_username;    // MQTT用户名
    const char* mqtt_password;    // MQTT密码
    const char* mqtt_in_topic;    // 接收主题
    const char* mqtt_out_topic;   // 发送主题
    
    // 协议配置
    MQTT_Protocol protocol;       // 协议类型
    const char* root_ca;          // CA证书（仅SSL协议需要）
};

// 消息回调函数类型
typedef std::function<void(String topic, String message)> MQTT_MessageCallback;

// 公共API方法
void MQTT_Init(const MQTT_Config& config);
bool MQTT_Begin();
void MQTT_Loop();
void MQTT_SendMessage(const String& message);
void MQTT_SetMessageCallback(MQTT_MessageCallback callback);
bool MQTT_IsConnected();
bool MQTT_IsWiFiConnected();
bool MQTT_IsMQTTConnected();
MQTT_Protocol MQTT_GetProtocol();

// 便捷配置函数
void MQTT_ConfigTCP(const char* ssid, const char* password, const char* broker, 
                    int port, const char* client_id, const char* username, 
                    const char* password_mqtt, const char* in_topic, const char* out_topic);

void MQTT_ConfigSSL(const char* ssid, const char* password, const char* broker, 
                    int port, const char* client_id, const char* username, 
                    const char* password_mqtt, const char* in_topic, const char* out_topic,
                    const char* ca_cert);

// 为了兼容性，保留DWJ_MQTT别名
#define DWJ_MQTT MQTT

#endif // DWJ_MQTT_H 