/**
 * @FilePath     : /MQTT_Demo/src/main.cpp
 * @Description  : 极简MQTT演示程序 - 支持TCP和SSL协议
 * @Author       : 董文捷
 * @Version      : 1.0.0
 * @LastEditors  : 董文捷
 * @LastEditTime : 2025-06-18 12:33:28
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2025.
**/

#include <DWJ_MQTT.h>

// 选择协议类型
#define USE_SSL_PROTOCOL true  // true=SSL协议, false=TCP协议

#if USE_SSL_PROTOCOL
// SSL协议配置（需要CA证书）
MQTT_Config config = {
    "Kilig1332232308",           // WiFi名称
    "dwj080900",                 // WiFi密码
    "b00001b0.ala.cn-hangzhou.emqxsl.cn", // MQTT服务器地址
    8883,                        // MQTT端口 (SSL端口)
    "mqttx_da5aa4aa",            // 客户端ID
    "username",                  // MQTT用户名
    "password",                  // MQTT密码
    "Android_Get",               // 接收主题
    "Android_Set",               // 发送主题
    MQTT_PROTOCOL_SSL,           // 协议类型
    R"EOF(
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
)EOF"                          // CA证书
};

#else
// TCP协议配置（不需要CA证书）
MQTT_Config config = {
    "Kilig1332232308",              // WiFi名称
    "dwj080900",              // WiFi密码
    "broker.emqx.io",        // MQTT服务器地址
    1883,                        // MQTT端口 (TCP端口)
    "DWJ",          // 客户端ID
    "",                    // MQTT用户名
    "",                      // MQTT密码
    "DWJ_Get",             // 接收主题
    "DWJ_Set",                // 发送主题
    MQTT_PROTOCOL_TCP,           // 协议类型
    nullptr                      // CA证书 (TCP不需要)
};
#endif

void setup() {
    Serial.begin(115200);
    
    Serial.println("=== MQTT演示程序 ===");
    Serial.print("协议类型: ");
    Serial.println(USE_SSL_PROTOCOL ? "SSL (MQTTs)" : "TCP (MQTT)");
    
    // 初始化MQTT配置
    MQTT_Init(config);
    
    // 设置消息回调
    MQTT_SetMessageCallback([](String topic, String message) {
        Serial.println("=== 收到MQTT消息 ===");
        Serial.print("主题: "); Serial.println(topic);
        Serial.print("消息: "); Serial.println(message);
        Serial.println("==================");
    });
    
    // 开始连接
    if (MQTT_Begin()) {
        Serial.println("MQTT客户端启动成功!");
        Serial.print("当前协议: ");
        Serial.println(MQTT_GetProtocol() == MQTT_PROTOCOL_TCP ? "TCP" : "SSL");
    } else {
        Serial.println("MQTT客户端启动失败!");
    }
}

void loop() {
    // 主循环 - 处理MQTT消息和重连
    MQTT_Loop();
    
    // 检查串口输入并发送消息
    if (Serial.available()) {
        String message = Serial.readStringUntil('\n');
        message.trim();
        if (message.length() > 0) {
            MQTT_SendMessage(message);
        }
    }
    
    delay(100);
}
