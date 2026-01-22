#include <Wire.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <PID_v1.h>

MPU6050 mpu;

// --- MPU6050 Variables ---
bool dmpReady = false;
uint16_t packetSize;
uint8_t fifoBuffer[64];
Quaternion q;
VectorFloat gravity;
float ypr[3];

// --- PID Setup ---
double input, output, setpoint = 180.35; 
double Kp = 37, Ki = 120, Kd = 0.3; 
PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

// --- TB6612FNG Pins (แก้ไขใหม่ทั้งหมด) ---
const int AIN1 = 8;    // ย้ายหนีพิน 5, 6
const int AIN2 = 7;    // ย้ายหนีพิน 5, 6
const int PWMA = 5;    // PWM แท้ (Timer 0)
const int BIN1 = 10;   
const int BIN2 = 9;    
const int PWMB = 6;    // PWM แท้ (Timer 0)
const int STBY = 11;   // ย้ายหนีพิน 7

// --- Encoder Pins (เหมือนเดิม) ---
const int encLeftA = 2;  
const int encLeftB = 4;
const int encRightA = 3; 
const int encRightB = 12;

volatile long leftCount = 0;
volatile long rightCount = 0;

float K_pos = 0.002;

void moveMotors(float speed) {
    if (speed == 0) {
        digitalWrite(AIN1, LOW); digitalWrite(AIN2, LOW);
        digitalWrite(BIN1, LOW); digitalWrite(BIN2, LOW);
    } else if (speed > 0) { // เดินหน้า
        digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH);
        digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH);
    } else { // ถอยหลัง
        digitalWrite(AIN1, HIGH);  digitalWrite(AIN2, LOW);
        digitalWrite(BIN1, HIGH);  digitalWrite(BIN2, LOW);
        speed = abs(speed);
    }
    // ใช้ 5 และ 6 รับรองว่าหมุนสมูททั้งคู่ครับ
    analogWrite(PWMA, constrain(speed, 0, 255));
    analogWrite(PWMB, constrain(speed, 0, 255));
}

// ISR สำหรับ Encoder เหมือนเดิม
void readLeftEncoder() {
    // ถ้า A กับ B เหมือนกันให้ลด ถ้าต่างกันให้เพิ่ม (หรือสลับกันตามการต่อสาย)
    if (digitalRead(encLeftA) == digitalRead(encLeftB)) leftCount++; 
    else leftCount--;
}

void readRightEncoder() {
    // แก้ไขจากเดิมที่สั่งลบอย่างเดียว ให้มีการเช็คเงื่อนไข
    if (digitalRead(encRightA) == digitalRead(encRightB)) rightCount--; 
    else rightCount++;
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(400000); 

    // ตั้งค่าพินมอเตอร์ตามตัวแปรใหม่
    pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT); pinMode(PWMA, OUTPUT);
    pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT); pinMode(PWMB, OUTPUT);
    pinMode(STBY, OUTPUT); digitalWrite(STBY, HIGH); 

    pinMode(encLeftA, INPUT_PULLUP); pinMode(encLeftB, INPUT_PULLUP);
    pinMode(encRightA, INPUT_PULLUP); pinMode(encRightB, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(encLeftA), readLeftEncoder, RISING);
    attachInterrupt(digitalPinToInterrupt(encRightA), readRightEncoder, RISING);

    mpu.initialize();
    uint8_t devStatus = mpu.dmpInitialize();

    mpu.setXAccelOffset(-1855); mpu.setYAccelOffset(1570); mpu.setZAccelOffset(917);
    mpu.setXGyroOffset(85); mpu.setYGyroOffset(-32); mpu.setZGyroOffset(12);

    if (devStatus == 0) {
        mpu.setDMPEnabled(true);
        dmpReady = true;
        packetSize = mpu.dmpGetFIFOPacketSize();
        pid.SetMode(AUTOMATIC);
        pid.SetOutputLimits(-255, 255);
        pid.SetSampleTime(10);
        Serial.println("Ready!");
    }
}

// --- ส่วน Loop สำหรับ Debug ---
void loop() {
    if (!dmpReady) return;

    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

        float currentAngle = ypr[1] * 180 / M_PI + 180;
        
        long distance = (leftCount + rightCount) / 2;
        float posCorrection = distance * K_pos;

        // Input ของ PID คือ มุมปัจจุบัน + ค่าแก้ระยะทางจาก Encoder
        input = currentAngle + posCorrection;

        // ตรวจสอบมุมล้ม (Safety Cut-off)
        if (currentAngle > 150 && currentAngle < 210) {
            pid.Compute();
            moveMotors(output);
        } else {
            moveMotors(0); 
            // Reset ค่าเมื่อหุ่นล้ม เพื่อไม่ให้ Encoder ค้างสะสม
            leftCount = 0; rightCount = 0;
        }

        // --- Serial Debug Monitor ---
        static unsigned long lastPrint = 0;
        if (millis() - lastPrint > 100) {
            lastPrint = millis();
            Serial.print("Ang: ");    Serial.print(currentAngle);
            Serial.print(" | L: ");   Serial.print(leftCount);
            Serial.print(" | R: ");   Serial.print(rightCount);
            Serial.print(" | PosC: "); Serial.print(posCorrection);
            Serial.print(" | Out: ");  Serial.println(output);
        }
    }
}
