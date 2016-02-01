#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <Servo.h>
#include <SimpleTimer.h>
#include "DHT.h"
#include "EmonLib.h"              // Include Emon Library
EnergyMonitor emon1;              // Create an instance
#include <Wire.h>
#include <PubSubClient.h>

char message_buff[250];
char message_buff_call[200];
EthernetClient ethClient;
void callback(char* topic, byte* payload, unsigned int length)
{
}
PubSubClient client("geeknesia.com", 1883, callback, ethClient);

int val11;
float val2;

Servo tombolpowerserver ;
int pos = 0;

#define DHTTYPE DHT22
#define DHTPIN 46
DHT dht(DHTPIN, DHTTYPE);

char auth[] = "997509a2fcc1438c98e038d5b230314d";
//WidgetLCD lcd(4);
SimpleTimer timer;

byte arduino_mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress arduino_ip ( 192, 168, 137, 101);
IPAddress dns_ip     (  8,   8,   8,   8);
IPAddress gateway_ip ( 192, 168, 137, 1);
IPAddress subnet_mask(255, 255, 0,   0);

void setup()
{
  Serial.begin(9600);
  //Blynk.begin(auth, "cloud.blynk.cc", 8442, arduino_ip, dns_ip, gateway_ip, subnet_mask, arduino_mac);
  Blynk.begin(auth);
  tombolpowerserver.attach(44);
  timer.setInterval(3000L, dht11display);
  timer.setInterval(3000L, power);
  timer.setInterval(3000L, vbattery);
  timer.setInterval(3000L, geek);
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

  //lcd.print(0, 1, h);
  //lcd.print(0, 0, t);
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);

}
void power ()
{
  double Irms = emon1.calcIrms(1480);
  Blynk.virtualWrite(V10, Irms);
  Blynk.virtualWrite(V11, Irms*230.0);
  }

void vbattery (){
      float temp;
      val11=analogRead(0);
      temp=val11/4.117;
      val2=(temp/10);
      //Serial.println(val2);
      Blynk.virtualWrite(V12, val2);
  }

void pencettombolpower ()
{

  tombolpowerserver.write(140);
  delay(2000);
  tombolpowerserver.write(0);

}

void geek(){
  
  
  client.connect("device-86a36925d58960bdd6e3d1c9d883bc51", NULL, NULL, "iot/will", 2, 64, "device-86a36925d58960bdd6e3d1c9d883bc51");
      Serial.println("Aktif");
          float temp;
      val11=analogRead(9);
      temp=val11/3.840;
      val2=(temp/10);
      Serial.println(val2);
    client.publish("iot/live", "device-86a36925d58960bdd6e3d1c9d883bc51"); //Masukkan device id
    //itoa(val2, pubschar1, 10);
    String pubString  = "{\"code\":\"815ecd3ed3818462038e91ac00467620:6984188513627be3422e357115c62a53\","; //masukkan username dan password anda sesuai yang ada pada detail device
    pubString += "\"attributes\": {";
    pubString += "\"volt\": "+ String(val2);

    pubString += "}}";
    pubString.toCharArray(message_buff, pubString.length() + 1);
    client.publish("iot/data", message_buff);
    }

void loop()
{
  Blynk.run();
  timer.run();
}
