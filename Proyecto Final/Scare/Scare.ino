/* Proyecto Final

  Código modificado por los alumnos:
  - Paredes Zamudio Luis Daniel @Wallsified
  - González Arceo Carlos Eduardo @Carlos-crea

 Notas al 10/nov/22: 
 - Se necesita un método que regrese el valor de los decibeles
 eso es lo que podemos mandar a JavaScript para interactuar con los 
 segmentos de la batería, creo. 

 - Averiguar si lo que está dentro de loop() se puede hacer como 
 un método externo, simplemente para organizar mejor las partes. 

 - El método "processor" necesita cambiarse si o si. Solo copie y pegué
 lo de la práctica de HTTP para que el compilador me dijera si hay errores
 (que si los hay, pero no de compilación. )

 - Se puede cambiar la fuente en la pantalla para farolear aun más. 


*/

//Bibliotecas----------------------------------
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "Wire.h"
#include "Adafruit_GFX.h"
#include <Adafruit_SH110X.h>


//Constantes de la Ejecución.
int leds[5] = {33, 25, 26, 27, 14}; //Igual y no los ocupamos. 
const char* ssid = "Clase_IoT";//"INFINITUMA6A4_2.4";//"Mark Wifi";
const char* password = "0123456789";//"Zamudiov3!";//"achtzehnpfannkuchen18";


//Stuff de la pantalla.
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define i2c_Address 0x3c

const int sampleWindow = 50;                              // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

//Objeto AsyncWebServer, puerto 80-------------
AsyncWebServer server(80);


void start_screen() {
  display.begin(i2c_Address, true);              //OLED display start
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.display();
  delay(1000);
  display.clearDisplay();
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
  delay(2000);

  //Función para error 404
  server.onNotFound([](AsyncWebServerRequest * request) {
    request->send(404, "text/plain", "Error 404");
  });
}


// Acciones a realizar durante la ejecución.
String processor(const String& var) {

  if (var == "BUTTONPLACEHOLDER") { //switches HTTP
    String buttons = "";
    return buttons;
  }

  if (var == "SLIDERVALUE") { //slider HTTP
    return "hello";
  }
  return String(); //evidentemente todo esto se cambia
}

void setup() {
  //Iniciamos los Leds en output usando un bucle for para recorrer el arreglo.
  Serial.begin(115200);
  for (int i = 0; i < 5; i++) {
    pinMode(leds[i], OUTPUT);
  }
  setup_wifi();
  start_screen();
  server.begin();

  //Revisar archivos subidos a la memoria flash---
  if (!SPIFFS.begin(true)) {
    Serial.println("Hubo un error en SPIFFS. Vuelve a enviar los archivos.");
    return;
  }

  //Ruta raíz del archivo HTML-----------------
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  //Archivo CSS--------------------------------
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  //  Archivo JS--------------------------------
  server.on("/help.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/script.js", "text/js");
  });

  // Imágenes --------------------------------
  server.on("/door_batt.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/door_off.png", "image/png");
  });
}


void loop() {
  //este es el loop de lectura del micrófono. 
  unsigned long startMillis = millis();                  // Start of sample window
  float peakToPeak = 0;                                  // peak-to-peak level

  unsigned int signalMax = 0;                            //minimum value
  unsigned int signalMin = 1024;                         //maximum value

  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(0);                             //get reading from microphone
    if (sample < 1024)                                  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;                           // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;                           // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;                    // max - min = peak-peak amplitude
  float db = map(peakToPeak, 20, 900, 49.5, 90);         //calibrate for deciBels
  display.setCursor(0, 0);                               
  display.setTextSize(1.5);                              
  display.print(db);                                     //write calibrated deciBels
  display.print(" dB");                                  //write units


  for (int x = 16; x < 165; x = x + 17) {                
    display.drawLine(x, 32, x, 27, SH110X_WHITE); //NO CAMBIAR ESO
  }
  display.drawRoundRect(0, 32, 120, 19, 6, SH110X_WHITE);       //draw outline of bar graph
  int r = map(db, 0, 120, 1, 120);                       //set bar graph for width of screen
  display.fillRoundRect(1, 33, r, 18, 6, SH110X_WHITE);         //draw bar graph with a width of r
  display.display();                                     
  display.clearDisplay();                                

}
