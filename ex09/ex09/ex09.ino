#include <WiFi.h>

// 改成你自己的WiFi名称、密码
const char* ssid = "哈哈哈哈哈哈";
const char* password = "wsy951223";

const int TOUCH_CH = T0; // GPIO4触摸引脚
#define SAMPLE_COUNT 2  // 减少采样数量，提升近距离感应灵敏度
int touchBuffer[SAMPLE_COUNT];
int bufIdx = 0;

WiFiServer server(80);

// 轻度平滑滤波，近距离就能识别变化
int getSmoothTouch()
{
  int raw = touchRead(TOUCH_CH);
  touchBuffer[bufIdx] = raw;
  bufIdx = (bufIdx + 1) % SAMPLE_COUNT;
  int sum = 0;
  for(int i = 0; i < SAMPLE_COUNT; i++){
    sum += touchBuffer[i];
  }
  return sum / SAMPLE_COUNT;
}

// Web仪表盘页面 AJAX实时刷新
const char dashHtml[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>实时触摸仪表盘</title>
<style>
body{
  text-align:center;
  background:#111;
  color:#fff;
  font-family:Arial;
  margin-top:80px;
}
h2{font-size:32px;}
#dataNum{
  font-size:72px;
  color:#0cf;
  font-weight:bold;
  margin:30px 0;
}
.tip{font-size:18px;color:#aaa;}
</style>
</head>
<body>
<h2>ESP32 触摸实时仪表盘</h2>
<div id="dataNum">0</div>
<p class="tip">手指靠近GPIO4引脚 数字变小；移开恢复</p>

<script>
function refreshData(){
  fetch("/touchVal")
  .then(res=>res.text())
  .then(val=>{
    document.getElementById("dataNum").innerText = val;
  })
}
setInterval(refreshData, 150);
</script>
</body>
)rawliteral";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("WiFi连接中");
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

    if(req.indexOf("/touchVal") != -1){
      int val = getSmoothTouch();
      client.print("HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\n\r\n");
      client.print(val);
      client.stop();
      return;
    }

    client.print("HTTP/1.1 200 OK\r\nContent-Type:text/html\r\n\r\n");
    client.print(dashHtml);
    client.stop();
  }
  delay(10);
}