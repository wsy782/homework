// ex06 双通道反相渐变警车双闪灯效
#define LED1 2
#define LED2 16

int brightness = 0;
int step = 1;

void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
}

void loop() {
  brightness += step;
  // 亮度到达边界反转增减方向
  if (brightness <= 0) {
    step = 1;
  }
  if (brightness >= 255) {
    step = -1;
  }
  // 两路PWM反相输出，一灯渐亮另一灯同步渐暗
  analogWrite(LED1, brightness);
  analogWrite(LED2, 255 - brightness);

  delay(8);
}