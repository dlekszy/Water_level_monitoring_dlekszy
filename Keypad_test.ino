#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

int pumpTriggerPer;
int offTriggerPer;

String readString;
String Q;

byte data_count = 0;

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2); // Initialize the LCD with the I2C address 0x27, and 16 columns and 2 rows

String inputBuffer = ""; // Buffer to store entered digits
bool enteringValue = false; // Flag to indicate whether the user is entering a value

void setup() {
  Serial.begin(9600);
  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
  lcd.print("Press C to adjust"); // Initial message
}

void loop() {
  char key = keypad.getKey();
  if (key == 'C') { // Enter threshold adjustment mode
    lcd.clear();
    lcd.print("Press A for Max");
    lcd.setCursor(0, 1);
    lcd.print("Press B for Min");
    
    bool adjusting = true;
    while (adjusting) {
      char adjustKey = keypad.getKey();
      if (adjustKey != NO_KEY) {
        if (adjustKey == 'A') {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Enter Max Level");
          lcd.setCursor(0, 1);
          lcd.print("Press D to Confirm");
          inputBuffer = "";
       ///   int data_count = 0;
          while (true) {
            char levelKey = keypad.getKey();
            if (levelKey != NO_KEY && levelKey != 'D') {
               if (levelKey == '*') { // Backspace action
                if (inputBuffer.length() > 0) {
                  inputBuffer.remove(inputBuffer.length() - 1); // Remove the last character
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Enter Max Level");
                  lcd.setCursor(0, 1);
                  lcd.print(inputBuffer); // Print the updated input buffer
                }
              }else if(levelKey == '#'){
               adjusting = false;
               lcd.clear();
               lcd.print("Press A for Max");
               lcd.setCursor(0, 1);
               lcd.print("Press B for Min");
               break; 
              }else{
              inputBuffer += levelKey;
              lcd.clear();
              //lcd.setCursor(data_count, 0);
              lcd.setCursor(0, 1);
              lcd.print(inputBuffer);
              }
              data_count++;
             // if (data_count >= 2) { // Max digits reached
               // break;  
              //}
              lcd.setCursor(0, 1);
            } else if (levelKey == 'D') {
                if(inputBuffer.toInt() >= 41 && inputBuffer.toInt() <= 95){
              // Confirm the entered value and exit adjustment mode
              adjusting = false;
              // Process the input buffer, validate and update the maximum level
              // For example: maxLevel = inputBuffer.toInt();
              offTriggerPer = inputBuffer.toInt();
              Serial.print("OTP:");
              Serial.println(offTriggerPer);
              break;
                }else{
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Invalid value");
                    lcd.setCursor(0, 1);
                    lcd.print("Range is 41-95");
                   }
            }
          }
        } else if (adjustKey == 'B') {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Enter Min Level");
          lcd.setCursor(0, 1);
          lcd.print("Press D to Confirm");
          inputBuffer = "";
       ///   int data_count = 0;
          while (true) {
            char levelKey = keypad.getKey();
            if (levelKey != NO_KEY && levelKey != 'D') {
                if (levelKey == '*') { // Backspace action
                    if (inputBuffer.length() > 0) {
                    inputBuffer.remove(inputBuffer.length() - 1); // Remove the last character
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Enter Max Level");
                    lcd.setCursor(0, 1);
                    lcd.print(inputBuffer); // Print the updated input buffer
                  }
                }else if(levelKey == '#'){
                 adjusting = false;
                 lcd.clear();
                 lcd.print("Press A for Max");
                 lcd.setCursor(0, 1);
                 lcd.print("Press B for Min");
                 break; 
                }else{
                 inputBuffer += levelKey;
                 lcd.clear();
                 //lcd.setCursor(data_count, 0);
                 lcd.setCursor(0, 1);
                 lcd.print(inputBuffer);
                }
                 data_count++;
                 // if (data_count >= 2) { // Max digits reached
                 // break;  
                 //}
                 lcd.setCursor(0, 1);
              } else if (levelKey == 'D') {
                 // Confirm the entered value and exit adjustment mode
                   if(inputBuffer.toInt() >= 10 && inputBuffer.toInt() <= 40){
                   adjusting = false;
                   // Process the input buffer, validate and update the maximum level
                   // For example: maxLevel = inputBuffer.toInt();
                   pumpTriggerPer = inputBuffer.toInt();
                   Serial.print("PTP:");
                   Serial.println(pumpTriggerPer);
                   break;
                   }else{
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Invalid value");
                    lcd.setCursor(0, 1);
                    lcd.print("Range is 10-40");
                   }
            }
          }
        }
      }
    }
    lcd.clear();
    lcd.print("Exiting...");
    lcd.setCursor(0, 1);
    lcd.print("Press C");
    delay(1000); // Provide visual feedback before exiting adjustment mode
  }
  
 }
