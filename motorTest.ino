// --- TB6612FNG Pins ---
const int AIN1 = 5;  // ทิศทางมอเตอร์ A
const int AIN2 = 6;
const int PWMA = 3;  // ความเร็ว (ต้องเป็นพิน PWM)
const int BIN1 = 9;  // ทิศทางมอเตอร์ B
const int BIN2 = 10;
const int PWMB = 11; // ความเร็ว (ต้องเป็นพิน PWM)
const int STBY = 7;  // พิน Standby

void setup() {
    Serial.begin(115200);
    
    // ตั้งค่าพินมอเตอร์เป็น OUTPUT
    pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT); pinMode(PWMA, OUTPUT);
    pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT); pinMode(PWMB, OUTPUT);
    pinMode(STBY, OUTPUT);

    // เปิดใช้งาน Driver (ต้องเป็น HIGH มอเตอร์ถึงจะทำงาน)
    digitalWrite(STBY, HIGH); 
    
    Serial.println("Motor Test Starting...");
}

void moveMotors(int speed) {
    if (speed > 0) { // เดินหน้า
        digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW);
        digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);
        Serial.print("Forward at speed: ");
    } else if (speed < 0) { // ถอยหลัง
        digitalWrite(AIN1, LOW);  digitalWrite(AIN2, HIGH);
        digitalWrite(BIN1, LOW);  digitalWrite(BIN2, HIGH);
        Serial.print("Backward at speed: ");
    } else { // หยุด
        digitalWrite(AIN1, LOW);  digitalWrite(AIN2, LOW);
        digitalWrite(BIN1, LOW);  digitalWrite(BIN2, LOW);
        Serial.println("Stop");
    }
    
    analogWrite(PWMA, abs(speed));
    analogWrite(PWMB, abs(speed));
    if (speed != 0) Serial.println(abs(speed));
}

void loop() {
    // 1. เดินหน้าความเร็วปานกลาง
    moveMotors(150); 
    delay(2000);

    // 2. หยุด
    moveMotors(0);
    delay(1000);

    // 3. ถอยหลังความเร็วปานกลาง
    moveMotors(-150);
    delay(2000);

    // 4. หยุด
    moveMotors(0);
    delay(1000);
}