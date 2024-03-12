#include <Wire.h> // Incluir la biblioteca Wire para la comunicación I2C

// Definiciones y variables globales
const int MPU_ADDR = 0x68; // Dirección I2C del MPU-6050
int16_t AcX, AcY, AcZ, GyX, GyY, GyZ; // Variables para almacenar los valores de aceleración y giroscopio
float roll, pitch, yaw; // Variables para almacenar los ángulos de orientación calculados
unsigned long tiempo_previo = 0; // Variable para almacenar el tiempo anterior en el ciclo de loop
float offsetX = 0, offsetY = 0, offsetZ = 0; // Variables para almacenar los offsets de calibración del giroscopio

void setup() {
  Wire.begin(); // Iniciar la comunicación I2C
  Serial.begin(115200); // Iniciar la comunicación serial a 115200 bps para la salida de datos
  Wire.beginTransmission(MPU_ADDR); // Iniciar transmisión al MPU-6050
  Wire.write(0x6B); // Escribir en el registro PWR_MGMT_1
  Wire.write(0);    // Establecer el bit de encendido, despertando al MPU-6050
  Wire.endTransmission(true); // Finalizar la transmisión y liberar el bus

  calibrarGiroscopio(); // Llamar a la función de calibración del giroscopio al iniciar
}

void loop() {
  long tiempo_actual = millis(); // Obtener el tiempo actual en milisegundos
  float dt = (tiempo_actual - tiempo_previo) / 1000000.0; // Calcular el delta de tiempo en segundos

  // Leer valores del acelerómetro
  Wire.beginTransmission(MPU_ADDR); // Iniciar transmisión al MPU-6050
  Wire.write(0x3B); // Solicitar los datos del acelerómetro desde el registro 0x3B
  Wire.endTransmission(false); // Finalizar la transmisión manteniendo el bus activo
  Wire.requestFrom(MPU_ADDR, 6, true); // Solicitar 6 bytes del acelerómetro
  AcX = Wire.read() << 8 | Wire.read(); // Leer los valores de aceleración en X
  AcY = Wire.read() << 8 | Wire.read(); // Leer los valores de aceleración en Y
  AcZ = Wire.read() << 8 | Wire.read(); // Leer los valores de aceleración en Z

  // Leer valores del giroscopio y aplicar la calibración
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x43); // Solicitar los datos del giroscopio desde el registro 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);
  GyX = (Wire.read() << 8 | Wire.read()) - offsetX; // Leer y calibrar GyX
  GyY = (Wire.read() << 8 | Wire.read()) - offsetY; // Leer y calibrar GyY
  GyZ = (Wire.read() << 8 | Wire.read()) - offsetZ; // Leer y calibrar GyZ

  // Procesamiento de los datos para cálculos de orientación
  float GxRad = (GyX / 131.0) * (3.1416 / 180.0); // Convertir GyX a radianes
  float GyRad = (GyY / 131.0) * (3.1416 / 180.0); // Convertir GyY a radianes
  float GzRad = (GyZ / 131.0) * (3.1416 / 180.0); // Convertir GyZ a radianes

  // Calcular roll y pitch usando los datos del acelerómetro
  float roll_acc = atan2(AcY, sqrt(AcX * AcX + AcZ * AcZ)) * 180.0 / PI;
  float pitch_acc = atan2(AcX, sqrt(AcY * AcY + AcZ * AcZ)) * 180.0 / PI;

  // Integrar las velocidades angulares para obtener las orientaciones
  roll += GyX * dt; // Integrar GyX para obtener el roll
  pitch += GyY * dt; // Integrar GyY para obtener el pitch
  yaw += GyZ * dt; // Integrar GyZ para obtener el yaw

  // Aplicar un filtro complementario para combinar datos del acelerómetro y giroscopio
  roll = roll * 0.95 + roll_acc * 0.05; // Filtro para roll
  pitch = pitch * 0.95 + pitch_acc * 0.05; // Filtro para pitch

  // Salida de los valores calculados a la consola serial
  Serial.print("Roll = "); Serial.print(roll);
  Serial.print(" Pitch = "); Serial.print(pitch);
  Serial.print(" Yaw = "); Serial.print(yaw);
  Serial.print(" GxRad = "); Serial.print(GxRad);
  Serial.print(" GyRad = "); Serial.print(GyRad);
  Serial.print(" GzRad = "); Serial.println(GzRad);

  tiempo_previo = tiempo_actual; // Actualizar el tiempo previo para el siguiente ciclo
  delay(10); // Pequeña pausa para estabilizar la lectura de datos
}

// Función para calibrar el giroscopio
void calibrarGiroscopio() {
  long sumX = 0, sumY = 0, sumZ = 0; // Variables para sumar los valores

  // Sumar 100 lecturas del giroscopio para el promedio
  for (int i = 0; i < 100; i++) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x43); // Solicitar datos del giroscopio
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 6, true);
    sumX += Wire.read() << 8 | Wire.read(); // Sumar valores de X
    sumY += Wire.read() << 8 | Wire.read(); // Sumar valores de Y
    sumZ += Wire.read() << 8 | Wire.read(); // Sumar valores de Z

    delay(10); // Pequeña pausa entre lecturas
  }

  // Calcular el promedio para obtener los offsets
  offsetX = sumX / 100.0;
  offsetY = sumY / 100.0;
  offsetZ = sumZ / 100.0;
}
