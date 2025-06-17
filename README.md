# Arduino Track Day Datalogger 📈🚗

Sistema de adquisición de datos para vehículos tipo track day, desarrollado con Arduino y compatible con análisis en WinTax.

---

## 🎯 Objetivo

Este proyecto recopila datos de vehículos de pista utilizando:

- 📟 Lectura OBD2 por red **CAN**
- 🧭 Sensores propios: **acelerómetro**, **giroscopio** (MPU6050)
- ⚙️ Medidores analógicos: presión de freno, extensión de amortiguadores, etc.
- 💾 Salida **CSV vía Serial**, compatible con WinTax o análisis posterior

---

## 🛠️ Hardware Recomendado

| Componente               | Modelo Sugerido                   |
|-------------------------|------------------------------------|
| Microcontrolador        | Arduino Mega 2560 (u otro con CAN) |
| Módulo CAN              | MCP2515 con TJA1050                |
| Sensor IMU              | MPU6050                            |
| Sensores analógicos     | Presión, recorrido lineal, etc.    |
| Software de análisis    | WinTax, Excel, Python, etc.        |

---

## 🔌 Conexiones

| Señal                  | Pin Arduino |
|------------------------|-------------|
| CAN CS                 | 10          |
| IMU SDA/SCL            | A4 / A5     |
| Presión de freno       | A1          |
| Amortiguador trasero R | A2          |
| (otros sensores)       | A3–A5       |
[ VEHÍCULO ]
   │
[ OBD2 ]
   ├── Pin 6 (CAN H) ──┐
   ├── Pin 14 (CAN L) ─┘──> MCP2515 → Arduino
   └── Pin 16 (12V) ──> Step-down → Arduino 5V
              └── Pin 4 (GND) ─────→ Arduino GND

[ SENSORES A1–A5 ] ──> Arduino A1–A5

[ MPU6050 ] ── SDA/SCL → Arduino I2C

[ Arduino ] ──> Serial (USB) → PC → WinTax / Logger

---

## 📄 Estructura de la salida CSV

```
Time(s),RPM,Speed(kph),CoolantTemp(C),TPS(%),
AccX(g),AccY(g),AccZ(g),GyroX(dps),GyroY(dps),GyroZ(dps),
A1,A2,A3,A4,A5
```

Ejemplo:

```
0.00,1230,45,85.0,12.3,0.01,-0.02,0.99,-0.5,1.2,0.1,2.55,1.23,0.87,0.00,1.77
```

---

## 🔁 Frecuencia de muestreo

- 10 Hz (cada 100 ms)
- Puede ajustarse modificando el parámetro `interval` en el código fuente.

---

## ⚙️ Cómo usar

1. Clona este repositorio.
2. Conecta todos los sensores y módulos al Arduino.
3. Abre `src/data_logger.ino` en el IDE de Arduino.
4. Sube el sketch y abre el **Monitor Serial** a 115200 baudios.
5. Guarda la salida CSV o redirígela a un archivo.

---

## 📦 Requisitos de librerías Arduino

Instala desde el Library Manager:

- `MCP_CAN_lib` (por Cory J. Fowler)
- `MPU6050` (de Jeff Rowberg)

---

## 📈 Exportar a WinTax

1. Guarda el CSV con delimitador coma `,`.
2. Importa en WinTax como canal personalizado.
3. Alinea los nombres de columna si es necesario.

---

## 📜 Licencia

MIT License – libre uso para proyectos educativos y deportivos.  
No se garantiza exactitud para fines comerciales o homologaciones oficiales.

---

## 🙋 Autor

Juan Sebastian Hurtado López  
Barcelona, España – 2025  
Proyecto de Máster en Motorsport & E-Racing  
