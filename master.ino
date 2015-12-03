#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <Servo.h>
#include <SimpleTimer.h>
#include "DHT.h"
#include "EmonLib.h"              // Include Emon Library
EnergyMonitor emon1;              // Create an instance

Servo tombolpowerserver ;
int pos = 0;

#define DHTTYPE DHT22
#define DHTPIN 46
DHT dht(DHTPIN, DHTTYPE);

char auth[] = "997509a2fcc1438c98e038d5b230314d";
WidgetLCD lcd(4);
SimpleTimer timer;

byte arduino_mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress arduino_ip ( 192, 168, 137, 101);
IPAddress dns_ip     (  8,   8,   8,   8);
IPAddress gateway_ip ( 192, 168, 137, 1);
IPAddress subnet_mask(255, 255, 0,   0);

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, "cloud.blynk.cc", 8442, arduino_ip, dns_ip, gateway_ip, subnet_mask, arduino_mac);
  //Blynk.begin(auth);
  tombolpowerserver.attach(44);
  timer.setInterval(1000L, dht11display);
  dht.begin();
  emon1.voltage(2, 234.26, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(1, 111.1);        // Current: input pin, calibration.

}

BLYNK_WRITE(V1)
{ 
  // This command writes Arduino's uptime in seconds to Virtual Pin (5)
  // Blynk.virtualWrite(1, pencettombolpower() );
  pencettombolpower ();
}

void dht11display()
{
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  lcd.print(0, 1, h);
  lcd.print(0, 0, t);
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);

}

void pencettombolpower ()
{

  tombolpowerserver.write(140);
  delay(2000);
  tombolpowerserver.write(0);

}

void loop()
{
  Blynk.run();
  timer.run();
}
