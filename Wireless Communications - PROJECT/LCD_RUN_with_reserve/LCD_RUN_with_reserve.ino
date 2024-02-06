

#include <ESP8266WiFi.h>

#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

//  // Insert your network credentials
//  #define WIFI_SSID "ThE MonsteR"
//  #define WIFI_PASSWORD "K13553067"
//////////////////////////////////////// WIFI ////////////////////
// Insert your network credentials
#define WIFI_SSID "Hassan"
#define WIFI_PASSWORD "Aa123456790"
/*
  // Insert your network credentials
  #define WIFI_SSID "NU-MECT"
  #define WIFI_PASSWORD "Nmect_U@2018"

*/
// Insert Firebase project API Key
#define API_KEY "AIzaSyDOpG6ZPgepDOW5JqkgbEZYnuLkvR8RhY0"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "digitalcommunicationsk13-default-rtdb.firebaseio.com"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

float cmValue;
float inchValue;
float temp;
float hum;


bool signupOK = false;



#include <LiquidCrystal_I2C.h>

// set the LCD number of columns and rows
int lcdColumns = D1;
int lcdRows = D2;
int upButton = D3;
int downButton = D4;
int selectButton = D5;
int menu = 1;
int intro = 1;
// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

//defining House Icon
byte housechar1[8] = {
  B00000,
  B00001,
  B00011,
  B00011,  //Row 0, Col 0
  B00111,
  B01111,
  B01111,
  B11111,
};
byte housechar2[8] = {
  B11111,
  B11111,
  B11100,
  B11100,  //Row 1, Col 0
  B11100,
  B11100,
  B11100,
  B11100,
};
byte housechar3[8] = {
  B00000,
  B10010,
  B11010,
  B11010,  //ROW 0, Col 1
  B11110,
  B11110,
  B11110,
  B11111,
};
byte housechar4[8] = {
  B11111,
  B11111,
  B11111,
  B10001,  //Row 1, Col 1
  B10001,
  B10001,
  B11111,
  B11111,
};

void setup() {
  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);
  updateMenu();
  Serial.begin(115200);
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
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  lcd.setCursor(0, 0);
  lcd.print("    Physical    ");
  lcd.setCursor(0, 1);
  lcd.print("    Sensors   ");
  delay(3000);
  lcd.clear();
  lcd.createChar(1, housechar1);
  lcd.createChar(2, housechar2);
  lcd.createChar(3, housechar3);
  lcd.createChar(4, housechar4);
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.setCursor(0, 1);
  lcd.write(2);
  lcd.setCursor(1, 0);
  lcd.write(3);
  lcd.setCursor(1, 1);
  lcd.write(4);
  lcd.setCursor(3, 0);
  lcd.print("Smart Home");
  delay(3000);
  lcd.clear();
  lcd.print(">Temperature");
  lcd.setCursor(0, 1);
  lcd.print(" DOOR");
  delay(3000);
}


void loop() {

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 3000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();


    if (!digitalRead(downButton)) {
      menu++;
      updateMenu();
      delay(100);
      while (!digitalRead(downButton))
        ;
    }
    if (!digitalRead(upButton)) {
      menu--;
      updateMenu();
      delay(100);
      while (!digitalRead(upButton))
        ;
    }
    if (!digitalRead(selectButton)) {
      executeAction();
      updateMenu();
      delay(100);
      while (!digitalRead(selectButton))
        ;
    }
    //////////////////////////////////////  DHT   //////////////////////////////////////

    Firebase.RTDB.getFloat(&fbdo, "/DHT 11/temp");
    temp = fbdo.floatData();
    Serial.print("temperature in celesius : ");
    Serial.print(temp);
    Serial.println(" celesius");

    //////////////////////////////////////  Servo   //////////////////////////////////////

    Firebase.RTDB.getString(&fbdo, "DOOR");
    Serial.print("DOOR IS ");
    Serial.println(fbdo.stringData());


    //////////////////////////////////////  IR   //////////////////////////////////////

    Firebase.RTDB.getString(&fbdo, "PUMP");
    Serial.print("PUMP IS ");
    Serial.println(fbdo.stringData());

    //////////////////////////////////////  PIR   //////////////////////////////////////

    Firebase.RTDB.getString(&fbdo, "LED");
    Serial.print("LED ");
    Serial.println(fbdo.stringData());

    //////////////////////////////////////  FAN   //////////////////////////////////////

    Firebase.RTDB.getString(&fbdo, "FAN");
    Serial.print("FAN ");
    Serial.println(fbdo.stringData());
  }
  delay(3000);
}

