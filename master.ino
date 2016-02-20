#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
//#define BLYNK_DEBUG
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
  int i = 0; for (i = 0; i < length; i++)
  {
    message_buff_call[i] = payload[i];
  }
  message_buff_call[i] = '\0';
  String msgString = String(message_buff_call);
  Serial.println(msgString);

  if (msgString.equals("on"))
  {
    Serial.println("ON");
    digitalWrite(35, HIGH);
  }
  if (msgString.equals("off"))
  {
    Serial.println("OFF");
    digitalWrite(35, LOW);
  }
}
PubSubClient client("geeknesia.com", 1883, callback, ethClient);

int val11;
float val2;
float valpln;
float valdc;

Servo tombolpowerserver ;
int pos = 0;

#define DHTTYPE DHT22
#define DHTPIN 46
DHT dht(DHTPIN, DHTTYPE);

char auth[] = "0ef215c4602b404a9fa36a88ceaa518c";
//WidgetLCD lcd(4);
SimpleTimer timer;

byte arduino_mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress server     ( 128, 199, 237, 153);
IPAddress arduino_ip ( 192, 168, 137, 101);
IPAddress dns_ip     (  8,   8,   8,   8);
IPAddress gateway_ip ( 192, 168, 137, 1);
IPAddress subnet_mask(255, 255, 0,   0);

void setup()
{
  Serial.begin(9600);
  //Blynk.begin(auth, server, 8442, arduino_ip, dns_ip, gateway_ip, subnet_mask, arduino_mac);
  Blynk.begin(auth);
  tombolpowerserver.attach(44);
  timer.setInterval(3000L, dht11display);
  timer.setInterval(3000L, vbattery);
  timer.setInterval(3000L, geek);
  dht.begin();
  //emon1.voltage(10, 234.26, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(8, 40);        // Current: input pin, calibration.

}

BLYNK_WRITE(V1)
{
  //pencettombolpower ();
  tombolpowerserver.write(140);
  delay(2000);
  tombolpowerserver.write(0);
}

/*void pencettombolpower ()
  {

  tombolpowerserver.write(140);
  delay(2000);
  tombolpowerserver.write(0);

  } */


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

void vbattery () {
  float temp;
  val11 = analogRead(9);
  temp = val11 / 4.130;
  val2 = (temp / 10);
  Blynk.virtualWrite(V13, val2);
}

void geek() {

  client.connect("device-86a36925d58960bdd6e3d1c9d883bc51", NULL, NULL, "iot/will", 2, 64, "device-86a36925d58960bdd6e3d1c9d883bc51");

  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  //Tegangan Battery
  float vtemp;
  val11 = analogRead(9);
  vtemp = val11 / 4.130;
  val2 = (vtemp / 10);

  //Tegangan PLN
  float vout = 0.0;
  float vin = 0.0;
  float vpln = 0.0;
  float R1 = 33000.0; //
  float R2 = 8200.0; //
  int value = 0;
  value = analogRead(10);
  vout = (value * 5.0) / 1010.0; // see text
  vin = vout / (R2 / (R1 + R2));
  vpln = vin * 14.4 ;

  //Arus
  double Irms = emon1.calcIrms(1480);
  //Daya
  float power = Irms * vpln ;

  client.publish("iot/live", "device-86a36925d58960bdd6e3d1c9d883bc51"); //Masukkan device id

  String pubString  = "{\"code\":\"815ecd3ed3818462038e91ac00467620:6984188513627be3422e357115c62a53\","; //masukkan username dan password anda sesuai yang ada pada detail device
  pubString += "\"attributes\": {";
  pubString += "\"h1\":\"" + String(h) + "\",\"";
  pubString += "t1\":\"" + String(t) + "\",\"";
  pubString += "watt\":\"" + String(power) + "\",\"";
  pubString += "current\":\"" + String(Irms) + "\",\"";
  pubString += "voltb\":\"" + String(val2) + "\",\"";
  pubString += "volt\":\"" + String(vpln) + "\"";
  pubString += "}}";
  pubString.toCharArray(message_buff, pubString.length() + 1);
  client.publish("iot/data", message_buff);

  Blynk.virtualWrite(V10, Irms);
  Blynk.virtualWrite(V11, vpln);
  Blynk.virtualWrite(V12, power);
}

void loop()
{
  Blynk.run();
  timer.run();

}
