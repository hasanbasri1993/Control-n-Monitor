#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <Servo.h>
#include <SimpleTimer.h>

Servo tombolpowerserver ;
int pos = 0; 


#include <dht.h>
dht DHT;
#define DHT11_PIN 5

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "997509a2fcc1438c98e038d5b230314d";
WidgetLCD lcd(4);
SimpleTimer timer;

// Mac address should be different for each device in your LAN
byte arduino_mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress arduino_ip ( 192,168,0,101);
IPAddress dns_ip     (  8,   8,   8,   8);
IPAddress gateway_ip ( 192,168,0,254);
IPAddress subnet_mask(255, 255, 255,   0);

void setup()
{
  Serial.begin(9600);
  //Blynk.begin(auth, server_ip, 8442, arduino_ip, dns_ip, gateway_ip, subnet_mask, arduino_mac);
  // Or like this:
  Blynk.begin(auth, "cloud.blynk.cc", 8442, arduino_ip, dns_ip, gateway_ip, subnet_mask, arduino_mac);
  tombolpowerserver.attach(2);
  timer.setInterval(1000L, dht11display);
 // timer.setInterval(1000L, BLYNK_WRITE);
}



BLYNK_WRITE(V1)
{
  // This command writes Arduino's uptime in seconds to Virtual Pin (5)
 // Blynk.virtualWrite(1, pencettombolpower() );
 pencettombolpower ();
}

//BLYNK_READ(V4)
//{
//  // This command writes Arduino's uptime in seconds to Virtual Pin (5)
//  Blynk.virtualWrite(1, pencettombolpower() );
// //pencettombolpower ();
//}

void dht11display()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  
   int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:  
    Serial.print("OK,\t"); 
    break;
    case DHTLIB_ERROR_CHECKSUM: 
    Serial.print("Checksum error,\t"); 
    break;
    case DHTLIB_ERROR_TIMEOUT: 
    Serial.print("Time out error,\t"); 
    break;
    case DHTLIB_ERROR_CONNECT:
        Serial.print("Connect error,\t");
        break;
    case DHTLIB_ERROR_ACK_L:
        Serial.print("Ack Low error,\t");
        break;
    case DHTLIB_ERROR_ACK_H:
        Serial.print("Ack High error,\t");
        break;
    default: 
    Serial.print("Unknown error,\t"); 
    break;
  }
  // DISPLAY DATA
  Serial.print(DHT.humidity);
  Serial.print(",\t");
  Serial.println(DHT.temperature);
  lcd.print(0, 1, DHT.humidity);
  lcd.print(0, 0, DHT.temperature);
  Blynk.virtualWrite(V5, DHT.temperature);
  Blynk.virtualWrite(V6, DHT.humidity);
  
}

void pencettombolpower ()
{

    // in steps of 1 degree
    tombolpowerserver.write(140);              // tell servo to go to position in variable 'pos'
    delay(2000);                       // waits 15ms for the servo to reach the position
  
   
    tombolpowerserver.write(0);              // tell servo to go to position in variable 'pos'
                          // waits 15ms for the servo to reach the position
   
}

void loop()
{
  Blynk.run();
  timer.run();
  }
