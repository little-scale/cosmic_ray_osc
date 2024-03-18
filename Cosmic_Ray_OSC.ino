#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

// LED setup
const int number_of_leds = 10;  // number of leds in strip
const int led_pin = 2;
const int cosmic_ray = 1;
int strobe_rate = 250 / number_of_leds;  // set total time for strobe

char ssid[] = "---";  // your network SSID (name)
char pass[] = "---";
OSCErrorCode error;
unsigned int ledState = LOW;

WiFiUDP Udp;
const IPAddress outIp(192, 168, 0, 6);  // target IP address
const unsigned int outPort = 9999;
const unsigned int localPort = 8888;

const int id = 0;  // unique ID for this cosmic ray detector

int strobing = 1;
byte r = 255;
byte g = 128;
byte b = 64;

int ping = 0;






Adafruit_NeoPixel leds = Adafruit_NeoPixel(number_of_leds, led_pin);


void setup() {
  Serial.begin(57600);
  pinMode(cosmic_ray, INPUT);

  attachInterrupt(cosmic_ray, cosmic_ray_isr, RISING);

  // Setup LEDs
  leds.begin();
  leds.clear();
  leds.setBrightness(50);

  for (int i = 0; i < number_of_leds; i++) {
    leds.setPixelColor(i, r, g, b);
  }
  leds.show();

  // Start WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(localPort);
}

void loop() {
  getosc();

  if (ping == 1) {
    if (strobing == 1) {
      strobe();
    }

    sendOSC();
    Serial.println("ping");
    ping = 0; 
  }
}

void cosmic_ray_isr() {
  ping = 1;
}

void strobe() {
  for (int i = 0; i < number_of_leds; i++) {
    leds.clear();
    leds.setPixelColor(i, r, g, b);
    delay(strobe_rate);
    leds.show();
  }
  leds.clear();
  leds.show();
}

void sendOSC() {
  OSCMessage msg("/cosmic_ray");
  msg.add(id);
  msg.add(1);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
  delay(1);
}

void getosc() {
  OSCMessage msg;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }
    if (!msg.hasError()) {
      msg.dispatch("/led", led);
      msg.dispatch("/rgb", rgb);
    } else {
      error = msg.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
}

void rgb(OSCMessage &msg) {
  r = msg.getInt(0);
  g = msg.getInt(1);
  b = msg.getInt(2);

  Serial.print("RGB ");
  Serial.print(r);
  Serial.print(" ");
  Serial.print(g);
  Serial.print(" ");
  Serial.println(b);

  for (int i = 0; i < number_of_leds; i++) {
    leds.setPixelColor(i, r, g, b);
  }
  leds.show();
}

void led(OSCMessage &msg) {
  ledState = msg.getInt(0);
  Serial.print("/led: ");
  Serial.println(ledState);
  if (ledState == 1) {
    strobing = 0;
    for (int i = 0; i < number_of_leds; i++) {
      leds.setPixelColor(i, r, g, b);
    }
    leds.show();
  }

  if (ledState == 0) {
    strobing = 0;
    for (int i = 0; i < number_of_leds; i++) {
      leds.setPixelColor(i, 0, 0, 0);
    }
    leds.show();
  }

  if (ledState == 2) {
    strobing = 1;
  }
}
