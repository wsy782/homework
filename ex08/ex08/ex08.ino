#include <WiFi.h>

// 替换成你自己WiFi名称、密码
const char* ssid = "哈哈哈哈哈哈";
const char* password = "wsy951223";

const int LED_PIN = 2;
const int TOUCH_CH = T0; // GPIO4触摸通道

bool armState = false;
bool alarmFlag = false;

// 触摸滑动滤波
#define TOUCH_SAMPLE_NUM 5
int touchBuf[TOUCH_SAMPLE_NUM];
int bufIndex = 0;
int baseTouchVal = 0; // 上电空载基准

// 网页界面
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>ESP32安防报警器</title>
<style>
body{text-align:center;font-size:24px;margin-top:40px;}
button{font-size:22px;padding:12px 30px;margin:15px;}
#status{font-weight:bold;color:#d62828;}
</style>
</head>
<body>
<h2>物联网安防报警器</h2>
<button onclick="sendCmd('arm')">布防 Arm</button>
<button onclick="sendCmd('disarm')">撤防 Disarm</button>
<p>当前系统状态：<span id="status">%STATUS%</span></p>

<script>
function sendCmd(cmd){
  fetch("/cmd?act="+cmd).then(updateStatus);
}
function updateStatus(){
  fetch("/state").then(res=>res.text()).then(text=>{
    document.getElementById("status").innerText = text;
  })
}
setInterval(updateStatus, 500);
</script>
</body>
)rawliteral";

WiFiServer server(80);

String getHtml(){
  String page = htmlPage;
  if(armState){
    page.replace("%STATUS%", "已布防，触摸触发报警");
  }else{
    page.replace("%STATUS%", "未布防，触摸无响应");
  }
  return page;
}

// 均值滤波读取触摸值
int getFilterTouch()
{
  int raw = touchRead(TOUCH_CH);
  touchBuf[bufIndex] = raw;
  bufIndex = (bufIndex + 1) % TOUCH_SAMPLE_NUM;
  int sum = 0;
  for(int i=0; i<TOUCH_SAMPLE_NUM; i++) sum += touchBuf[i];
  return sum / TOUCH_SAMPLE_NUM;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // 上电校准空载基准
  delay(1000);
  int total = 0;
  for(int i=0; i<10; i++){
    total += touchRead(TOUCH_CH);
    delay(50);
  }
  baseTouchVal = total / 10;

  // 连接WiFi
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
  // 网页请求处理
  WiFiClient client = server.available();
  if(client){
    String req = client.readStringUntil('\r');
    client.flush();

    if(req.indexOf("/cmd?act=arm") != -1){
      armState = true;
    }else if(req.indexOf("/cmd?act=disarm") != -1){
      // 撤防全部重置
      armState = false;
      alarmFlag = false;
      digitalWrite(LED_PIN, LOW);
    }
    else if(req.indexOf("/state") != -1){
      client.print("HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\n\r\n");
      if(armState) client.print("已布防，触摸触发报警");
      else client.print("未布防，触摸无响应");
      client.stop();
      return;
    }

    client.print("HTTP/1.1 200 OK\r\nContent-Type:text/html\r\n\r\n");
    client.print(getHtml());
    client.stop();
  }

  int touchAvg = getFilterTouch();
  // 动态阈值：数值下降25%判定触摸
  bool isTouched = (touchAvg < baseTouchVal * 0.75);

  // 仅布防、无报警时触发锁定
  if(armState && !alarmFlag && isTouched)
  {
    alarmFlag = true;
  }

  // LED非阻塞闪烁
  unsigned long now = millis();
  static unsigned long lastFlash = 0;
  if(alarmFlag)
  {
    if(now - lastFlash > 100)
    {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      lastFlash = now;
    }
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
  }

  delay(20);
}