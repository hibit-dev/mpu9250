// -------------------------------------------------
// Copyright (c) 2022 HiBit <https://www.hibit.dev>
// -------------------------------------------------

angle calculateAccelerometerAngles()
{
  angle accelerometer;

  accelerometer.x = atan(normalized.accelerometer.y / sqrt(sq(normalized.accelerometer.x) + sq(normalized.accelerometer.z)));
  accelerometer.y = atan(-1 * normalized.accelerometer.x / sqrt(sq(normalized.accelerometer.y) + sq(normalized.accelerometer.z)));
  accelerometer.z = atan2(accelerometer.y, accelerometer.x);

  return accelerometer;
}

angle calculateGyroscopeAngles(unsigned long sampleMicros)
{
  angle gyroscope;

  gyroscope.x = normalized.gyroscope.x * sampleMicros / 1000000;
  gyroscope.y = normalized.gyroscope.y * sampleMicros / 1000000;
  gyroscope.z = normalized.gyroscope.z * sampleMicros / 1000000;

  return gyroscope;
}

void detectPitch(angle gyroscope, angle accelerometer)
{
  position.x = 0.98 * (position.x + degrees(gyroscope.x)) + 0.02 * degrees(accelerometer.x);
}

double getPitch()
{
  return position.x;
}

void detectRoll(angle gyroscope, angle accelerometer)
{
  position.y = 0.98 * (position.y + degrees(gyroscope.y)) + 0.02 * degrees(accelerometer.y);
}

double getRoll()
{
  return position.y;
}
