#include <SPI.h>

#include <Adafruit_WINC1500.h>
#include <Adafruit_WINC1500SSLClient.h>
#include <EmonLib_3PH.h>

#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

// ATWINC1500 pins on Adafruit Feather M0 Wi-Fi
#define WINC_CS   8
#define WINC_IRQ  7
#define WINC_RST  4
#define WINC_EN   2
Adafruit_WINC1500 WiFi(WINC_CS, WINC_IRQ, WINC_RST);

// Wi-Fi Network
#define WLAN_SSID "CHANGEME"
#define WLAN_PASS "CHANGEME"

#define SENSORS 3
EnergyMonitor emon[SENSORS];

// Adafruit MQTT
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  8883
#define AIO_USERNAME    "CHANGEME"
#define AIO_KEY         "CHANGEME"
Adafruit_WINC1500SSLClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish mqtt_voltage[SENSORS] = {
  Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/l1-voltage"),
  Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/l2-voltage"),
  Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/l3-voltage")
};
Adafruit_MQTT_Publish mqtt_current[SENSORS] = {
  Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/l1-current"),
  Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/l2-current"),
  Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/l3-current")
};
Adafruit_MQTT_Publish mqtt_power[SENSORS] = {
  Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/l1-real-power"),
  Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/l2-real-power"),
  Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/l3-real-power")
};
// Sensor pins
#define PIN_V1  3
#define PIN_CT1 0
#define PIN_V2  4
#define PIN_CT2 1
#define PIN_V3  5
#define PIN_CT3 2

void setup() {

  // Enable 12-bit ADC
  analogReadResolution(ADC_BITS);

  SerialUSB.begin(115200);

// Enable Wi-Fi
#ifdef WINC_EN
  pinMode(WINC_EN, OUTPUT);
  digitalWrite(WINC_EN, HIGH);
#endif

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    SerialUSB.print(".");
    delay(100);
  }

  emon[0].voltage(PIN_V1, 268.97, 2.0, 1);
  emon[0].voltage(240);
  emon[0].current(PIN_CT1, 90.9);
  
  emon[1].voltage(PIN_V2, 268.97, 2.0, 2);
  emon[1].voltage(240);
  emon[1].current(PIN_CT2, 90.9);
  
  emon[2].voltage(PIN_V3, 268.97, 2.0, 3);
  emon[2].voltage(240);
  emon[2].current(PIN_CT3, 90.9);  

}

void loop() {
  MQTT_connect();

  for (int i = 0; i < SENSORS; i++) {
    // perform measurements
    emon[i].calcVI(16,2000);    // 2 h.c. for buffering + 14 h.c. for measuring
    SerialUSB.println(i);
    
    // convert and pack data
    if (mqtt_voltage[i].publish((float)emon[i].Vrms) || 0) {
      SerialUSB.print("Published voltage: ");
      SerialUSB.println((float)emon[i].Vrms);
    }
    if (mqtt_current[i].publish((float)emon[i].Irms) || 0) {
      SerialUSB.print("Published current: ");
      SerialUSB.println((float)emon[i].Irms);
    }
    if (mqtt_power[i].publish((float)emon[i].apparentPower) || 0) {
      SerialUSB.print("Published apparent power: ");
      SerialUSB.println((float)emon[i].apparentPower);
    }
  }
  
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  SerialUSB.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       SerialUSB.println(mqtt.connectErrorString(ret));
       SerialUSB.println("Retrying MQTT connection in 10 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
  }
  SerialUSB.println("MQTT Connected!");
}
