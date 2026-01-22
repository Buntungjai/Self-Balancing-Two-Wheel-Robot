// Arduino sketch that returns calibration offsets for MPU6050
// Version 1.1 (31th January 2014)
// Done by Luis Ródenas <luisrodenaslorda@gmail.com>.
// Based on the I2Cdev library and previous work by Jeff Rowberg.

#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

// กำหนดการตั้งค่า
// ******************************************************
int buffersize=1000;    // จำนวนครั้งที่อ่านเพื่อหาค่าเฉลี่ย
int acel_deadzone=13;    // ยอมให้ค่า Accel Error ได้เท่าไหร่ ค่าเริ่มต้นคือ 8
int giro_deadzone=4;    // ยอมให้ค่า Gyro Error ได้เท่าไหร่ ค่าเริ่มต้นคือ 1
// ******************************************************

MPU6050 accelgyro;

int16_t ax, ay, az, gx, gy, gz;

int mean_ax, mean_ay, mean_az, mean_gx, mean_gy, mean_gz;
int ax_offset, ay_offset, az_offset, gx_offset, gy_offset, gz_offset;

///////////////////////////////////   SETUP   ////////////////////////////////////
void setup() {
  Wire.begin();
  Serial.begin(115200); // ใช้ Baud rate 115200 สำหรับโค้ดนี้ (ระวัง! ต่างจากโค้ดเดิมของคุณ)
  
  Serial.println("Starting MPU6050 calibration...");
  accelgyro.initialize();

  // ตรวจสอบการเชื่อมต่อ
  if (accelgyro.testConnection()) {
    Serial.println("MPU6050 connection successful.");
  } else {
    Serial.println("MPU6050 connection failed!");
    while(1);
  }

  // กำหนด Offset เริ่มต้น (ค่าปัจจุบันของชิป)
  ax_offset = accelgyro.getXAccelOffset();
  ay_offset = accelgyro.getYAccelOffset();
  az_offset = accelgyro.getZAccelOffset();
  gx_offset = accelgyro.getXGyroOffset();
  gy_offset = accelgyro.getYGyroOffset();
  gz_offset = accelgyro.getZGyroOffset();
  
  // เริ่มกระบวนการ Calibrate
  calibrateMPU6050();
}

///////////////////////////////////   LOOP   /////////////////////////////////////
void loop() {
  // เมื่อ Calibrate เสร็จแล้ว ให้ค้างไว้ที่นี่
}


///////////////////////////////////   FUNCTIONS   /////////////////////////////////

void meansensors() {
  long i=0, buff_ax=0, buff_ay=0, buff_az=0, buff_gx=0, buff_gy=0, buff_gz=0;

  while (i < buffersize) {
    // อ่านค่าดิบ
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    // รวมค่า
    buff_ax = buff_ax + ax;
    buff_ay = buff_ay + ay;
    buff_az = buff_az + az;
    buff_gx = buff_gx + gx;
    buff_gy = buff_gy + gy;
    buff_gz = buff_gz + gz;
    
    i++;
    delay(2); // ให้เวลาอ่านค่า
  }
  
  // หาค่าเฉลี่ย
  mean_ax = buff_ax / buffersize;
  mean_ay = buff_ay / buffersize;
  mean_az = buff_az / buffersize;
  mean_gx = buff_gx / buffersize;
  mean_gy = buff_gy / buffersize;
  mean_gz = buff_gz / buffersize;
}

void calibrateMPU6050() {
  int ready = 0;
  int i = 0;
  Serial.println("\nPlace MPU6050 on a flat surface and do not move it.");
  Serial.println("Starting calibration... please wait.");
  
  // ปรับ Gyro Offsets ก่อน
  while (ready != 3) {
    meansensors();
    if (abs(mean_gx) <= giro_deadzone) ready++; else ready = 0;
    gx_offset = gx_offset - mean_gx / giro_deadzone;
    accelgyro.setXGyroOffset(gx_offset);
  }
  ready = 0;
  while (ready != 3) {
    meansensors();
    if (abs(mean_gy) <= giro_deadzone) ready++; else ready = 0;
    gy_offset = gy_offset - mean_gy / giro_deadzone;
    accelgyro.setYGyroOffset(gy_offset);
  }
  ready = 0;
  while (ready != 3) {
    meansensors();
    if (abs(mean_gz) <= giro_deadzone) ready++; else ready = 0;
    gz_offset = gz_offset - mean_gz / giro_deadzone;
    accelgyro.setZGyroOffset(gz_offset);
  }

  // ปรับ Accel Offsets
  ready = 0;
  while (ready != 3) {
    meansensors();
    if (abs(mean_ax) <= acel_deadzone) ready++; else ready = 0;
    ax_offset = ax_offset - mean_ax / acel_deadzone;
    accelgyro.setXAccelOffset(ax_offset);
  }
  ready = 0;
  while (ready != 3) {
    meansensors();
    if (abs(mean_ay) <= acel_deadzone) ready++; else ready = 0;
    ay_offset = ay_offset - mean_ay / acel_deadzone;
    accelgyro.setYAccelOffset(ay_offset);
  }
  ready = 0;
  // *Z-axis ต้องชดเชยแรงโน้มถ่วง 1g (ซึ่งคือ 16384 สำหรับค่าเริ่มต้น +-2g)
  while (ready != 3) { 
    meansensors();
    if (abs(16384 - mean_az) <= acel_deadzone) ready++; else ready = 0;
    az_offset = az_offset + (16384 - mean_az) / acel_deadzone; 
    accelgyro.setZAccelOffset(az_offset);
  }

  // แสดงผลลัพธ์
  Serial.println("\nCalibration complete!");
  Serial.println("Copy these offsets into your main sketch's setup() function:");
  Serial.print("accelgyro.setXAccelOffset("); Serial.print(ax_offset); Serial.println(");");
  Serial.print("accelgyro.setYAccelOffset("); Serial.print(ay_offset); Serial.println(");");
  Serial.print("accelgyro.setZAccelOffset("); Serial.print(az_offset); Serial.println(");");
  Serial.print("accelgyro.setXGyroOffset("); Serial.print(gx_offset); Serial.println(");");
  Serial.print("accelgyro.setYGyroOffset("); Serial.print(gy_offset); Serial.println(");");
  Serial.print("accelgyro.setZGyroOffset("); Serial.print(gz_offset); Serial.println(");");
  
  // แสดงค่า Offset ที่ถูกต้องสำหรับใช้ในการ Calibrate ครั้งต่อไป
  Serial.print("\nRaw offsets (A/G): \t"); 
  Serial.print(ax_offset); Serial.print("\t");
  Serial.print(ay_offset); Serial.print("\t");
  Serial.print(az_offset); Serial.print("\t");
  Serial.print(gx_offset); Serial.print("\t");
  Serial.print(gy_offset); Serial.print("\t");
  Serial.println(gz_offset);
}