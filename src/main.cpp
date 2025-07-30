#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

const char* ssid = "SATURN_CONFIG";
const char* password = "Tuxingkeji.8888";

WebServer server(80);

void handleRoot() {
  File file = SPIFFS.open("/index.html");
  
  if(!file){
    String errorMsg = "Error: Cannot open /index.html\n\nFiles on SPIFFS:\n";
    File root = SPIFFS.open("/");
    File f = root.openNextFile();
    while(f){
      errorMsg += String(f.name()) + " (" + f.size() + " bytes)\n";
      f = root.openNextFile();
    }
    server.send(200, "text/plain", errorMsg);
    return;
  }

  // 直接返回HTML文件内容
  server.streamFile(file, "text/html");
  file.close();
}

void setup() {
  Serial.begin(115200);
  
  // 初始化文件系统
  if(!SPIFFS.begin(true)){
    Serial.println("SPIFFS初始化失败");
    while(1);
  }
  
  // 连接WiFi
  WiFi.begin(ssid, password);
  Serial.print("正在连接WiFi");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n连接成功，IP地址: " + WiFi.localIP().toString());

  // 设置路由
  server.on("/", handleRoot);
  
  // 启动服务器
  server.begin();
  Serial.println("HTTP服务器已启动");
}

void loop() {
  server.handleClient();
}