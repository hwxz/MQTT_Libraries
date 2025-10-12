/**
 * @FilePath     : /DWJ_MQTT/examples/Simple_Example/Simple_Example.ino
 * @Description  : DWJ_MQTT库最简单使用示例 - 支持TCP和SSL协议
 * @Author       : 董文捷
 * @Version      : 1.0.0
 * @LastEditors  : 董文捷
 * @LastEditTime : 2025-06-18 12:20:00
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2025.
**/

#include <DWJ_MQTT.h>

// 选择协议类型
#define USE_SSL_PROTOCOL true  // true=SSL协议, false=TCP协议

// CA证书（仅SSL协议需要）
const char* CA_CERT = "你的CA证书";

void setup() {
    Serial.begin(115200);
    
    Serial.println("=== 简单MQTT示例 ===");
    Serial.print("协议类型: ");
    Serial.println(USE_SSL_PROTOCOL ? "SSL (MQTTs)" : "TCP (MQTT)");
    
    // 根据协议类型使用不同的配置方法
    if (USE_SSL_PROTOCOL) {
        // 使用SSL协议（需要CA证书）
        MQTT_ConfigSSL(
            "您的WiFi名称",           // WiFi名称
            "您的WiFi密码",           // WiFi密码
            "mqtt服务器地址",         // MQTT服务器地址
            8883,                     // SSL端口
            "ESP32_Simple_SSL",       // 客户端ID
            "用户名",                 // MQTT用户名
            "密码",                   // MQTT密码
            "esp32/receive",          // 接收主题
            "esp32/send",             // 发送主题
            CA_CERT                   // CA证书
        );
    } else {
        // 使用TCP协议（不需要CA证书）
        MQTT_ConfigTCP(
            "您的WiFi名称",           // WiFi名称
            "您的WiFi密码",           // WiFi密码
            "mqtt服务器地址",         // MQTT服务器地址
            1883,                     // TCP端口
            "ESP32_Simple_TCP",       // 客户端ID
            "用户名",                 // MQTT用户名
            "密码",                   // MQTT密码
            "esp32/receive",          // 接收主题
            "esp32/send"              // 发送主题
        );
    }
    
    // 设置消息回调
    MQTT_SetMessageCallback([](String topic, String message) {
        Serial.println("收到消息: " + message);
    });
    
    // 开始连接
    if (MQTT_Begin()) {
        Serial.println("MQTT连接成功!");
    } else {
        Serial.println("MQTT连接失败!");
    }
}

void loop() {
    // 处理MQTT消息
    MQTT_Loop();
    
    // 发送消息
    if (Serial.available()) {
        String message = Serial.readStringUntil('\n');
        message.trim();
        if (message.length() > 0) {
            MQTT_SendMessage(message);
        }
    }
    
    delay(100);
} 