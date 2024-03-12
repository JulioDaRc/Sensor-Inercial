#include <Wire.h>

const int MPU_ADDR = 0x68; // Dirección I2C del MPU-6050
int16_t AcX, AcY, AcZ, GyX, GyY, GyZ; // Variables para almacenar los valores de aceleración y giroscopio
float roll, pitch, yaw;
unsigned long tiempo_previo = 0;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void loop() {
  long tiempo_actual = millis();
  float dt = (tiempo_actual - tiempo_previo) / 1000000.0; // Delta de tiempo en segundos

  // Leer acelerómetro
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  // Leer giroscopio
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  // Calibración de los ejes del giroscopio
  GyX = GyX - 550;
  GyY = GyY + 260;
  GyZ = GyZ + 510;


float GxRad=(GyX/131.0)*(3.1416/180.0);
float GyRad=(GyY/131.0)*(3.1416/180.0);
float GzRad=(GyZ/131.0)*(3.1416/180.0);
  
  // Cálculos de orientación basados en acelerómetro
  float roll_acc = atan2(AcY, sqrt(AcX * AcX + AcZ * AcZ)) * 180.0 / PI;
  float pitch_acc = atan2(AcX, sqrt(AcY * AcY + AcZ * AcZ)) * 180.0 / PI;

  // Integrar velocidades angulares para obtener orientaciones
  roll += GyX * dt; // Integra la velocidad angular en X para obtener el roll
  pitch += GyY * dt; // Integra la velocidad angular en Y para obtener el pitch
  yaw += GyZ * dt; // Integra la velocidad angular en Z para obtener el yaw

  // Aplicar filtro complementario
  roll = roll * 0.95 + roll_acc * 0.05;
  pitch = pitch * 0.95 + pitch_acc * 0.05;
  // Nota: No se aplica filtro complementario al yaw ya que no hay medición directa del acelerómetro para compensarlo



  Serial.print("Roll = "); Serial.print(roll);
  Serial.print(" Pitch = "); Serial.print(pitch);
  Serial.print(" Yaw = "); Serial.print(yaw);
   Serial.print(" GxRAD/S = "); Serial.print(GxRad);
 Serial.print(" GyRAD/S = "); Serial.print(GyRad);
 Serial.print(" GzRAD/S = "); Serial.println(GzRad);
  

  tiempo_previo = tiempo_actual; // Actualizar el tiempo previo para el próximo ciclo
  delay(10);
}
