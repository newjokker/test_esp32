#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

WebServer server(80);

const char* ssid = "SATURN_CONFIG";
const char* password = "Tuxingkeji.8888";

void debugFS() {
  if(!SPIFFS.begin(true)){
    Serial.println("格式化SPIFFS...");
    SPIFFS.format();
  }
  
  File root = SPIFFS.open("/");
  Serial.println("强制列出所有文件：");
  File file = root.openNextFile();
  while(file){
    Serial.printf(">> 文件名: %s\n", file.name());
    Serial.printf(">> 大小: %d bytes\n", file.size());
    
    // 打印文件前16字节内容（HEX格式）
    uint8_t buf[16];
    size_t len = file.read(buf, sizeof(buf));
    Serial.print(">> 头部: ");
    for(size_t i=0; i<len; i++){
      Serial.printf("%02X ", buf[i]);
    }
    Serial.println("\n------------------------");
    file.close();
    file = root.openNextFile();
  }
}

void setup() {

  Serial.begin(115200);

  Serial.println("开始文件系统诊断...");
  debugFS();

  // 初始化 SPIFFS（带详细日志）
  Serial.println("正在初始化SPIFFS...");
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS挂载失败！可能原因：");
    Serial.println("1. 文件系统未上传");
    Serial.println("2. 文件系统损坏");
    Serial.println("3. 分区表配置错误");
    while (1);
  }
  
  Serial.println("SPIFFS挂载成功！");
  Serial.println("文件系统内容：");
  
  // 列出SPIFFS中的所有文件
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  bool foundIndex = false;
  
  while(file){
    Serial.printf("找到文件: /%s (大小: %d字节)\n", file.name(), file.size());
    if(String(file.name()) == "index.html") {
      foundIndex = true;
    }
    file = root.openNextFile();
  }
  
  if(!foundIndex) {
    Serial.println("警告：未找到index.html文件！");
  } else {
    Serial.println("已找到index.html文件");
  }

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // 连接 WiFi
  WiFi.begin(ssid, password);
  Serial.print("正在连接WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功");
  Serial.print("IP地址: ");
  Serial.println(WiFi.localIP());

  // 路由设置
  server.on("/", HTTP_GET, []() {
    Serial.println("接收到根路径请求");
    File file = SPIFFS.open("index.html");
    if (!file) {
      Serial.println("错误：无法打开index.html");
      server.send(500, "text/plain", "Failed to load HTML file");
      return;
    }
    Serial.println("正在发送index.html文件");
    server.streamFile(file, "text/html");
    file.close();
  });

  server.on("/led", HTTP_GET, []() {
    String state = server.arg("state");
    Serial.printf("接收到LED控制请求: state=%s\n", state.c_str());
    if (state == "on") {
      digitalWrite(LED_BUILTIN, HIGH);
      server.send(200, "text/plain", "ON");
    } else {
      digitalWrite(LED_BUILTIN, LOW);
      server.send(200, "text/plain", "OFF");
    }
  });

  server.on("/ledStatus", HTTP_GET, []() {
    String status = digitalRead(LED_BUILTIN) ? "ON" : "OFF";
    Serial.printf("LED状态查询: %s\n", status);
    server.send(200, "text/plain", status);
  });

  server.on("/systemInfo", HTTP_GET, []() {
    Serial.println("系统信息查询");
    String json = "{";
    json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
    json += "\"rssi\":\"" + String(WiFi.RSSI()) + "\"";
    json += "}";
    server.send(200, "application/json", json);
  });

  // 启动服务器
  server.begin();
  Serial.println("HTTP服务器已启动");
}

void loop() {
  server.handleClient();
}