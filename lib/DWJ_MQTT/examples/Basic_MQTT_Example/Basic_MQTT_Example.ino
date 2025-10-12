/**
 * @FilePath     : /DWJ_MQTT/examples/Basic_MQTT_Example/Basic_MQTT_Example.ino
 * @Description  : DWJ_MQTT库基础使用示例 - 支持TCP和SSL协议
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

// 消息计数器
int messageCount = 0;

void setup() {
    // 初始化串口
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
    
    Serial.println("=== DWJ_MQTT 基础示例 ===");
    Serial.print("协议类型: ");
    Serial.println(USE_SSL_PROTOCOL ? "SSL (MQTTs)" : "TCP (MQTT)");
    Serial.println("正在初始化MQTT客户端...");
    
    // 根据协议类型使用不同的配置方法
    if (USE_SSL_PROTOCOL) {
        // 使用SSL协议（需要CA证书）
        MQTT_ConfigSSL(
            "您的WiFi名称",           // WiFi名称
            "您的WiFi密码",           // WiFi密码
            "mqtt服务器地址",         // MQTT服务器地址
            8883,                     // SSL端口
            "ESP32_Basic_SSL",        // 客户端ID
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
            "ESP32_Basic_TCP",        // 客户端ID
            "用户名",                 // MQTT用户名
            "密码",                   // MQTT密码
            "esp32/receive",          // 接收主题
            "esp32/send"              // 发送主题
        );
    }
    
    // 设置消息回调函数
    MQTT_SetMessageCallback([](String topic, String message) {
        Serial.println("=== 收到MQTT消息 ===");
        Serial.print("主题: "); Serial.println(topic);
        Serial.print("消息: "); Serial.println(message);
        Serial.println("=====================");
        
        // 这里可以添加您的消息处理逻辑
        // 例如：解析JSON、控制GPIO等
    });
    
    // 开始连接
    if (MQTT_Begin()) {
        Serial.println("✅ MQTT客户端启动成功!");
        Serial.print("当前协议: ");
        Serial.println(MQTT_GetProtocol() == MQTT_PROTOCOL_TCP ? "TCP" : "SSL");
        Serial.println("📡 现在可以通过串口发送消息");
        Serial.println("💡 输入 'help' 查看可用命令");
    } else {
        Serial.println("❌ MQTT客户端启动失败!");
        Serial.println("请检查配置参数和网络连接");
        while (1) {
            delay(1000);
        }
    }
}

void loop() {
    // 主循环 - 处理MQTT消息和重连
    MQTT_Loop();
    
    // 检查串口输入
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        
        if (input.length() > 0) {
            handleSerialInput(input);
        }
    }
    
    // 定期发送状态消息（每30秒）
    static unsigned long lastStatusTime = 0;
    if (millis() - lastStatusTime > 30000) {
        sendStatusMessage();
        lastStatusTime = millis();
    }
    
    delay(100);
}

// 处理串口输入
void handleSerialInput(String input) {
    if (input.equalsIgnoreCase("help")) {
        showHelp();
    } else if (input.equalsIgnoreCase("status")) {
        showStatus();
    } else if (input.equalsIgnoreCase("test")) {
        sendTestMessage();
    } else if (input.equalsIgnoreCase("json")) {
        sendJsonMessage();
    } else if (input.equalsIgnoreCase("protocol")) {
        showProtocol();
    } else {
        // 发送普通消息
        MQTT_SendMessage(input);
        Serial.println("📤 消息已发送: " + input);
    }
}

// 显示帮助信息
void showHelp() {
    Serial.println("\n=== 可用命令 ===");
    Serial.println("help     - 显示此帮助信息");
    Serial.println("status   - 显示连接状态");
    Serial.println("test     - 发送测试消息");
    Serial.println("json     - 发送JSON格式消息");
    Serial.println("protocol - 显示当前协议信息");
    Serial.println("其他     - 直接发送消息到MQTT服务器");
    Serial.println("================\n");
}

// 显示连接状态
void showStatus() {
    Serial.println("\n=== 连接状态 ===");
    Serial.print("WiFi连接: "); 
    Serial.println(MQTT_IsWiFiConnected() ? "✅ 已连接" : "❌ 未连接");
    Serial.print("MQTT连接: "); 
    Serial.println(MQTT_IsMQTTConnected() ? "✅ 已连接" : "❌ 未连接");
    Serial.print("总体状态: "); 
    Serial.println(MQTT_IsConnected() ? "✅ 正常" : "❌ 异常");
    Serial.println("================\n");
}

// 显示协议信息
void showProtocol() {
    Serial.println("\n=== 协议信息 ===");
    MQTT_Protocol protocol = MQTT_GetProtocol();
    Serial.print("当前协议: ");
    Serial.println(protocol == MQTT_PROTOCOL_TCP ? "TCP (MQTT)" : "SSL (MQTTs)");
    Serial.print("协议特点: ");
    if (protocol == MQTT_PROTOCOL_TCP) {
        Serial.println("无加密，速度快，适合内网");
    } else {
        Serial.println("SSL加密，安全性高，适合公网");
    }
    Serial.println("================\n");
}

// 发送测试消息
void sendTestMessage() {
    messageCount++;
    String testMessage = "测试消息 #" + String(messageCount) + " - 时间: " + String(millis());
    MQTT_SendMessage(testMessage);
    Serial.println("📤 测试消息已发送: " + testMessage);
}

// 发送JSON格式消息
void sendJsonMessage() {
    messageCount++;
    
    // 创建JSON消息
    DynamicJsonDocument doc(256);
    doc["message_id"] = messageCount;
    doc["device"] = "ESP32";
    doc["timestamp"] = millis();
    doc["status"] = "online";
    doc["data"] = "Hello from ESP32!";
    
    String jsonString;
    serializeJson(doc, jsonString);
    
    MQTT_SendMessage(jsonString);
    Serial.println("📤 JSON消息已发送: " + jsonString);
}

// 发送状态消息
void sendStatusMessage() {
    if (!MQTT_IsConnected()) {
        return; // 如果未连接，不发送状态消息
    }
    
    DynamicJsonDocument doc(256);
    doc["type"] = "status";
    doc["device"] = "ESP32";
    doc["uptime"] = millis();
    doc["wifi_rssi"] = WiFi.RSSI();
    doc["free_heap"] = ESP.getFreeHeap();
    doc["message_count"] = messageCount;
    
    String jsonString;
    serializeJson(doc, jsonString);
    
    MQTT_SendMessage(jsonString);
    Serial.println("📊 状态消息已发送");
} 