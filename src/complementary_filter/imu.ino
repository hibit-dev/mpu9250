// -------------------------------------------------
// Copyright (c) 2022 HiBit <https://www.hibit.dev>
// -------------------------------------------------

bool isImuReady()
{
  uint8_t isReady; // Interruption flag

  I2Cread(MPU9250_IMU_ADDRESS, 58, 1, &isReady);

  return isReady & 0x01; // Read register and wait for the RAW_DATA_RDY_INT
}

void readRawImu()
{
  uint8_t buff[14];

  // Read output registers:
  // [59-64] Accelerometer
  // [65-66] Temperature
  // [67-72] Gyroscope
  I2Cread(MPU9250_IMU_ADDRESS, 59, 14, buff);

  // Accelerometer, create 16 bits values from 8 bits data
  accelerometer.x = (buff[0] << 8 | buff[1]);
  accelerometer.y = (buff[2] << 8 | buff[3]);
  accelerometer.z = (buff[4] << 8 | buff[5]);

  // Temperature, create 16 bits values from 8 bits data
  temperature.value = (buff[6] << 8 | buff[7]);

  // Gyroscope, create 16 bits values from 8 bits data
  gyroscope.x = (buff[8] << 8 | buff[9]);
  gyroscope.y = (buff[10] << 8 | buff[11]);
  gyroscope.z = (buff[12] << 8 | buff[13]);
}
