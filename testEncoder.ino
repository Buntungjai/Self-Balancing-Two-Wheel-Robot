// --- กำหนดพินตามที่คุณเลือก ---
const int encLeftA = 2;  
const int encLeftB = 4;
const int encRightA = 3; 
const int encRightB = 12;

// ตัวแปรเก็บค่าตำแหน่ง (ใช้ volatile เพราะมีการแก้ไขใน Interrupt)
volatile long leftCount = 0;
volatile long rightCount = 0;

void setup() {
  Serial.begin(115200); // แนะนำความเร็วนี้สำหรับ MPU6050 ในอนาคต
  
  // ตั้งค่าพินเป็น Input
  pinMode(encLeftA, INPUT_PULLUP);
  pinMode(encLeftB, INPUT_PULLUP);
  pinMode(encRightA, INPUT_PULLUP);
  pinMode(encRightB, INPUT_PULLUP);

  // ตั้งค่า Interrupt สำหรับขา A ของทั้งสองข้าง
  attachInterrupt(digitalPinToInterrupt(encLeftA), readLeftEncoder, RISING);
  attachInterrupt(digitalPinToInterrupt(encRightA), readRightEncoder, RISING);

  Serial.println("--- Dual Encoder Test Ready ---");
  Serial.println("Rotate the wheels by hand to see counts...");
}

void loop() {
  // แสดงผลค่า Encoder ทุกๆ 100ms
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 100) {
    lastTime = millis();
    
    Serial.print("Left: ");
    Serial.print(leftCount);
    Serial.print("\t | Right: ");
    Serial.println(rightCount);
  }
}

// ฟังก์ชัน Interrupt สำหรับล้อซ้าย
void readLeftEncoder() {
  // เช็คทิศทาง: ถ้า A และ B สถานะเหมือนกัน = หมุนไปทางหนึ่ง, ถ้าต่างกัน = อีกทางหนึ่ง
  if (digitalRead(encLeftA) == digitalRead(encLeftB)) {
    leftCount--;
  } else {
    leftCount++;
  }
}

// ฟังก์ชัน Interrupt สำหรับล้อขวา
void readRightEncoder() {
  if (digitalRead(encRightA) == digitalRead(encRightB)) {
    rightCount++;
  } else {
    rightCount--;
  }
}