// ex03 millis无阻塞SOS求救灯 标准三短三长三短
const int ledPin = 2;
unsigned long timer = 0;
int state = 0;

// 时序常量
const unsigned short ON_SHORT = 200;
const unsigned short ON_LONG = 600;
const unsigned short GAP_SAME = 200;
const unsigned short GAP_LETTER = 500;
const unsigned short GAP_LOOP = 2000;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  unsigned long now = millis();
  switch (state) {
    // ========== 第一组：S 3次短闪 ==========
    case 0: // 短亮
      digitalWrite(ledPin, HIGH);
      timer = now;
      state = 1;
      break;
    case 1: // 短亮计时结束，熄灭
      if (now - timer >= ON_SHORT) {
        digitalWrite(ledPin, LOW);
        timer = now;
        state = 2;
      }
      break;
    case 2: // 单次闪烁间隔
      if (now - timer >= GAP_SAME) {
        static int cntS1 = 0;
        cntS1++;
        if (cntS1 >= 3) {
          cntS1 = 0;
          state = 3; // 进入S与O间隔
        } else {
          state = 0; // 再来一次短闪
        }
      }
      break;

    // S-O字母间隔
    case 3:
      if (now - timer >= GAP_LETTER) {
        state = 4;
      }
      break;

    // ========== 第二组：O 3次长闪 ==========
    case 4: // 长亮
      digitalWrite(ledPin, HIGH);
      timer = now;
      state = 5;
      break;
    case 5: // 长亮计时结束，熄灭
      if (now - timer >= ON_LONG) {
        digitalWrite(ledPin, LOW);
        timer = now;
        state = 6;
      }
      break;
    case 6: // 单次长闪间隔
      if (now - timer >= GAP_SAME) {
        static int cntO = 0;
        cntO++;
        if (cntO >= 3) {
          cntO = 0;
          state = 7; // O与S间隔
        } else {
          state = 4; // 再来一次长闪
        }
      }
      break;

    // O-S字母间隔
    case 7:
      if (now - timer >= GAP_LETTER) {
        state = 8;
      }
      break;

    // ========== 第三组：S 3次短闪 ==========
    case 8: // 短亮
      digitalWrite(ledPin, HIGH);
      timer = now;
      state = 9;
      break;
    case 9: // 短亮计时结束，熄灭
      if (now - timer >= ON_SHORT) {
        digitalWrite(ledPin, LOW);
        timer = now;
        state = 10;
      }
      break;
    case 10: // 单次短闪间隔
      if (now - timer >= GAP_SAME) {
        static int cntS2 = 0;
        cntS2++;
        if (cntS2 >= 3) {
          cntS2 = 0;
          state = 11; // 整套SOS结束长停顿
        } else {
          state = 8; // 再来一次短闪
        }
      }
      break;

    // 一轮完整SOS结束，长时间熄灭后循环
    case 11:
      if (now - timer >= GAP_LOOP) {
        state = 0; // 从头重新循环SOS
      }
      break;
  }
}