void updateMenu() {
  // sowitch (intr) {
  //   case 0:
  //     lcd.setCursor (0, 0);
  //     lcd.print ("    Physical    ");
  //     lcd.setCursor (0, 1);
  //     lcd.print ("    Sensor   ");
  //     delay (3000);
  //     lcd.clear();
  //     lcd.createChar(1, housechar1);
  //     lcd.createChar(2, housechar2);
  //     lcd.createChar(3, housechar3);
  //     lcd.createChar(4, housechar4);
  //     lcd.setCursor(0, 0);
  //     lcd.write(1);
  //     lcd.setCursor(0, 1);
  //     lcd.write(2);
  //     lcd.setCursor(1, 0);
  //     lcd.write(3);
  //     lcd.setCursor(1, 1);
  //     lcd.write(4);
  //     lcd.setCursor(3, 0);
  //     lcd.print("Smart Home");
  //     delay (3000);
  //     break;
  // }
  switch (menu) {
    case 0:
      menu = 1;
      break;
    case 1:
      lcd.clear();
      lcd.print(">TEMPERATURE");
      lcd.setCursor(0, 1);
      lcd.print(" DOOR");
      break;
    case 2:
      lcd.clear();
      lcd.print(" TEMPERATURE");
      lcd.setCursor(0, 1);
      lcd.print(">DOOR");
      break;
    case 3:
      lcd.clear();
      lcd.print(">PUMP");
      lcd.setCursor(0, 1);
      lcd.print(" FAN");
      break;
    case 4:
      lcd.clear();
      lcd.print(" PUMP");
      lcd.setCursor(0, 1);
      lcd.print(">FAN");
      break;
    case 5:
      lcd.clear();
      lcd.print(">LED");
      break;
    case 6:
      menu = 6;
      break;
  }
}

void executeAction() {
  switch (intro) {
    case 1:
      action1();
      break;
  }
  switch (menu) {
    case 1:
      action1();
      break;
    case 2:
      action2();
      break;
    case 3:
      action3();
      break;
    case 4:
      action4();
      break;
    case 5:
      action5();
      break;
  }
}

void action1() {
  lcd.clear();
  lcd.print(">TEMPERATURE #1");
  Firebase.RTDB.getFloat(&fbdo, "/DHT 11/temp");
  temp = fbdo.floatData();
  lcd.setCursor(0, 1);
  lcd.print(temp);
  delay(1500);
}
void action2() {
  lcd.clear();
  lcd.print(">DOOR #2");
  lcd.setCursor(0, 1);
  Firebase.RTDB.getString(&fbdo, "DOOR");
  lcd.print(fbdo.stringData());
  delay(1500);
}
void action3() {
  lcd.clear();
  lcd.print(">PUMP #3");
  Firebase.RTDB.getString(&fbdo, "PUMP");
  lcd.setCursor(0, 1);
  lcd.print(fbdo.stringData());
  delay(1500);
}
void action4() {
  lcd.clear();
  lcd.print(">FAN #4");
  Firebase.RTDB.getString(&fbdo, "FAN");
  lcd.setCursor(0, 1);
  lcd.print(fbdo.stringData());
  delay(1500);
}
void action5() {
  lcd.clear();
  lcd.print(">LED #5");
  Firebase.RTDB.getString(&fbdo, "LED");
  lcd.setCursor(0, 1);
  lcd.print(fbdo.stringData());
  delay(1500);
}
