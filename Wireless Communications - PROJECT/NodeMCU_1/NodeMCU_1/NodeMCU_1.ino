/*
  Sender DHT 11 with FAN and BIR with LED
*/
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include "SD.h"



//dht
#include "DHT.h"
#include <espnow.h>


//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
/*
  // Insert your network credentials
  #define WIFI_SSID "The MonsteR"
  #define WIFI_PASSWORD "K13553067"
*/

// Insert your network credentials
#define WIFI_SSID "Hassan"
#define WIFI_PASSWORD "Aa123456790"



// Insert Firebase project API Key
#define API_KEY "AIzaSyDOpG6ZPgepDOW5JqkgbEZYnuLkvR8RhY0"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "digitalcommunicationsk13-default-rtdb.firebaseio.com"

//Define Firebase Data object
FirebaseData fbdo;  //pointer

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;


// DHT Sensor NodeMCU board
#define DHTTYPE DHT11
// DHT Sensor
const int DHTPin = D6;

// FAN - GPIO 0 = D3 on ESP-12E NodeMCU board
const int fan = D5;

// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

/*
  //define sound velocity in cm/uS
  #define SOUND_VELOCITY 0.034
  #define CM_TO_INCH 0.393701

  long duration;
  float distanceCm;
  float distanceInch;
*/
//long duration;
float temp;
float hum;
//end defind dht11

int sensor = D7;  // Digital pin 
const int ledPin = D8;
const int ldrPin = A0;


//////////////////////////////////////////////////////////////////////////**VOID SETUP**//////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(fan, OUTPUT);
  //The setup function sets your ESP GPIOs to Outputs,
  pinMode(ledPin, OUTPUT);
//  pinMode(led2pin, OUTPUT);
  //The setup function sets your ESP GPIOs to Inputs,
  pinMode(sensor, INPUT);
  pinMode(ldrPin, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str()); // anonymous
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}


///////////////////////////////////////////////////////////////////////////////**VOID LOOP**/////////////////////////////////////////////////////////////////////////////
void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    int ldrStatus = analogRead(ldrPin);
    int state = digitalRead(sensor);
    Serial.print("LDR: ");
    Serial.println(ldrStatus);

    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setFloat(&fbdo, "DHT 11/hum", dht.readHumidity())) {
      //      Serial.print("Distance (hum): ");
      //      Serial.println(hum);
      //
      //      Serial.println("PASSED");
      //      Serial.println("PATH: " + fbdo.dataPath());
      //      Serial.println("TYPE: " + fbdo.dataType());

    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }



    /*
      Firebase.RTDB.setInt(&fbdo, "HC-SR04/ultra", 78);
      count++;
    */
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)

Serial.print("Tempreture is " );
Serial.print(t);
Serial.println("°C");

    if (t >= 25) {
      
      digitalWrite(fan, HIGH);
      Serial.print("FAN ON\n");
      Firebase.RTDB.setString(&fbdo, "FAN", "Open");
    }
    else {
      digitalWrite(fan, LOW);
      Firebase.RTDB.setString(&fbdo, "FAN", "Close");
      //Serial.print("FAN OFF");
    }

    if (state == HIGH && ldrStatus <= 20) {
      digitalWrite(ledPin, HIGH);
      Firebase.RTDB.setString(&fbdo, "LED", "Open");
      Serial.println("LED ON");
      //Serial.print(ldrStatus);
      // Serial.println("LDR is DARK and There is motion, LED is ON");

    }
    else {
      digitalWrite(ledPin, LOW);
      Serial.println("LED OFF");
      Firebase.RTDB.setString(&fbdo, "LED", "OFF");
      //Serial.println("No need for light, LED is OFF");
    }

    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "DHT 11/temp", t )) {
      //Serial.print("Distance (temp):  ");
      //Serial.println(temp);

      //Serial.println("PASSED");
      //Serial.println("PATH: " + fbdo.dataPath());
      //Serial.println("TYPE: " + fbdo.dataType());

    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    /////////////////////////////////LDR//////////////////////////////////
    if (Firebase.RTDB.setFloat(&fbdo, "light/ldr", ldrStatus)) {
//      Serial.print("LUX: ");
//      Serial.println(ldrStatus);
//      Serial.println("PASSED");
//      Serial.println("PATH: " + fbdo.dataPath());
//      Serial.println("TYPE: " + fbdo.dataType());

    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    /////////////////////////////////pir//////////////////////
    if (Firebase.RTDB.setFloat(&fbdo, "light/pir", state)) {
//      Serial.print("state: ");
//      Serial.println(state);
//      Serial.println("PASSED");
//      Serial.println("PATH: " + fbdo.dataPath());
//      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

  }


  delay(200);
}
