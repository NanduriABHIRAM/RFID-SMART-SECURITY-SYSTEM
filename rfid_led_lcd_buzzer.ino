#include <SPI.h>//enable communication with RFID reader
#include <MFRC522.h>// RFID SENSOR
#include <LiquidCrystal_I2C.h> //LCD pannel
#include <Servo.h>//Servo motor

#define RST_PIN 9
#define SS_PIN 10
#define SERVO_PIN 4 // Pin for the servo motor
#define RED_LED_PIN 5
#define GREEN_LED_PIN 6
#define BUZZER_PIN 7

// Initialize LCD at I2C address 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// RFID Reader instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Servo instance
Servo myServo;

String MasterTag = "271D966"; //  your authorized tag ID
String tagID = "";

void setup() {
  // Initialize components
  lcd.init();
  lcd.backlight();
  SPI.begin();
  mfrc522.PCD_Init();
  myServo.attach(SERVO_PIN);
  myServo.write(0); // Set servo to initial position (0 degrees)

  // Configure LED and buzzer pins as output
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Turn off LEDs and buzzer initially
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Display initial message
  lcd.clear();
  lcd.print(" Scan Your ID ");
  lcd.setCursor(0, 1);
  lcd.print(">>");
}

void loop() {
  // Wait for a new card
  if (getID()) {
    lcd.clear();
    lcd.setCursor(0, 0);

    // Check if the tag matches the MasterTag
    if (tagID == MasterTag) {
      lcd.print("Access Approved");
      accessGranted(); // Handle access approval
    } else {
      lcd.print("Access Denied");
      accessDenied(); // Handle access denial
    }

    lcd.setCursor(0, 1);
    lcd.print("ID: ");
    lcd.print(tagID); // Display the scanned tag's ID

    delay(3000); // Display result for 3 seconds

    // Reset message for next scan
    lcd.clear();
    lcd.print(" Scan Your ID ");
    lcd.setCursor(0, 1);
    lcd.print(">>");
  }
}

// Function to read a new RFID tag
boolean getID() {
  // Check if a new card is present
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return false;
  }

  // Read the tag ID
  tagID = "";
  for (uint8_t i = 0; i < mfrc522.uid.size; i++) {
    tagID += String(mfrc522.uid.uidByte[i], HEX);
  }

  tagID.toUpperCase(); // Convert to uppercase
  mfrc522.PICC_HaltA(); // Halt the card
  return true;
}

// Function to handle access approval
void accessGranted() {
  digitalWrite(GREEN_LED_PIN, HIGH); // Turn on green LED
  digitalWrite(BUZZER_PIN,  HIGH);   // Turn on buzzer
  delay(100);                      // Keep buzzer on for 1 second
  digitalWrite(BUZZER_PIN, LOW);    // Turn off buzzer
  delay(500);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  delay(500);
  digitalWrite(GREEN_LED_PIN, LOW); // Turn off green LED
  rotateServo();                    // Rotate the servo
}

// Function to handle access denial
void accessDenied() {
  digitalWrite(RED_LED_PIN, HIGH);  // Turn on red LED
  digitalWrite(BUZZER_PIN, HIGH);   // Turn on buzzer
  delay(1500);                      // Keep buzzer on for 1 second
  digitalWrite(BUZZER_PIN, LOW);    // Turn off buzzer
  delay(1000);                      // Keep red LED on for an additional second
  digitalWrite(RED_LED_PIN, LOW);   // Turn off red LED
}

// Function to rotate the servo motor
void rotateServo() {
  myServo.write(90); // Rotate servo to 90 degrees
  delay(2000);       // Keep it open for 2 seconds
  myServo.write(0);  // Return servo to initial position
}
