#include <Arduino.h>

template<class T> inline Print &operator <<(Print &obj, T arg) {
  obj.print(arg);
  return obj;
}

// Allow OTA software updates
#include <ElegantOTA.h>

#define WIFI_TIMEOUT 30000              // checks WiFi every ...ms. Reset after this time, if WiFi cannot reconnect.
#define HTTP_PORT 80
unsigned long auto_last_change = 0;
unsigned long last_wifi_check_time = 0;

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#define ESP_RESET ESP.restart()
WebServer server(HTTP_PORT);
#else
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
ESP8266WebServer server(HTTP_PORT);
#define ESP_RESET ESP.restart()
#endif

//#define STATIC_IP                       // uncomment for static IP, set IP below
#ifdef STATIC_IP
  IPAddress ip(192,168,0,123);
  IPAddress gateway(192,168,0,1);
  IPAddress subnet(255,255,255,0);
#endif

#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

//extern const char index_html[];

#include "data.h"

#define WIFI_SSID     "Connectify-me"
#define WIFI_PASSWORD "asd369/*"


#define StripPin 4
#define stripNumOfLeds 24

Adafruit_NeoPixel strip = Adafruit_NeoPixel(stripNumOfLeds, StripPin, NEO_GRB + NEO_KHZ800);

boolean ButtonState = true;
boolean Rainbow = false;

int ledBrightness = 50;
int Red = 255;
int Green = 0;
int Blue = 0;

long HueNow = 0;
String Valor = "";

void wifi_setup() {
  Serial.println();
  Serial.print("Conectandose a ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.mode(WIFI_STA);
  #ifdef STATIC_IP  
    WiFi.config(ip, gateway, subnet);
  #endif

  unsigned long connect_start = millis();
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    if(millis() - connect_start > WIFI_TIMEOUT) {
      Serial.println();
      Serial.print("Reintentando ");
      Serial.print(WIFI_TIMEOUT);
      Serial.print("Reseteando el ESP ahora.");
      ESP_RESET;
    }
      if (!MDNS.begin("sml")) {
    Serial.println("Error configurando mDNS!");
    while (1) {
      delay(1000);
    }
  }
  }

  Serial.println("");
  Serial.println("WiFi conectada");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void Rainbowcolor() {
  strip.rainbow(HueNow);
  HueNow += 256;
  if (HueNow > 65536) {
    HueNow = 0;
  }
  strip.show();
}

void Simplecolor(uint32_t ColorNow) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, ColorNow);
  }
strip.show();
}

void srv_handle_index_html() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", index_html);
}

void srv_handle_not_found() {
  String mensaje = "<h1>404</h1>";
  mensaje += "Pagina No encontrada</br>";
  mensaje += "Intenta otra pagina</br>";
  server.send(404, "text/html", mensaje);
}

void ONstrip() {
  ButtonState = true;
  Serial.println("Tira LED encendida");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "Tira LED encendida");
}

void OFFstrip() {
  ButtonState = false;
  Serial.println("Tira LED apagada");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "Tira LED apagada");
}

void StripBrightness() {

  if (server.hasArg("valor")) {
    Valor = server.arg("valor");
    ledBrightness = atoi(Valor.c_str());
    strip.setBrightness(ledBrightness);
    Serial << "Cambiando el brillo a " << ledBrightness << "\n";
  }
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "Brillo cambiado");
}

void StripColor() {
  Serial.println("Cambiando color");
  if (server.hasArg("r")) {
    Valor = server.arg("r");
    Red = atoi(Valor.c_str());
    Serial << "Rojo " << Red << "\n";
  }
  if (server.hasArg("g")) {
    Valor = server.arg("g");
    Green = atoi(Valor.c_str());
    Serial << "Verde " << Green << "\n";
  }
  if (server.hasArg("b")) {
    Valor = server.arg("b");
    Blue = atoi(Valor.c_str());
    Serial << "Azul " << Blue << "\n";
  }

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "cambiando color");
}

void RainbowEffect() {
  if (server.hasArg("ButtonState")) {
    Valor = server.arg("ButtonState");
    if (Valor == "on") {
      Rainbow = true;
    } else {
      Rainbow = false;
    }

    Serial << "Estado Arcoiris " << Rainbow << "\n";
  }
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "cambiando Arcoiris");
}

void setup() {
  Serial.begin(115200);
  delay(500);

 Serial.println("Wifi setup");
  wifi_setup(); 


#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  Serial.println("Iniciando Smart Music Lamp");
  server.on("/" , srv_handle_index_html);
  server.on("/on", ONstrip);
  server.on("/off", OFFstrip);
  server.on("/bright", StripBrightness);
  server.on("/color", StripColor);
  server.on("/rainbow", RainbowEffect);
  server.onNotFound(srv_handle_not_found);
  // Starting Async OTA web server AFTER all the server.on requests registered
  ElegantOTA.begin(&server);
  server.begin();
  Serial.println("Servidor HTTP iniciado");
  Serial.println("Listo!");

  strip.begin();
  strip.setBrightness(ledBrightness);
  strip.show();

  index_html.replace("%ip", WiFi.localIP().toString());
}

void loop() {
  server.handleClient();
  unsigned long now = millis();

#if defined(ESP8266)
  MDNS.update();
#endif

if(now - last_wifi_check_time > WIFI_TIMEOUT) {
    Serial.print("Chequeando conexion WiFi... ");
    if(WiFi.status() != WL_CONNECTED) {
      Serial.println("Conexion perdida. Reconectando...");
      wifi_setup();
    } else {
      Serial.println("OK");
    }
    last_wifi_check_time = now;
  }

  if (ButtonState) {
    if (Rainbow) {
      Rainbowcolor();
    } else {
      uint32_t ColorNow = strip.Color(Red, Green, Blue);
      Simplecolor(ColorNow);
    }
  } else {
    strip.clear();
    strip.show();
  }
  delay(10);

}


