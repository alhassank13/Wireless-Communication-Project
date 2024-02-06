

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>

WiFiClient client;


//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"



//////////////////////////////////////// WIFI ////////////////////
// Insert your network credentials
#define WIFI_SSID "Hassan"
#define WIFI_PASSWORD "Aa123456790"

/*
  // Insert your network credentials
  #define WIFI_SSID "The MonsteR"
  #define WIFI_PASSWORD "K13553067"
*/

//////////////////////////////////////// FIREBASE ////////////////////
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



#include <Servo.h>

Servo servo;



int PUMP = D7; // choose pin for the LED
int inputPin = D3; // choose input pin (for Infrared sensor)
int val = 0; // variable for reading the pin status
#define echoPin D2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin D1 //attach pin D3 Arduino to pin Trig of HC-SR04
int ledservo = D6;
// defines variables
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement


///////////////////////////////////////////////////////////////////////////////**VOID LOOP**/////////////////////////////////////////////////////////////////////////////
void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(115200); // // Serial Communication is starting with 9600 of baudrate speed
  Serial.println("Ultrasonic Sensor HC-SR04 Test"); // print some text in Serial Monitor
  Serial.println("with Arduino UNO R3");
  Serial.begin(115200);
  pinMode(PUMP, OUTPUT); // declare LED as output
  pinMode(inputPin, INPUT); // declare Infrared sensor as input
  pinMode(ledservo, OUTPUT);
  servo.attach(D8);

  servo.write(0);

  delay(2000);
  ///////////////////WIFI/////////////////////

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

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 3000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();


    ////////////////////////////////////// Ultrasonic ////////////////////////////////////////

    // Clears the trigPin condition
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    // Displays the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    //////////////////////////////////////  Servo   //////////////////////////////////////
    if (distance < 10)
    {
      servo.write(180);
      Serial.println("DOOR IS OPEN");
      if (Firebase.RTDB.setString(&fbdo, "DOOR", "Open")) {
        //        Serial.println("PASSED");
        //        Serial.println("PATH: " + fbdo.dataPath());
        //        Serial.println("TYPE: " + fbdo.dataType());
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }

      digitalWrite(ledservo, HIGH);
      delay(3000);
      servo.write(0);
      Serial.println("DOOR IS Close");
      if (Firebase.RTDB.setString(&fbdo, "DOOR", "Close")) {
        //        Serial.println("PASSED");
        //        Serial.println("PATH: " + fbdo.dataPath());
        //        Serial.println("TYPE: " + fbdo.dataType());
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
      delay(300);
      digitalWrite(ledservo, LOW);
    }
    else
    {
      servo.write(0);
      Serial.println("DOOR IS Close");
      delay(300);
      digitalWrite(ledservo, LOW);

    }

    ///////////////////////////////////////IR///////////////////////////////
    val = digitalRead(inputPin); // read input value

    if (val == HIGH)
    { // check if the input is HIGH

      digitalWrite(PUMP, LOW); //
      Serial.println("Pump IS Close");

      if (Firebase.RTDB.setString(&fbdo, "PUMP", "Close")) {
        //        Serial.println("PASSED");
        //        Serial.println("PATH: " + fbdo.dataPath());
        //        Serial.println("TYPE: " + fbdo.dataType());

      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }

    }
    else
    {
      digitalWrite(PUMP, HIGH); // turn LED ON
      Serial.println("Pump IS Open");
      if (Firebase.RTDB.setString(&fbdo, "PUMP", "Open")) {
        //        Serial.println("PASSED");
        //        Serial.println("PATH: " + fbdo.dataPath());
        //        Serial.println("TYPE: " + fbdo.dataType());
      }

      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }

    }
  }
  delayMicroseconds(300);
}
