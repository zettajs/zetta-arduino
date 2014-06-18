#include <LiquidCrystal.h>
#include <SPI.h>
#include <WiFi.h>
#include <Zetta.h>

#define PIN_ANALOG_IN_SOUND A1

int status = WL_IDLE_STATUS;

char ssid[] = "Loft21";
char pass[] = "silkylotus997";

char url[] = "http://192.168.1.123:3000";

ZettaServer server(url);
ZettaDevice microphone(&server), lcd(&server);

LiquidCrystal lcdDevice(12, 11, 5, 4, 3, 2);

void setup() {
  Serial.begin(9600);
  
  if(WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while(true);
  }
  
  while(status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSDID: ");
    Serial.println(ssid);
    
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  
  printWifiStatus();

  lcdDevice.begin(16, 2);
  lcdDevice.print("hello, world!");

  server.connect();

  microphone
    .type("microphone")
    .state("ready")
    .monitor("amplitude")
    .register();
  
  char allowWhenReady[] = { "change" };
  char changeFields[] = { "message", "text" };

  lcd
    .type("lcd")
    .state("ready")
    .when("ready", allowWhenReady)
    .map("change", &change, changeFields)
    .monitor("message")
    .register();
}

void loop() {
  if (server.connected()) {
    String amplitude = String(analogRead(PIN_ANALOG_IN_SOUND), DEC);
    microphone.publish("amplitude", amplitude);

    lcd.receive();

    delay(10000);
  }
}

void change(args) {
  char message[] = args[0];
  lcdDevice.print(message);
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println("dbm");
}
