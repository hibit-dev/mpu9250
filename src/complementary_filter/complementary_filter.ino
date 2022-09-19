// -------------------------------------------------
// Copyright (c) 2022 HiBit <https://www.hibit.dev>
// -------------------------------------------------

#include "Wire.h"
#include "I2C.h"

#define MPU9250_IMU_ADDRESS 0x68
#define MPU9250_MAG_ADDRESS 0x0C

#define GYRO_FULL_SCALE_250_DPS  0x00
#define GYRO_FULL_SCALE_500_DPS  0x08
#define GYRO_FULL_SCALE_1000_DPS 0x10
#define GYRO_FULL_SCALE_2000_DPS 0x18

#define ACC_FULL_SCALE_2G  0x00
#define ACC_FULL_SCALE_4G  0x08
#define ACC_FULL_SCALE_8G  0x10
#define ACC_FULL_SCALE_16G 0x18

#define TEMPERATURE_OFFSET 21 // As defined in documentation

#define INTERVAL_MS_PRINT 1000

#define G 9.80665

struct gyroscope_raw {
  int16_t x, y, z;
} gyroscope;

struct accelerometer_raw {
  int16_t x, y, z;
} accelerometer;

struct magnetometer_raw {
  int16_t x, y, z;

  struct {
    int8_t x, y, z;
  } adjustment;
} magnetometer;

struct temperature_raw {
  int16_t value;
} temperature;

struct {
  struct {
    float x, y, z;
  } accelerometer, gyroscope, magnetometer;

  float temperature;
} normalized;

struct angle {
  float x, y, z = 0;
};

angle position;

unsigned long lastPrintMillis = 0;
unsigned long lastSampleMicros = 0;

void setup()
{
  Wire.begin();
  Serial.begin(115200);

  // Data Output Rate = 1000 / (1 + SRD)
  // Output rate must be at least 2x DLPF rate
  I2CwriteByte(MPU9250_IMU_ADDRESS, 25, 0x01); // Set the SRD to 1
  // Set the DLPF to 184HZ by default
  I2CwriteByte(MPU9250_IMU_ADDRESS, 26, 0x01);
  // Configure gyroscope and accelerometer scale
  I2CwriteByte(MPU9250_IMU_ADDRESS, 27, GYRO_FULL_SCALE_1000_DPS);
  I2CwriteByte(MPU9250_IMU_ADDRESS, 28, ACC_FULL_SCALE_2G);
  // Enable interrupt pin for raw data
  I2CwriteByte(MPU9250_IMU_ADDRESS, 56, 0x01);
}

void loop()
{
  unsigned long currentMillis = millis();

  readSample();

  if (currentMillis - lastPrintMillis > INTERVAL_MS_PRINT) {
    Serial.print("TEMP:\t");
    Serial.print(normalized.temperature, 2);
    Serial.print("\xC2\xB0"); //Print degree symbol
    Serial.print("C");
    Serial.println();

    Serial.print("Pitch:\t");
    Serial.print(getPitch());
    Serial.print("\xC2\xB0"); //Print degree symbol
    Serial.println();

    Serial.print("Roll:\t");
    Serial.print(getRoll());
    Serial.print("\xC2\xB0"); //Print degree symbol
    Serial.println();

    Serial.println();

    lastPrintMillis = currentMillis;
  }
}

bool readSample()
{
  if (isImuReady() == false) {
    return false;
  }

  unsigned long sampleMicros = (lastSampleMicros > 0) ? micros() - lastSampleMicros : 0;

  lastSampleMicros = micros();

  readRawImu();

  normalize(gyroscope);
  normalize(accelerometer);
  normalize(temperature);

  angle accelerometer = calculateAccelerometerAngles();
  angle gyroscope = calculateGyroscopeAngles(sampleMicros);

  detectPitch(gyroscope, accelerometer);
  detectRoll(gyroscope, accelerometer);

  return true;
}
