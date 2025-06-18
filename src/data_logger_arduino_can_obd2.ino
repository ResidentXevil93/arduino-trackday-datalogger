#include <CAN.h>
#include <OBD2.h>
#include <Wire.h>
#include <MPU6050.h>

// IMU
MPU6050 mpu;
float accX, accY, accZ, gyroX, gyroY, gyroZ;

// Pines analógicos: A1–A5
int analogVals[5];

// Tiempo
unsigned long previousMillis = 0;
const unsigned long interval = 100; // 100 ms

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 no conectado");
  }

  if (!CAN.begin(500E3)) {
    Serial.println("Fallo al iniciar CAN");
    while (1);
  }

  OBD2.begin();

  // Cabecera CSV
  Serial.println("Time(s),RPM,Speed(kph),CoolantTemp(C),TPS(%),"
                 "AccX(g),AccY(g),AccZ(g),GyroX(dps),GyroY(dps),GyroZ(dps),"
                 "A1,A2,A3,A4,A5");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Leer IMU
    mpu.getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);
    float ax = accX / 16384.0;
    float ay = accY / 16384.0;
    float az = accZ / 16384.0;
    float gx = gyroX / 131.0;
    float gy = gyroY / 131.0;
    float gz = gyroZ / 131.0;

    // Leer analógicos
    for (int i = 0; i < 5; i++) {
      analogVals[i] = analogRead(A1 + i);
    }

    // Leer OBD2
    float rpm = OBD2.pidAvailable(PID_ENGINE_RPM) ? OBD2.readPID(PID_ENGINE_RPM) : 0;
    float speed = OBD2.pidAvailable(PID_VEHICLE_SPEED) ? OBD2.readPID(PID_VEHICLE_SPEED) : 0;
    float coolant = OBD2.pidAvailable(PID_COOLANT_TEMPERATURE) ? OBD2.readPID(PID_COOLANT_TEMPERATURE) : 0;
    float tps = OBD2.pidAvailable(PID_THROTTLE) ? OBD2.readPID(PID_THROTTLE) : 0;

    // Imprimir CSV
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
