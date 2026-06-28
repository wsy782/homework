#define TOUCH_PIN 4
#define LED_PIN 2
#define THRESHOLD 600

bool ledState = false;
// 中断消抖时间戳（必须全局变量，中断函数才能访问）
unsigned long lastTouch = 0;
const unsigned long shakeTime = 350; // 350ms内只识别一次触摸

void gotTouch() {
  unsigned long now = millis();
  // 两次触摸间隔必须大于350ms，才允许翻转
  if (now - lastTouch > shakeTime) {
    lastTouch = now;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  touchAttachInterrupt(TOUCH_PIN, gotTouch, THRESHOLD);
}

void loop() {
  // 串口打印观察数值
  int val = touchRead(TOUCH_PIN);
  Serial.print("Touch Value: ");
  Serial.println(val);
  delay(100);
}