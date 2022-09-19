// -------------------------------------------------
// Copyright (c) 2022 HiBit <https://www.hibit.dev>
// -------------------------------------------------

void setMagnetometerAdjustmentValues()
{
  uint8_t buff[3];

  I2CwriteByte(MPU9250_MAG_ADDRESS, 0x0A, 0x1F); // Set 16-bits output & fuse ROM access mode

  delay(1000);

  I2Cread(MPU9250_MAG_ADDRESS, 0x10, 3, buff); // Read adjustments

  magnetometer.adjustment.x = buff[0]; //Adjustment for X axis
  magnetometer.adjustment.y = buff[1]; //Adjustment for Y axis
  magnetometer.adjustment.z = buff[2]; //Adjustment for Z axis

  I2CwriteByte(MPU9250_MAG_ADDRESS, 0x0A, 0x10); // Power down
}

bool isMagnetometerReady()
{
  uint8_t isReady; // Interruption flag

  I2Cread(MPU9250_MAG_ADDRESS, 0x02, 1, &isReady);

  return isReady & 0x01; // Read register and wait for the DRDY
}

void readRawMagnetometer()
{
  uint8_t buff[7];

  // Read output registers:
  // [0x03-0x04] X-axis measurement
  // [0x05-0x06] Y-axis measurement
  // [0x07-0x08] Z-axis measurement
  I2Cread(MPU9250_MAG_ADDRESS, 0x03, 7, buff);

  // Magnetometer, create 16 bits values from 8 bits data
  magnetometer.x = (buff[1] << 8 | buff[0]);
  magnetometer.y = (buff[3] << 8 | buff[2]);
  magnetometer.z = (buff[5] << 8 | buff[4]);
}
