
#include <Arduino.h>
#include "BasicStepperDriver.h"

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define RPM 7

// Since microstepping is set externally, make sure this matches the selected mode
// If it doesn't, the motor will move at a different RPM than chosen
// 1=full step, 2=half step etc.
#define MICROSTEPS 16

// All the wires needed for full functionality
#define DIR 12
#define STEP 14
//Uncomment line to use enable/disable functionality
//#define SLEEP 13

// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP);

//Uncomment line to use enable/disable functionality
//BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP, SLEEP);
char DataIn;  
int wait = 1000;
String readString;

void setup() {
    
    Serial.begin(115200);
    stepper.begin(RPM, MICROSTEPS);
    // if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line
    // stepper.setEnableActiveState(LOW);
    stepper.setSpeedProfile(stepper.LINEAR_SPEED, 500, 500);
    delay(500);
  
}

void loop() {
  
  while (Serial.available()){  
    DataIn = Serial.read();       // String DataIn receives the data typed in the Serial Monitor
    readString += DataIn;
    delay(2);  //delay to allow byte to arrive in input buffer
  }

  if (readString.length()>0) {
    Serial.println(readString);
    
    if (readString.length()>1){
      String mode = readString.substring(0, 1);
      String deg = readString.substring(1, 4); //get the second four characters
      
      if (mode == "A"){
        Serial.print("CW ");
        stepper.rotate(deg.toInt());
        delay(wait);
      }else if (mode =="B"){
        Serial.print("CCW ");
        stepper.rotate(-1*deg.toInt());
        delay(wait);
      }
      Serial.println(deg + "degree");
        readString = "";
      Serial.println("s");
    }

  }

}
