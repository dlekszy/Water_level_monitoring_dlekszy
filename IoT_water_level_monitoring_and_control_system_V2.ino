//IoT water level monitoring and control system

#define BLYNK_TEMPLATE_ID "TMPL2ZLi28ONx"
#define BLYNK_TEMPLATE_NAME "IoT water level monitoring and control system"
#define BLYNK_AUTH_TOKEN "HluAkU6PyAhgK7Y7Jt4nJVu9rs5E1SSb"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "Airtel_4G_SMARTBOX_0C2E";
char pass[] = "@dleksy57";

int emptyTankDistance = 57;
int fullTankDistance = 30;
int pumpTriggerPer = 10;
int offTriggerPer = 95; 

#define TRIG 12                //D6
#define ECHO 13                //D7
#define Relay 14           //D5 

#define V_B_1 V1
#define V_B_3 V3
#define V_B_4 V4

LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16x2 display

float duration;
float distance;
int waterLevelPer;

bool toggleRelay = false;
bool modeFlag = true;
String currMode;

char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;

void checkBlynkStatus() {
  bool isconnected = Blynk.connected();
  if (isconnected == false) {
  }
  if (isconnected == true) {
  }
}

BLYNK_WRITE(VPIN_BUTTON_3) {
  if (param.asInt() == 1) {
    modeFlag = true;
    currMode = "AUTO";
    trackStateMode = 1;
  } else {
    modeFlag = false;
    currMode = "MANUAL";
    if ((trackStateMode + 1) == 2) {  //Track the initial state of the manual mode activation only and reset the relay to LOW
      digitalWrite(Relay, LOW);
      trackStateMode = 0;  //restore the state.
    }
  }
  Serial.print(currMode);
  displayData();  // Update LCD with the new mode
}

BLYNK_WRITE(VPIN_BUTTON_4) {
  toggleRelay = param.asInt();  //this was brought from the block of the below if(!modeFLag) to this position because it has to be recording every actions of virtual pin button state regardless of if/else condition.
  if (!modeFlag) {  //if manual mode is set, then this code block executes.
    digitalWrite(Relay, toggleRelay);
  } else {  //else, ON/OFF virtual indicator to signify that Auto Mode is running.
    Blynk.virtualWrite(V_B_4, toggleRelay);
  }
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V_B_1);
  Blynk.virtualWrite(V_B_3, modeFlag);
  Blynk.virtualWrite(V_B_4, toggleRelay);
}

void displayData() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Water Level: ");
  lcd.print(waterLevelPer);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Mode: ");
  lcd.print(currMode);
  lcd.setCursor(9, 1);
  lcd.print("Relay: ");
  lcd.print(toggleRelay ? "ON" : "OFF");
}

void measureDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(20);
  digitalWrite(TRIG, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = ((duration / 2) * 0.343) / 10;
  if (distance > fullTankDistance && distance < emptyTankDistance) {
    waterLevelPer = map((int)distance, emptyTankDistance, fullTankDistance, 0, 100);
    Blynk.virtualWrite(V_B_1, waterLevelPer);
    if (waterLevelPer <= pumpTriggerPer) {
      if (modeFlag) {
        if (!toggleRelay) {
          digitalWrite(Relay, HIGH);
          toggleRelay = true;
          Blynk.virtualWrite(V_B_4, toggleRelay);
        }
      }
    }
    if (waterLevelPer >= offTriggerPer) {
      if (modeFlag) {
        if (toggleRelay) {
          digitalWrite(Relay, LOW);
          toggleRelay = false;
          Blynk.virtualWrite(V_B_4, toggleRelay);
        }
      }
    }
  }
  displayData();
  delay(100);
}

void setup() {
  Serial.begin(9600);
  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(Relay, OUTPUT);

  lcd.init();                      // Initialize the LCD
  lcd.backlight();                 // Turn on the backlight

  currMode = modeFlag ? "AUTO" : "MANUAL";

  WiFi.begin(ssid, pass);
  timer.setInterval(2000L, checkBlynkStatus);
  timer.setInterval(1000L, measureDistance);
  Blynk.config(auth);
  delay(1000);

  Blynk.virtualWrite(V_B_3, modeFlag);
  Blynk.virtualWrite(V_B_4, toggleRelay);

  delay(500);
}

void loop() {
  Blynk.run();
  timer.run();

  // Read serial data
  String readString;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {  // Check for end of message
      // Process the received message
      if (readString.startsWith("PTP:")) {  // Check for pump trigger percentage
        int value = readString.substring(4).toInt();  // Extract the value part of the message
        if (value >= 10 && value <= 40) {  // Check if the value is within the valid range
          pumpTriggerPer = value;
        } 
        //else {
       //   Serial.println("Invalid pump trigger percentage");
       // }
      } else if (readString.startsWith("OTP:")) {  // Check for off trigger percentage
        int value = readString.substring(4).toInt();  // Extract the value part of the message
        if (value >= 41 && value <= 95) {  // Check if the value is within the valid range
          offTriggerPer = value;
        } 
        //else {
       //   Serial.println("Invalid off trigger percentage");
       // }
      }
      //else {
      //  Serial.println("Invalid message format");
      //}
      readString = "";  // Clear the string buffer for the next message
    } else {
      readString += c;  // Append the received character to the string buffer
    }
  }

}
