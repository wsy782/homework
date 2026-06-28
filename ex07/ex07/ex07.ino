#include <WiFi.h>

// 替换成你自己的WiFi名称、密码
const char* ssid     = "哈哈哈哈哈哈";
const char* password = "wsy951223";

// LED引脚定义
const int LED_PIN = 2;
int pwmVal = 0;

// 网页前端代码
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>无极调光器</title>
</head>
<body style="text-align:center;font-size:22px;">
<h2>ESP32 PWM无极调光</h2>
<input type="range" min="0" max="255" value="%VALUE%" id="slider" oninput="sendVal(this.value)">
<p>当前亮度：<span id="num">%VALUE%</span></p>
<script>
function sendVal(v){
  document.getElementById("num").innerText = v;
  fetch("/set?val="+v);
}
</script>
</body>
)rawliteral";

WiFiServer server(80);

String replaceHtml(String str){
  String res = str;
  res.replace("%VALUE%", String(pwmVal));
  return res;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, pwmVal);

  // 连接WiFi
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功，IP：");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if(client){
    String req = client.readStringUntil('\r');
    client.flush();

    // 解析网页滑动条亮度数值
    if(req.indexOf("/set?val=") != -1){
      int pos = req.indexOf("val=");
      pwmVal = req.substring(pos+4).toInt();
      pwmVal = constrain(pwmVal, 0, 255);
      analogWrite(LED_PIN, pwmVal);
    }

    // 返回网页
    client.print("HTTP/1.1 200 OK\r\nContent-Type:text/html\r\n\r\n");
    client.print(replaceHtml(htmlPage));
    client.stop();
  }
}