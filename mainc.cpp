#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>

const char* ssid = "ESP32-WIFI-AT_AG";
const char* password = "123456789";

Servo servo_izquierda; int pin_izquierda = 23;
Servo servo_derecha; int pin_derecha = 22;
int angulo; int angulo1; int angulo2;
AsyncWebServer server(80);
float x=0; float y=0; float z=9;

void setup() {
  Serial.begin(115200);

  // Conectar a WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi.");

  // Configuración de los servos
  servo_izquierda.attach(pin_izquierda); // Pin GPIO para servo 1
  servo_derecha.attach(pin_derecha); // Pin GPIO para servo 2
  Serial.println("Servos Acoplados");
}

void loop() {
  //si no conecta con el server
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Red no Conectada");
    delay(1000);
  }
  // Control de los servos según los valores de x e y
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Red Conectada");
    HTTPClient http;
    http.begin("http://192.168.4.1/update");  // Dirección IP del servidor ESP32 y la ruta de actualización

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println("Datos recibidos: " + payload);

      // Parsear los datos recibidos
      int delimiterIndex1 = payload.indexOf(',');
      int delimiterIndex2 = payload.lastIndexOf(',');

      x = payload.substring(0, delimiterIndex1).toFloat();
      y = payload.substring(delimiterIndex1 + 1, delimiterIndex2).toFloat();
      z = payload.substring(delimiterIndex2 + 1).toFloat();

      // Lógica para mover los servomotores según los rangos de aceleración
      if ((y < -4) && (y > -10)) {
        // Mover hacia delante
        servo_izquierda.write(0);
        servo_derecha.write(180);
        delay(15);
        /*
        for (angulo1=180,angulo2=0;(angulo1>=0)&&(angulo2<=180);angulo1--,angulo2++){
          servo_izquierda.write(angulo1);
          servo_derecha.write(angulo2);
          delay(15);
        }
        */
      } if ((y > 4) && (y < 10)) {
        // Mover hacia atras
        servo_izquierda.write(180);
        servo_derecha.write(0);
        delay(15);
        /*
        for (angulo1=0,angulo2=180;(angulo1<=180)&&(angulo2>=0);angulo1++,angulo2--){
          servo_izquierda.write(angulo1);
          servo_derecha.write(angulo2);
          delay(15);
        }
        */
      } if ((x > 4) && (x < 10)) {
        // Mover hacia izquierda
        servo_izquierda.write(0);
        servo_derecha.write(0);
        delay(15);
        /*
        for (angulo=180;angulo>=0;angulo--){
          servo_izquierda.write(angulo);
          servo_derecha.write(angulo);
          delay(15);
        }
        */
      } if ((x < -4) && (x > -10)) {
        // Mover hacia derecha
        servo_izquierda.write(180);
        servo_derecha.write(180);
        delay(15);
        /*
        for (angulo=0;angulo<=180;angulo++){
          servo_izquierda.write(angulo);
          servo_derecha.write(angulo);
          delay(15);
        }
        */
      }
      if (((x>-4)&&(x<4))&&((y>-4)&&(y<4))) {
        // Modo estático
        servo_izquierda.write(90);
        servo_derecha.write(90);
        delay(15);
      }
    } else {
      Serial.print("Error en la solicitud: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  // Retraso entre solicitudes para no saturar la red
}