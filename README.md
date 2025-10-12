# DWJ_MQTT 库

ESP32 MQTT客户端封装库，支持TCP和SSL两种协议，提供简单易用的MQTT通信接口。

## 功能特性

- 支持TCP和SSL两种协议
- 自动WiFi连接管理
- 自动MQTT连接和重连
- 消息发送和接收
- 支持回调函数处理接收到的消息
- 自动JSON解析
- 极简的API设计

## 协议选择

### TCP协议 (MQTT)
- 端口：1883
- 特点：无加密，速度快，适合内网使用
- 不需要CA证书

### SSL协议 (MQTTs)
- 端口：8883
- 特点：SSL加密，安全性高，适合公网使用
- 需要CA证书

## 安装

1. 下载此库
2. 解压到Arduino的libraries文件夹
3. 重启Arduino IDE

## 快速开始

### 方法1：使用便捷配置函数

```cpp
#include <DWJ_MQTT.h>

// TCP协议配置（推荐内网使用）
MQTT_ConfigTCP(
    "您的WiFi名称",
    "您的WiFi密码", 
    "mqtt服务器地址",
    1883,                    // TCP端口
    "ESP32_Client",
    "用户名",
    "密码",
    "esp32/receive",
    "esp32/send"
);

// 或者SSL协议配置（推荐公网使用）
MQTT_ConfigSSL(
    "您的WiFi名称",
    "您的WiFi密码",
    "mqtt服务器地址", 
    8883,                    // SSL端口
    "ESP32_Client",
    "用户名",
    "密码",
    "esp32/receive",
    "esp32/send",
    "您的CA证书"
);

void setup() {
    Serial.begin(115200);
    MQTT_SetMessageCallback([](String topic, String message) {
        Serial.println("收到消息: " + message);
    });
    MQTT_Begin();
}

void loop() {
    MQTT_Loop();
    if (Serial.available()) {
        String msg = Serial.readStringUntil('\n');
        MQTT_SendMessage(msg);
    }
}
```

### 方法2：使用配置结构体

```cpp
#include <DWJ_MQTT.h>

// 选择协议类型
#define USE_SSL_PROTOCOL true  // true=SSL协议, false=TCP协议

#if USE_SSL_PROTOCOL
// SSL协议配置
MQTT_Config config = {
    "您的WiFi名称",           // wifi_ssid
    "您的WiFi密码",           // wifi_password
    "mqtt服务器地址",         // mqtt_broker
    8883,                     // mqtt_port (SSL端口)
    "ESP32_Client",           // mqtt_client_id
    "用户名",                 // mqtt_username
    "密码",                   // mqtt_password
    "esp32/receive",          // mqtt_in_topic
    "esp32/send",             // mqtt_out_topic
    MQTT_PROTOCOL_SSL,        // protocol
    "您的CA证书"              // root_ca
};
#else
// TCP协议配置
MQTT_Config config = {
    "您的WiFi名称",           // wifi_ssid
    "您的WiFi密码",           // wifi_password
    "mqtt服务器地址",         // mqtt_broker
    1883,                     // mqtt_port (TCP端口)
    "ESP32_Client",           // mqtt_client_id
    "用户名",                 // mqtt_username
    "密码",                   // mqtt_password
    "esp32/receive",          // mqtt_in_topic
    "esp32/send",             // mqtt_out_topic
    MQTT_PROTOCOL_TCP,        // protocol
    nullptr                   // root_ca (TCP不需要)
};
#endif

void setup() {
    Serial.begin(115200);
    MQTT_Init(config);
    MQTT_SetMessageCallback([](String topic, String message) {
        Serial.println("收到消息: " + message);
    });
    MQTT_Begin();
}

void loop() {
    MQTT_Loop();
    if (Serial.available()) {
        String msg = Serial.readStringUntil('\n');
        MQTT_SendMessage(msg);
    }
}
```

## API 参考

### 配置结构
```cpp
struct MQTT_Config {
    const char* wifi_ssid;
    const char* wifi_password;
    const char* mqtt_broker;
    int mqtt_port;
    const char* mqtt_client_id;
    const char* mqtt_username;
    const char* mqtt_password;
    const char* mqtt_in_topic;
    const char* mqtt_out_topic;
    MQTT_Protocol protocol;
    const char* root_ca;      // 仅SSL协议需要
};
```

### 协议枚举
```cpp
enum MQTT_Protocol {
    MQTT_PROTOCOL_TCP = 0,    // TCP协议（无SSL）
    MQTT_PROTOCOL_SSL = 1     // SSL协议（MQTTs）
};
```

### 主要方法
- `MQTT_Init(Config config)` - 初始化配置
- `MQTT_Begin()` - 开始连接
- `MQTT_Loop()` - 主循环（必须定期调用）
- `MQTT_SendMessage(String message)` - 发送消息
- `MQTT_SetMessageCallback(callback)` - 设置消息回调
- `MQTT_IsConnected()` - 检查连接状态
- `MQTT_IsWiFiConnected()` - 检查WiFi连接状态
- `MQTT_IsMQTTConnected()` - 检查MQTT连接状态
- `MQTT_GetProtocol()` - 获取当前协议

### 便捷配置函数
- `MQTT_ConfigTCP(...)` - TCP协议快速配置
- `MQTT_ConfigSSL(...)` - SSL协议快速配置

## 依赖

- ArduinoJson
- ArduinoMqttClient
- ESP32开发板

## 许可证

MIT License 