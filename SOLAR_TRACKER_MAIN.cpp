/* 
 * Project: SOLAR TRACKER
 * Author: Zac Cresencio \\ crese002
 * Date: 4/28/2026
 * EE1301, John Sartori
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "cmath"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC); // CHANGE ME TO AUTOMATIC

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB  
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

// Pins
const int LDR_L_PIN = A0;
const int LDR_R_PIN = A1;
const int SERVO_PIN = A2;
const int POT_PIN = A5;
const int BUTTON_PIN = D2;
const int MODE_LED_PIN = D3;
const int VDIV_PIN = D1;

// Create Servo
Servo servo;

// Timer for LDRs, Data, Button Debounce
unsigned long prev_ms_LDR = 0;
const unsigned long interval_LDR = 10; // .01 seconds

unsigned long prev_ms_DATA = 0;
const unsigned long interval_DATA = 5000; // 5 seconds

unsigned long prev_ms_BUTTON = 0;
const unsigned long interval_BUTTON = 200; // 200ms delay for debounce

// Vars
float servoPos = 90; // FIX ME, CREATE FILE TO STORE SERVO DATA
float servoPosIot = 90;
int ldrLVal, ldrRVal;
double voltage = 0; 
double current = 0; 
double power_mW = 0;

// logic vars
enum Direction {
  LEFT,
  RIGHT,
  CENTERED
};

Direction lightDetected = CENTERED;
int HYSTERSIS = 50;
int ldrDelta;

// Autotracking vs Manual Input
bool autoTracking = true;

// Button logic
bool currButton = LOW;
bool prevButton = LOW;

// Pot Logic
int currPot = 0;
int prevPot = 0;
int pot_HYST = 25;
bool pot_moved = false;

// Cloud Functions
int setPosFromString(String inputString);
int setModeFromString(String inputString);

void setup() {
  // pinModes
  pinMode(LDR_L_PIN, INPUT);
  pinMode(LDR_R_PIN, INPUT);
  pinMode(POT_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(VDIV_PIN, INPUT);
  pinMode(MODE_LED_PIN, OUTPUT);

  prevPot = analogRead(POT_PIN);

  // Servo begin
  servo.attach(SERVO_PIN);
  servo.write(servoPos);

  // Serial monitor
  Serial.begin(9600);

  // CLOUD VARIABLES
  Particle.variable("voltageVal", voltage);
  Particle.variable("currentVal", current);
  Particle.variable("powerVal", power_mW);

  // CLOUD FUNCTION
  Particle.function("solar_setPos", setPosFromString);
  Particle.function("solar_setMode", setModeFromString);
}

void loop() {
  
  unsigned long now_ms = millis();

  // -- AUTO TRACKING MODE -- //
  if (autoTracking == true)
  {
    // LDR Loop
    if (now_ms - prev_ms_LDR >= interval_LDR)
    {
      prev_ms_LDR = now_ms;
      
      // Calculate the difference in light
      ldrLVal = analogRead(LDR_L_PIN);
      ldrRVal = analogRead(LDR_R_PIN);
      ldrDelta = ldrLVal - ldrRVal;

      // Send signal to move accordingly
      if (ldrDelta > HYSTERSIS)
      {
        lightDetected = LEFT;   // move servo left
        servoPos -= 0.5;
        servoPos = constrain(servoPos, 1, 179);
        servo.write(servoPos);
        // Serial.println("LEFT");
      }
      else if (ldrDelta < -(HYSTERSIS))
      {
        lightDetected = RIGHT;  // move servo right
        servoPos += 0.5;
        servoPos = constrain(servoPos, 1, 179);
        servo.write(servoPos);
        // Serial.println("RIGHT");
      }
      else
      {
        lightDetected = CENTERED;
        servoPos += 0;
        servoPos = constrain(servoPos, 1, 179);
        servo.write(servoPos);
        // Serial.println("CENTERED");
      }
    }

    // Turn OFF LED that signifies manual control.
    digitalWrite(MODE_LED_PIN, LOW);

  }
  
  // -- MANUAL INPUT MODE -- //
  else if (autoTracking == false)
  {
    // Read from potentiometer
    currPot = analogRead(POT_PIN);

    // Detect if user is controlling through potentiometer
    if (currPot > prevPot + pot_HYST || currPot < prevPot - pot_HYST)
    {
      pot_moved = true;
    }
    prevPot = currPot;

    // Manual Control VIA Potentiometer
    if (pot_moved)
    {
      // Convert potentiometer value to 0-180 degrees, then constrain
      servoPos = constrain((currPot / 4095.0 * 180.0), 1, 179);
      servo.write(servoPos);
    }

    // Manual Control VIA Web
    else
    {
      servo.write(servoPosIot);
    }

    // Turn ON LED that signifies manual control.
    digitalWrite(MODE_LED_PIN, HIGH);
  }

  // DATA Loop, to send data to the cloud.
    if (now_ms - prev_ms_DATA >= interval_DATA)
    {
      prev_ms_DATA = now_ms;

      voltage = analogRead(VDIV_PIN) * (3.3 / 4095) * 2;
      current = (voltage / 2000) * 1000;
      power_mW = ( (voltage * voltage) / 2000) * 1000;

      // debugging purposes
      Serial.printf("Voltage: %.3f V | Current: %.4f mA | Power: %.3f mW\n",
                    voltage,
                    current,
                    power_mW);
    }

  // Toggle mode with a button
    currButton = digitalRead(BUTTON_PIN);
    if (currButton == HIGH && prevButton == LOW && (now_ms - prev_ms_BUTTON >= interval_BUTTON))
    {
      Serial.println("Button pressed");
      autoTracking = !autoTracking;  // Toggle between auto tracking and non auto tracking
      prev_ms_BUTTON = now_ms;
    }
    prevButton = currButton;

}

// Fetch data from website for servo position
int setPosFromString(String inputString)
{
  autoTracking = false;
  pot_moved = false;

  int pos = inputString.toInt();
  pos = constrain(pos, 1, 179);
  servoPosIot = pos;
  servo.write(servoPosIot);
  return pos;
}

// Fetch data from website for auto/manual mode
int setModeFromString(String inputString)
{
  if (inputString == "auto")
  {
    autoTracking = true;
    return 1;
  }
  if (inputString == "manual")
  {
    autoTracking = false;
    return 0;
  }
  
  return -1;
  
}
