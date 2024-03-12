#include <Wire.h>

const int MPU_ADDR = 0x68; // Dirección I2C del MPU-6050
int16_t AcX, AcY, AcZ; // Variables para almacenar los valores de aceleración
float AcX_mps2, AcY_mps2, AcZ_mps2; // Variables para almacenar los valores de aceleración en m/s²

void setup() {
  Wire.begin();
  Serial.begin(115200);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void loop() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // Dirección del registro de aceleración X
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true); // Solicita los 6 registros de aceleración

  AcX = Wire.read() << 8 | Wire.read(); // Aceleración en X
  AcY = Wire.read() << 8 | Wire.read(); // Aceleración en Y
  AcZ = Wire.read() << 8 | Wire.read(); // Aceleración en Z

   AcX=AcX-1000;
   AcY=AcY-200;

   
  // Convertir valores a m/s²
  AcX_mps2 = (AcX / 32767.0) * (2 * 9.81);
  AcY_mps2 = (AcY / 32767.0) * (2 * 9.81);
  AcZ_mps2 = (AcZ / 32767.0) * (2 * 9.81);
 
  // Calcular roll y pitch basados en los valores de aceleración
  float roll = atan2(AcY, sqrt(AcX * AcX + AcZ * AcZ)) * 180.0 / PI;
  float pitch = atan2(AcX, sqrt(AcY * AcY + AcZ * AcZ)) * 180.0 / PI;

 //  Imprimir los valores en el monitor serial
  Serial.print("Roll = "); Serial.print(roll);
  Serial.print(" Pitch = "); Serial.print(pitch);
  Serial.print(" AcX_m/s2 = "); Serial.print(AcX_mps2);
  Serial.print(" AcY_m/s2 = "); Serial.print(AcY_mps2);
  Serial.print(" AcZ_m/s2 = "); Serial.println(AcZ_mps2); 

  delay(10);
}
