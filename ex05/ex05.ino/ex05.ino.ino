// ========== 硬件引脚定义 ==========
#define TOUCH_PIN     4    // 触摸引脚T0
#define LED_PIN       2    // LED引脚
#define THRESHOLD     600  // 适配你的触摸数值

// ========== 触摸防抖、边沿检测变量 ==========
bool lastTouchState = false;
unsigned long lastTouchTime = 0;
const unsigned long DEBOUNCE_DELAY = 300;

// ========== 呼吸灯档位控制 ==========
int speedLevel = 1;
int breatheDelay[3] = {40, 18, 6}; // 1慢 2中 3快

// ========== PWM呼吸灯变量 ==========
int brightness = 0;
int step = 1;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("ex05 触摸多档位呼吸灯启动");
  Serial.println("档位1：慢速 | 档位2：中速 | 档位3：快速");
}

void loop() {
  // 1. 读取触摸原始值
  int touchVal = touchRead(TOUCH_PIN);
  Serial.print("Touch:");
  Serial.print(touchVal);
  Serial.print(" Level:");
  Serial.println(speedLevel);

  // 2. 触摸边沿检测+防抖，触摸一次切换一档
  bool nowTouch = (touchVal < THRESHOLD);
  unsigned long now = millis();

  if (nowTouch && !lastTouchState && (now - lastTouchTime > DEBOUNCE_DELAY))
  {
    lastTouchTime = now;
    speedLevel += 1;
    if(speedLevel > 3) speedLevel = 1;
    Serial.print("=== 切换档位至：");
    Serial.println(speedLevel);
  }
  lastTouchState = nowTouch;

  // 3. 模拟PWM呼吸，不用ledc函数，无编译报错
  brightness += step;
  if(brightness <= 0) step = 1;
  if(brightness >= 255) step = -1;
  
  analogWrite(LED_PIN, brightness);

  // 当前档位控制呼吸快慢
  delay(breatheDelay[speedLevel - 1]);
}