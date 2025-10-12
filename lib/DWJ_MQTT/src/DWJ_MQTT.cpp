/**
 * @FilePath     : /MQTT_Demo/lib/DWJ_MQTT/src/DWJ_MQTT.cpp
 * @Description  : ESP32 MQTT客户端封装库实现 - 支持MQTTs和TCP协议
 * @Author       : 董文捷
 * @Version      : 1.0.0
 * @LastEditors  : 董文捷
 * @LastEditTime : 2025-06-18 12:00:00
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2025.
**/

#include "DWJ_MQTT.h"

// 静态成员变量
static MQTT_Config* config = nullptr;
static WiFiClient* wifiClient = nullptr;
static WiFiClientSecure* wifiClientSecure = nullptr;
static MqttClient* mqttClient = nullptr;
static MQTT_MessageCallback messageCallback = nullptr;
static bool initialized = false;
static bool wifiConnected = false;
static bool mqttConnected = false;
static String inputString;
static unsigned long lastReconnectAttempt = 0;
static const unsigned long RECONNECT_INTERVAL = 5000; // 5秒重连间隔

// 私有方法声明
static bool connectWiFi();
static bool connectMQTT();
static bool subscribe();
static void onMqttMessage(int messageSize);
static void handleReconnection();
static void createMQTTClient();

// 初始化配置
void MQTT_Init(const MQTT_Config& cfg) {
    if (config != nullptr) {
        delete config;
    }
    config = new MQTT_Config(cfg);
    initialized = true;
    
    Serial.print("MQTT: 配置已初始化 - 协议: ");
    Serial.println(config->protocol == MQTT_PROTOCOL_TCP ? "TCP" : "SSL");
}

// 便捷配置函数 - TCP协议
void MQTT_ConfigTCP(const char* ssid, const char* password, const char* broker, 
                    int port, const char* client_id, const char* username, 
                    const char* password_mqtt, const char* in_topic, const char* out_topic) {
    MQTT_Config cfg;
    cfg.wifi_ssid = ssid;                    // WiFi名称
    cfg.wifi_password = password;            // WiFi密码
    cfg.mqtt_broker = broker;                // MQTT服务器地址
    cfg.mqtt_port = port;                    // MQTT端口
    cfg.mqtt_client_id = client_id;          // 客户端ID
    cfg.mqtt_username = username;            // MQTT用户名
    cfg.mqtt_password = password_mqtt;       // MQTT密码
    cfg.mqtt_in_topic = in_topic;            // 接收主题
    cfg.mqtt_out_topic = out_topic;          // 发送主题
    cfg.protocol = MQTT_PROTOCOL_TCP;        // TCP协议
    cfg.root_ca = nullptr;                   // TCP不需要CA证书
    
    MQTT_Init(cfg);
}

// 便捷配置函数 - SSL协议
void MQTT_ConfigSSL(const char* ssid, const char* password, const char* broker, 
                    int port, const char* client_id, const char* username, 
                    const char* password_mqtt, const char* in_topic, const char* out_topic,
                    const char* ca_cert) {
    MQTT_Config cfg;
    cfg.wifi_ssid = ssid;                    // WiFi名称
    cfg.wifi_password = password;            // WiFi密码
    cfg.mqtt_broker = broker;                // MQTT服务器地址
    cfg.mqtt_port = port;                    // MQTT端口
    cfg.mqtt_client_id = client_id;          // 客户端ID
    cfg.mqtt_username = username;            // MQTT用户名
    cfg.mqtt_password = password_mqtt;       // MQTT密码
    cfg.mqtt_in_topic = in_topic;            // 接收主题
    cfg.mqtt_out_topic = out_topic;          // 发送主题
    cfg.protocol = MQTT_PROTOCOL_SSL;        // SSL协议
    cfg.root_ca = ca_cert;                   // CA证书
    
    MQTT_Init(cfg);
}

// 开始连接
bool MQTT_Begin() {
    if (!initialized || config == nullptr) {
        Serial.println("MQTT: 错误 - 请先调用MQTT_Init()初始化配置");
        return false;
    }

    Serial.println("MQTT: 开始初始化...");

    // 创建MQTT客户端
    createMQTTClient();

    // 连接WiFi
    if (!connectWiFi()) {
        Serial.println("MQTT: WiFi连接失败");
        return false;
    }

    // 连接MQTT
    if (!connectMQTT()) {
        Serial.println("MQTT: MQTT连接失败");
        return false;
    }

    // 订阅主题
    if (!subscribe()) {
        Serial.println("MQTT: 订阅主题失败");
        return false;
    }

    Serial.println("MQTT: 初始化完成");
    return true;
}

// 主循环
void MQTT_Loop() {
    if (!initialized || mqttClient == nullptr) {
        return;
    }

    // 轮询MQTT消息
    mqttClient->poll();

    // 处理重连
    handleReconnection();
}

// 发送消息
void MQTT_SendMessage(const String& message) {
    if (!mqttConnected || mqttClient == nullptr || config == nullptr) {
        Serial.println("MQTT: 未连接，无法发送消息");
        return;
    }

    Serial.print("MQTT: 发送消息到主题: ");
    Serial.println(config->mqtt_out_topic);
    Serial.println(message);

    const bool retained = false;
    const int qos = 1;
    const bool dup = false;

    mqttClient->beginMessage(config->mqtt_out_topic, message.length(), retained, qos, dup);
    mqttClient->print(message);
    mqttClient->endMessage();

    Serial.println("MQTT: 消息发送完成");
}

