# DWJ_MQTT 示例

本文件夹包含了DWJ_MQTT库的使用示例，从简单到复杂，帮助您快速上手。**所有示例都支持TCP和SSL两种协议**。

## 📁 示例列表

### 1. Simple_Example
**最简单的使用示例**
- 文件：`Simple_Example/Simple_Example.ino`
- 描述：最基础的MQTT连接和消息收发
- 协议支持：TCP和SSL
- 适用：快速了解基本用法

### 2. Basic_MQTT_Example
**完整功能示例**
- 文件：`Basic_MQTT_Example/Basic_MQTT_Example.ino`
- 描述：包含完整功能的MQTT客户端
- 协议支持：TCP和SSL
- 功能：
  - 串口命令交互
  - JSON消息发送
  - 状态监控
  - 自动重连
  - 协议信息显示
- 适用：学习完整功能

### 3. Protocol_Example
**协议选择示例**
- 文件：`Protocol_Example/Protocol_Example.ino`
- 描述：专门演示TCP和SSL协议的选择和使用
- 协议支持：TCP和SSL
- 功能：
  - 协议切换演示
  - 协议特点说明
  - 完整交互功能
- 适用：了解协议差异

### 4. PlatformIO_Example
**PlatformIO项目示例**
- 文件：`PlatformIO_Example/src/main.cpp`
- 配置文件：`PlatformIO_Example/platformio.ini`
- 描述：PlatformIO环境下的完整项目
- 协议支持：TCP和SSL
- 适用：使用PlatformIO开发

## 🚀 快速开始

### Arduino IDE
1. 打开Arduino IDE
2. 选择 `文件 -> 打开`
3. 选择任意示例文件夹中的 `.ino` 文件
4. 修改配置参数和协议选择
5. 编译并上传

### PlatformIO
1. 打开PlatformIO
2. 导入 `PlatformIO_Example` 文件夹
3. 修改 `src/main.cpp` 中的配置参数和协议选择
4. 编译并上传

## ⚙️ 协议选择

所有示例都支持两种协议，通过修改 `USE_SSL_PROTOCOL` 宏来选择：

```cpp
// 选择协议类型
#define USE_SSL_PROTOCOL true  // true=SSL协议, false=TCP协议
```

### TCP协议（推荐内网使用）
- 端口：1883
- 特点：无加密，速度快，适合内网
- 配置：不需要CA证书

### SSL协议（推荐公网使用）
- 端口：8883
- 特点：SSL加密，安全性高，适合公网
- 配置：需要CA证书

## ⚙️ 配置说明

### 使用便捷配置函数（推荐）

```cpp
// SSL协议配置
MQTT_ConfigSSL(
    "您的WiFi名称",           // WiFi名称
    "您的WiFi密码",           // WiFi密码
    "mqtt服务器地址",         // MQTT服务器地址
    8883,                     // SSL端口
    "ESP32_Client",           // 客户端ID
    "用户名",                 // MQTT用户名
    "密码",                   // MQTT密码
    "esp32/receive",          // 接收主题
    "esp32/send",             // 发送主题
    CA_CERT                   // CA证书
);

// TCP协议配置
MQTT_ConfigTCP(
    "您的WiFi名称",           // WiFi名称
    "您的WiFi密码",           // WiFi密码
    "mqtt服务器地址",         // MQTT服务器地址
    1883,                     // TCP端口
    "ESP32_Client",           // 客户端ID
    "用户名",                 // MQTT用户名
    "密码",                   // MQTT密码
    "esp32/receive",          // 接收主题
    "esp32/send"              // 发送主题
);
```

### 传统配置方式

```cpp
MQTT_Config config = {
    .wifi_ssid = "您的WiFi名称",           // 必填
    .wifi_password = "您的WiFi密码",       // 必填
    .mqtt_broker = "mqtt服务器地址",       // 必填
    .mqtt_port = 8883,                    // 必填
    .mqtt_client_id = "ESP32_Client",     // 必填
    .mqtt_username = "用户名",            // 必填
    .mqtt_password = "密码",              // 必填
    .mqtt_in_topic = "esp32/receive",     // 必填
    .mqtt_out_topic = "esp32/send",       // 必填
    .protocol = MQTT_PROTOCOL_SSL,        // 协议类型
    .root_ca = "您的CA证书"               // SSL连接时必填
};
```

## 📋 串口命令

在支持交互的示例中，您可以通过串口发送以下命令：

- `help` - 显示帮助信息
- `status` - 显示连接状态
- `test` - 发送测试消息
- `json` - 发送JSON格式消息
- `protocol` - 显示当前协议信息
- 其他任意文本 - 直接发送到MQTT服务器

## 🔧 依赖库

确保已安装以下库：
- ArduinoJson
- ArduinoMqttClient

## 📝 注意事项

1. **协议选择**: 根据使用环境选择合适的协议
2. **WiFi配置**: 确保WiFi名称和密码正确
3. **MQTT服务器**: 确保服务器地址和端口正确
4. **SSL证书**: 如果使用SSL连接，需要正确的CA证书
5. **主题设置**: 确保发送和接收主题配置正确
6. **权限设置**: 确保MQTT用户名和密码有相应权限

## 🎯 示例特点

- **Simple_Example**: 最简洁，适合快速测试
- **Basic_MQTT_Example**: 功能完整，适合学习
- **Protocol_Example**: 协议演示，适合了解协议差异
- **PlatformIO_Example**: 项目化，适合实际开发

## 🔄 协议切换

要切换协议，只需修改示例开头的宏定义：

```cpp
// 使用SSL协议
#define USE_SSL_PROTOCOL true

// 使用TCP协议
#define USE_SSL_PROTOCOL false
```

选择适合您需求的示例和协议开始使用！ 