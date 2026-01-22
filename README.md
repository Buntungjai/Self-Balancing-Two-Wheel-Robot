# Self-Balancing-Two-Wheel-Robot with PID Control & Sensor Fusion

![Self Balancing Demo](images/selfBalancingRobot.gif)

https://youtu.be/95e5Ph7vr44

ก่อนที่จะได้หุ่นยนต์ทรงตัวเองนี้ ได้ทำหุ่นยนต์ต้นแบบมาก่อน <br>
ซึ่งได้แรงบันดาลใจจาก https://circuitdigest.com/microcontroller-projects/arduino-based-self-balancing-robot

โดยหน้าตาหุ่นต้นแบบจะ ทรงตัวไม่ได้และสายไฟ ยังยุ่งเหยิงดังรูปภาพด้านล่าง

<table border="0">
 <tr>
    <td><img src="images/origin1.JPG" width="300px"><br><p align="center">มุมมองด้านหน้า</p></td>
    <td><img src="images/origin2.JPG" width="300px"><br><p align="center">ใช้ปุ่มจูน PID</p></td>
 </tr>
 <tr>
    <td><img src="images/origin3.JPG" width="300px"><br><p align="center">ใช้ LED บอกสถานะ</p></td>
    <td><img src="images/origin4.JPG" width="300px"><br><p align="center">การเดินสายไฟ</p></td>
 </tr>
</table>

ซึ่งอุปกรณ์ที่ใช้คือ <br>
Arduino UNO R3 <br>
มอเตอร์ DC gear motor อัตราทด 1:48 <br>
L298N Motor Driver Module <br>
MPU6050 <br>
ล้อสองข้าง <br>
7.4V Li-ion Battery <br>
สาย Jumper <br>
แผ่นอะคลิลิคแบบไม่สมดุล(จริงๆเป็นถาดวางของ) <br>
step down dc to dc <br>
<br>
สุดท้ายเลยต้อง ออกแบบใหม่
<br>

<h3> มีบางอุปกรณ์ที่เลือกใช้ใหม่ </h3>
โครงทำจาก แผ่นอะคลิริค 10x10 มีประมาณ 6แผ่น โครงสร้างใหม่คำนวณการกระจายน้ำหนักและจุดศูนย์ถ่วง Center of Mass(CoM) เพื่อลดค่า Moment of Inertia (I) ทำให้การจูน PID ทำได้ง่ายขึ้น<br>
การคำนวณ CoM ให้พอเหมาะไม่สูงและไม่ต่ำเกินไป ถ้าสูง >> (ควบคุมง่ายแต่ช้า) ถ้าต่ำ >> (เร็วแต่ล้มไว เหมือนหุ่นยนต์ตัวแรกที่ผมทำเอาแบตเตอรี่ไว้ล่างสุด) แต่ถ้า CoM สูงเกินมอเตอร์ทำงานหนัก CoM ต่ำเกินระบบไว จูน PID ยาก<br>
ลด Moment of Inertia โดยการนำมวลหนักๆเข้าใกล้แกนล้อให้มากที่สุด
มอเตอร์ JGA25-370 DC6V170RPM<br>
ล้อขนาด 6.5 cm (เส้นผ่านศูนย์กลางรวมยางแล้ว)<br>
ไดเวอร์ TB6612FNG<br>
แบตเตอรี่์ li-ion 18650 2S+BMS 7.4V 2600mAh (เหมือนเดิม)<br>
เซนเซอร์ mpu6050 (เหมือนเดิม) <br>
LM2596 แปรงแรงดัน DC to DC<br>

<h3> เหตุผลในการเลือกคือ.. </h3>
เลือกโครงใหม่ ให้กว้างขึ้นเพื่อการทรงตัวที่ง่ายขึ้น<br>
เลือกมอเตอร์ที่ตอบสนองไวขึ้น มี encoder จับระยะได้<br>
เลือกมอเตอร์ไดเวอร์ใหม่ รุ่นที่กินไฟน้อยลง ความร้อนน้อยลง จ่ายกระแสได้ดึขึ้น<br>
ปรับ step down ให้เป็นตัวที่จ่ายกระแสได้ดี<br><br>

<table border="0">
 <tr>
    <td><img src="images/selfB1.JPG" width="300px"><br><p align="center">อะคลิลิค 10x10 </p></td>
    <td><img src="images/selfB2.JPG" width="300px"><br><p align="center">คัตเตอร์กรีดและหัก</p></td>
 </tr>
 <tr>
    <td><img src="images/selfB3.JPG" width="300px"><br><p align="center">จัดตำแหน่งชั้น 1</p></td>
    <td><img src="images/selfB4.JPG" width="300px"><br><p align="center">ยึดน๊อต</p></td>
 </tr>
</table>

<table border="0">
 <tr>
    <td><img src="images/selfB5.JPG" width="300px"><br><p align="center">ตำแหน่งการเจาะรู </p></td>
    <td><img src="images/selfB6.JPG" width="300px"><br><p align="center">ต่อชั้น 2</p></td>
 </tr>
 <tr>
    <td><img src="images/selfB7.JPG" width="300px"><br><p align="center">ต่อชั้น 3</p></td>
    <td><img src="images/selfB8.JPG" width="300px"><br><p align="center">ใส่ล้อและมอเตอร์</p></td>
 </tr>
</table>

<table border="0">
 <tr>
    <td><img src="images/selfB9.JPG" width="300px"><br><p align="center">ทดลองวางอุปกรณ์ </p></td>
    <td><img src="images/selfB10.JPG" width="300px"><br><p align="center">ทำสวิตช์ เปิด ปิด</p></td>
 </tr>
 <tr>
    <td><img src="images/selfB11.JPG" width="300px"><br><p align="center">เดินสายไฟ</p></td>
    <td><img src="images/selfB12.JPG" width="300px"><br><p align="center">ต่อสาย jumper</p></td>
 </tr>
</table>
