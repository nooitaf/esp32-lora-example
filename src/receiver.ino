// (2) 868mhz / 915Mhz OLED LoRaSender
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "SSD1306.h"

#include <WS2812FX.h>
#define LED_COUNT 3
#define LED_PIN 35

//WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ400);
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

#define SCK 5 // GPIO5 - SX1278's SCK
#define MISO 19 // GPIO19 - SX1278's MISO
#define MOSI 27 // GPIO27 - SX1278's MOSI
#define SS 18 // GPIO18 - SX1278's CS
#define RST 14 // GPIO14 - SX1278's RESET
#define DI0 26 // GPIO26 - SX1278's IRQ (interrupt request)
#define BAND 868E6 // 915E6

unsigned int counter = 0;

SSD1306 display (0x3c, 4, 15);
String rssi = "RSSI -";
String packSize = "-";
String packet;


void setup () {

  ws2812fx.init();
  ws2812fx.setBrightness(100);
  ws2812fx.setSpeed(100);
  ws2812fx.setColor(0x007BFF);
  ws2812fx.setMode(FX_MODE_CHASE_RAINBOW);
  ws2812fx.start();

  pinMode (16, OUTPUT);
  pinMode (2, OUTPUT);

  digitalWrite (16, LOW); // set GPIO16 low to reset OLED
  delay (50);
  digitalWrite (16, HIGH); // while OLED is running, GPIO16 must go high,

  Serial.begin (9600);
  while (! Serial);
  Serial.println ();
  Serial.println ("LoRa Receiver Callback");

  SPI.begin (SCK, MISO, MOSI, SS);
  LoRa.setPins (SS, RST, DI0);
  if (! LoRa.begin (BAND)) {
    Serial.println ("Starting LoRa failed!");
    while (1);
  }
  LoRa.onReceive(cbk);
  LoRa.receive ();
  Serial.println ("init ok");
  display.init ();
  // display.flipScreenVertically ();
  display.setFont (ArialMT_Plain_10);

  display.clear ();
  display.setTextAlignment (TEXT_ALIGN_LEFT);
  display.setFont (ArialMT_Plain_10);
  display.drawString (0, 30, "waiting for packed cats...");
  display.display ();

  delay (1500);
}

void loop () {
  ws2812fx.service();

  int packetSize = LoRa.parsePacket();
  if (packetSize) { cbk(packetSize); }
  delay (10);
}

void loraData () {
  display.clear ();
  display.setTextAlignment (TEXT_ALIGN_LEFT);
  display.setFont (ArialMT_Plain_10);

  display.drawString (0, 0, rssi);
  display.drawString (0, 15, "Received " + packSize + " bytes");
  display.drawStringMaxWidth (0, 30, 128, packet);
  display.drawString (0, 45, "Counter " + String(counter));
  display.display ();

  counter ++;
}

void cbk(int packetSize) {
  packet = "";
  packSize = String(packetSize, DEC);
  for (int i = 0; i <packetSize; i ++) {
    packet += (char) LoRa.read();
  }
  rssi = "RSSI" + String(LoRa.packetRssi(), DEC);
  loraData();
  Serial.println ("cbk received");
}