// 设置消息回调
void MQTT_SetMessageCallback(MQTT_MessageCallback callback) {
    messageCallback = callback;
}

// 检查连接状态
bool MQTT_IsConnected() {
    return wifiConnected && mqttConnected;
}

bool MQTT_IsWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

bool MQTT_IsMQTTConnected() {
    return mqttConnected && mqttClient != nullptr && mqttClient->connected();
}

// 获取当前协议
MQTT_Protocol MQTT_GetProtocol() {
    return config != nullptr ? config->protocol : MQTT_PROTOCOL_TCP;
}

// 私有方法实现

// 创建MQTT客户端
static void createMQTTClient() {
    // 清理旧的客户端
    if (mqttClient != nullptr) {
        delete mqttClient;
        mqttClient = nullptr;
    }
    if (wifiClient != nullptr) {
        delete wifiClient;
        wifiClient = nullptr;
    }
    if (wifiClientSecure != nullptr) {
        delete wifiClientSecure;
        wifiClientSecure = nullptr;
    }

    // 根据协议创建相应的客户端
    if (config->protocol == MQTT_PROTOCOL_TCP) {
        wifiClient = new WiFiClient();
        mqttClient = new MqttClient(*wifiClient);
        Serial.println("MQTT: 使用TCP协议");
    } else {
        wifiClientSecure = new WiFiClientSecure();
        mqttClient = new MqttClient(*wifiClientSecure);
        Serial.println("MQTT: 使用SSL协议");
    }
}

// 连接WiFi
static bool connectWiFi() {
    if (config == nullptr) return false;

    Serial.print("MQTT: 正在连接WiFi: ");
    Serial.println(config->wifi_ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(config->wifi_ssid, config->wifi_password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        Serial.print(".");
        delay(500);
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.println("MQTT: WiFi连接成功");
        wifiConnected = true;
        return true;
    } else {
        Serial.println();
        Serial.println("MQTT: WiFi连接失败");
        wifiConnected = false;
        return false;
    }
}

// 连接MQTT服务器
static bool connectMQTT() {
    if (!wifiConnected || config == nullptr || mqttClient == nullptr) {
        return false;
    }

    // 根据协议设置不同的配置
    if (config->protocol == MQTT_PROTOCOL_SSL) {
        // SSL协议设置
        if (wifiClientSecure != nullptr && config->root_ca != nullptr) {
            wifiClientSecure->setCACert(config->root_ca);
        }
    }
    // TCP协议不需要额外设置

    // 设置MQTT客户端参数
    mqttClient->setId(config->mqtt_client_id);
    mqttClient->setUsernamePassword(config->mqtt_username, config->mqtt_password);

    Serial.print("MQTT: 正在连接MQTT服务器: ");
    Serial.print(config->mqtt_broker);
    Serial.print(":");
    Serial.println(config->mqtt_port);

    if (!mqttClient->connect(config->mqtt_broker, config->mqtt_port)) {
        Serial.print("MQTT: MQTT连接失败! 错误代码 = ");
        Serial.println(mqttClient->connectError());
        mqttConnected = false;
        return false;
    }

    Serial.println("MQTT: MQTT连接成功!");
    mqttConnected = true;
    return true;
}

// 订阅主题
static bool subscribe() {
    if (!mqttConnected || mqttClient == nullptr || config == nullptr) {
        return false;
    }

    // 设置消息回调
    mqttClient->onMessage(onMqttMessage);

    Serial.print("MQTT: 正在订阅主题: ");
    Serial.println(config->mqtt_in_topic);

    const int subscribeQos = 1;
    mqttClient->subscribe(config->mqtt_in_topic, subscribeQos);

    Serial.print("MQTT: 等待主题消息: ");
    Serial.println(config->mqtt_in_topic);

    return true;
}

// 处理MQTT消息
static void onMqttMessage(int messageSize) {
    if (mqttClient == nullptr) return;

    Serial.print("MQTT: 收到消息，主题 '");
    Serial.print(mqttClient->messageTopic());
    Serial.print("', 长度 ");
    Serial.print(messageSize);
    Serial.println(" 字节:");

    inputString = "";
    while (mqttClient->available()) {
        inputString += (char)mqttClient->read();
    }

    if (inputString.length() == messageSize) {
        Serial.print("MQTT: 消息内容: ");
        Serial.println(inputString);

        // 调用用户回调函数
        if (messageCallback != nullptr) {
            messageCallback(String(mqttClient->messageTopic()), inputString);
        }

        // 尝试解析JSON
        DynamicJsonDocument json_msg(1024);
        DeserializationError error = deserializeJson(json_msg, inputString);

        if (!error) {
            if (json_msg.containsKey("items")) {
                String items = json_msg["items"];
                Serial.print("MQTT: 解析到items: ");
                Serial.println(items);
            }
        } else {
            Serial.print("MQTT: JSON解析失败: ");
            Serial.println(error.c_str());
        }
    }

    Serial.println();
}

// 处理重连
static void handleReconnection() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastReconnectAttempt < RECONNECT_INTERVAL) {
        return;
    }
    
    lastReconnectAttempt = currentTime;

    // 检查WiFi连接
    if (!MQTT_IsWiFiConnected()) {
        Serial.println("MQTT: WiFi连接断开，尝试重连...");
        connectWiFi();
    }

    // 检查MQTT连接
    if (!MQTT_IsMQTTConnected()) {
        Serial.println("MQTT: MQTT连接断开，尝试重连...");
        if (connectMQTT()) {
            subscribe();
        }
    }
} 