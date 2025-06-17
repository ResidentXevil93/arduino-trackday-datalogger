#include <mcp_can.h>
#include <SPI.h>
#include <Wire.h>
#include <MPU6050.h>

// CAN
const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

// IMU
MPU6050 mpu;
float accX, accY, accZ, gyroX, gyroY, gyroZ;

// Analógicos: A1–A5
int analogVals[5];

// Tiempo
unsigned long previousMillis = 0;
const unsigned long interval = 100; // 100ms = 10Hz

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Iniciar MPU
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 no conectado");
  }

  // Iniciar CAN
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("CAN Iniciado");
    CAN.setMode(MCP_NORMAL);
  } else {
    Serial.println("Fallo al iniciar CAN");
    while (1);
  }

  // Cabecera CSV
  Serial.println("Time(s),RPM,Speed(kph),CoolantTemp(C),TPS(%),"
                 "AccX(g),AccY(g),AccZ(g),GyroX(dps),GyroY(dps),GyroZ(dps),"
                 "A1,A2,A3,A4,A5");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Leer sensores MPU6050
    mpu.getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);

    // Convertir a unidades útiles
    float ax = accX / 16384.0;
    float ay = accY / 16384.0;
    float az = accZ / 16384.0;
    float gx = gyroX / 131.0;
    float gy = gyroY / 131.0;
    float gz = gyroZ / 131.0;

    // Leer sensores analógicos A1–A5
    for (int i = 0; i < 5; i++) {
      analogVals[i] = analogRead(A1 + i);
    }

    // Leer datos CAN (simplificado, ejemplo solo)
    int rpm = 0, speed = 0, coolant = 0, tps = 0;
    unsigned char len = 0;
    unsigned char buf[8];

    // RPM (PID 0C)
    CAN.sendMsgBuf(0x7DF, 0, 8, (byte[]){0x02, 0x01, 0x0C, 0, 0, 0, 0, 0});
    if (CAN_MSGAVAIL == CAN.checkReceive()) {
      CAN.readMsgBuf(&len, buf);
      if (buf[2] == 0x0C) rpm = ((buf[3] * 256) + buf[4]) / 4;
    }

    // Speed (PID 0D)
    CAN.sendMsgBuf(0x7DF, 0, 8, (byte[]){0x02, 0x01, 0x0D, 0, 0, 0, 0, 0});
    if (CAN_MSGAVAIL == CAN.checkReceive()) {
      CAN.readMsgBuf(&len, buf);
      if (buf[2] == 0x0D) speed = buf[3];
    }

    // Coolant Temp (PID 05)
    CAN.sendMsgBuf(0x7DF, 0, 8, (byte[]){0x02, 0x01, 0x05, 0, 0, 0, 0, 0});
    if (CAN_MSGAVAIL == CAN.checkReceive()) {
      CAN.readMsgBuf(&len, buf);
      if (buf[2] == 0x05) coolant = buf[3] - 40;
    }

    // Throttle Position (PID 11)
    CAN.sendMsgBuf(0x7DF, 0, 8, (byte[]){0x02, 0x01, 0x11, 0, 0, 0, 0, 0});
    if (CAN_MSGAVAIL == CAN.checkReceive()) {
      CAN.readMsgBuf(&len, buf);
      if (buf[2] == 0x11) tps = (buf[3] * 100) / 255;
    }

    // Salida CSV
    Serial.print(currentMillis / 1000.0, 2); Serial.print(",");
    Serial.print(rpm); Serial.print(",");
    Serial.print(speed); Serial.print(",");
    Serial.print(coolant); Serial.print(",");
    Serial.print(tps); Serial.print(",");
    Serial.print(ax, 2); Serial.print(",");
    Serial.print(ay, 2); Serial.print(",");
    Serial.print(az, 2); Serial.print(",");
    Serial.print(gx, 2); Serial.print(",");
    Serial.print(gy, 2); Serial.print(",");
    Serial.print(gz, 2); Serial.print(",");
    for (int i = 0; i < 5; i++) {
      Serial.print(analogVals[i]);
      if (i < 4) Serial.print(",");
    }
    Serial.println();
  }
}
