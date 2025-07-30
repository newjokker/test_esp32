
#include <WiFi.h>
#include <WebServer.h> 
WebServer server(80); 

const char* ssid = "SATURN_CONFIG";  
const char* password = "Tuxingkeji.8888"; 

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>ESP32-S3 控制面板</title>
    <style>
      body { font-family: Arial; text-align: center; margin: 0 auto; padding: 20px; }
      .button {
        padding: 10px 20px;
        font-size: 16px;
        margin: 10px;
        cursor: pointer;
        background-color: #4CAF50;
        color: white;
        border: none;
        border-radius: 5px;
      }
      .button-off { background-color: #f44336; }
      .status { font-size: 18px; margin-top: 20px; }
    </style>
  </head>
  <body>
    <h1>ESP32-S3 控制面板</h1>
    <p>欢迎使用ESP32-S3 Web控制界面</p>
    
    <div>
      <button class="button" onclick="controlLED('on')">打开 LED</button>
      <button class="button button-off" onclick="controlLED('off')">关闭 LED</button>
    </div>
    
    <div class="status" id="ledStatus">LED 状态: 未知</div>
    
    <script>
      function controlLED(state) {
        fetch('/led?state=' + state)
          .then(response => response.text())
          .then(data => {
            document.getElementById('ledStatus').innerText = 'LED 状态: ' + data;
          });
      }
      
      // 初始加载时获取LED状态
      window.onload = function() {
        fetch('/ledStatus')
          .then(response => response.text())
          .then(data => {
            document.getElementById('ledStatus').innerText = 'LED 状态: ' + data;
          });
      };
    </script>
  </body>
  </html>
  )rawliteral";
  
  server.send(200, "text/html; charset=UTF-8", html);
}

void handleLED() {
  if (server.arg("state") == "on") {
    digitalWrite(LED_BUILTIN, HIGH);
    server.send(200, "text/plain", "ON");
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    server.send(200, "text/plain", "OFF");
  }
}

void handleLEDStatus() {
  if (digitalRead(LED_BUILTIN)) {
    server.send(200, "text/plain", "ON");
  } else {
    server.send(200, "text/plain", "OFF");
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // 初始关闭LED
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi连接成功!");
  Serial.print("IP地址: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);                   // 绑定根路径
  server.on("/led", handleLED);                 // 绑定LED控制路径
  server.on("/ledStatus", handleLEDStatus);     // 新增：获取LED状态
  server.begin();                               // 启动服务器
}

void loop() {
  server.handleClient(); // 处理客户端请求
}

