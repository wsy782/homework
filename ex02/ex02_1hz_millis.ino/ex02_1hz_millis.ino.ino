// ex02 使用millis实现1Hz无阻塞LED闪烁，禁止delay阻塞程序
const int ledPin = 2;
unsigned long prevTime = 0;
// 亮500ms、灭500ms，完整周期1000ms=1Hz
const unsigned long interval = 500;
bool ledStatus = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currentTime = millis();
  // 判断计时到达切换时刻
  if (currentTime - prevTime >= interval) {
    prevTime = currentTime;
    ledStatus = !ledStatus;
    digitalWrite(ledPin, ledStatus);
    Serial.print("LED状态切换：");
    Serial.println(ledStatus ? "ON 亮" : "OFF 灭");
  }
}