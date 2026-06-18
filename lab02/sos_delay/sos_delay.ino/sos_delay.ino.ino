// lab02 delay阻塞实现SOS灯光
const int ledPin = 2;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // S：短闪3次
  for(int i=0; i<3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
  delay(500); // 字母间隔

  // O：长闪3次
  for(int i=0; i<3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(600);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
  delay(500);

  // S：短闪3次
  for(int i=0; i<3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
  delay(2000); // 整套SOS结束长停顿
}