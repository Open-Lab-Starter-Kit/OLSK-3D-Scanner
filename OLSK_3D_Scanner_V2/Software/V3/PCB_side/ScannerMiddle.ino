/////////////////////////////////INCLUDES////////////////////////////////////////////

#include <Adafruit_NeoPixel.h>
#define LED_COUNT 60
#include <EEPROM.h>

//CONSTANTS

//DEBUG
#define DEBUGGING_BASE 0
#define DEBUGGING_DEEP 0
#define DEBUGGING_MACRO_BUFFER 0
#define BENCHMARKING 0
#define BENCHMARK_TRESHOLD_MS 1
//if(DEBUGGING){USBSerialPrintln();}


#define START_DELAY_TIME 1000

//IOs
#define RGB_PIN 6


//SERIAL CONSTANTS
#define SERIAL_BUFFER_SIZE 256
#define USB_SERIAL_BAUD_RATE 115200
#define TEENSY_BAUD_RATE 115200

//CHARS AND STRINGS

//GRBL
#define GRBL_INTERROGATIVE '?'
#define GRBL_TILDE '~'
#define GRBL_ESCLAMATIVE '!'

#define EQUAL_CHAR '='
#define TRUE "true"
#define FALSE "false"

const byte GRBL_CTRLX = 24;

//PING for interface
#define PING "PING"

//OUT going commands
#define TAKE_PICTURE "TAKE_PICTURE"  //command to say to the software to take a picture
#define END_OF_JOB "JOB_END"  //command to say to software that job is finish
#define START_OF_JOB "STARTED"  //command to say to software that job is started
#define PAUSE_OF_JOB "PAUSED"  //command to say to software that job is started

//this one is separated
#define JOB_RESUMED "RESUMED"  //command to say to software that job is resumed

#define OUTGOING_COMMAND_AMOUNT 4

const char OUTGOING_COMMANDS[OUTGOING_COMMAND_AMOUNT][20] = {

  TAKE_PICTURE,        //case 0 TAKE_PICTURE
  START_OF_JOB,          //case 1 END_OF_JOB
  END_OF_JOB,        //case 2 START_OF_JOB
  PAUSE_OF_JOB,        //case 3 PAUSE_OF_JOB

};


//PICO available commands
#define STOPS_ROTATION "STOPS_ROTATION"  //defines how many stops are executed for taking pictures during each single rotation, dividing in equal parts the circle, example PR=4 will make the stops at 90, 180, 270 and 360 degrees
#define ROTATION_SPEED "SPEED"  //defines the speed of rotation, expressed in % rather than mm/min or radiants/seconds, example RS=50
#define RGB_SETTINGS "RGB"   //defines the RGB colours and power, power is 0-100, colours are 0-255, example RBG=200,200,200,80
#define HOMING "HOMING"      //executes the homing cycle for the arc; does not use any parameter
#define PICTURES_ARC "PICTURES_ARC"    //defines how many pictures are going to be taken at each single stop of the plate, dividing the arc in equal portions
#define START "START"        //starts the motion for collecting the pictures of a single cycle of plate and arc motion
#define STOP "STOP"          //stops the current scanning cycle if active and cancels the remaining commands
#define PAUSE "PAUSE"        //pauses the current scanning cycle if active
#define RESUME "RESUME"        //resumes the current scanning cycle if active
#define PRINT_INFO "INFO"        //resumes the current scanning cycle if active
#define RESUME_PICTURE "RESUME_PICTURE"        //resumes the current scanning cycle if active


#define PICO_COMMAND_AMOUNT 11

const char PICO_COMMANDS[PICO_COMMAND_AMOUNT][20] = {

  STOPS_ROTATION,  //case 0 STOPS_ROTATION
  ROTATION_SPEED,  //case 1 ROTATION_SPEED
  RGB_SETTINGS,    //case 2 RGB_SETTINGS
  HOMING,          //case 3 HOMING
  PICTURES_ARC,    //case 4 PICTURES_ARC
  START,           //case 5 START
  STOP,            //case 6 STOP
  PAUSE,           //case 7 PAUSE
  RESUME,           //case 8 RESUME
  PRINT_INFO,   //case 9 print settings
  RESUME_PICTURE, //case 10 resume picture

};



#define SYNC_ACTION_AMOUNT 10
#define SYNC_ACTION_CHARS 10

typedef struct {

  char type[SYNC_ACTION_CHARS];
  float positionX;
  float positionY;
  float positionZ;
  float positionRange;
  int pollingInterval;

} syncAction;

syncAction syncActions[SYNC_ACTION_AMOUNT];


//GRBL GCODE MACROS, corresponding to PICO_COMMANDS lines
//special chars are used to be substituted later with parametrized values
//special chars can be added only at the end of the strings
//! will be changed with Z feed rate speed -> for later!
//? will be changed with XY feed rate speed -> for later!
//we use the commands directly in the macro for PICO commands, check the defines
#define BUFFER_MACRO_LINES 500
#define BUFFER_MACRO_CHARS 50

char bufferMacroLines[BUFFER_MACRO_LINES][BUFFER_MACRO_CHARS] = {};

//GRBL RESPONSES
#define GRBL_MPOS "MPos"
#define GRBL_OK "ok"
#define GRBL_STATUS_START_STRING "<"
#define GRBL_STATUS_END_STRING ">"

//LINES
#define NEWLINE '\n'
#define RETURN '\r'
#define NULL_CHAR '\0'

/////////////////////////////////////////////////////////////////////////////////////

//BUFFERS
#define LINE_BUFFER_CHARACTERS 100

unsigned long startTime = 0.0;
unsigned long currentTime = 0.0;
unsigned long benchmarkTime = 0.0;
unsigned long startPollingTime = 0.0;
unsigned long startPollingTime2 = 0.0;


bool startUpOnce = true;
char bufferCharUSB = NULL_CHAR;
char bufferCharTEENSY = NULL_CHAR;
char USBLineArray[LINE_BUFFER_CHARACTERS];
char TEENSYLineArray[LINE_BUFFER_CHARACTERS];

byte bufferByteUSB = 0;
byte bufferByteTEENSY = 0;
int syncActionIndex = 0;
int bufferMacroLineIndex = 0;

float lastPositionX = 0.0;
float lastPositionY = 0.0;
float lastPositionZ = 0.0;

bool trackingPosition = false;
bool getStatus = true;

int previousTool = 0;

//COUNTERS
int USBLinePos = 0;
int TEENSYLinePos = 0;
//when we are executing the macros for the tool change, we need wait for an ok from GRBL before proceeding further
//this counter keeps the number of oks we need to wait before sending the next line
int waitForOk = 0;

//LEDs
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, RGB_PIN, NEO_GRB + NEO_KHZ800);


//RGB settings values
int colorR = 100;
int colorG = 100;
int colorB = 100;
int ledPower = 100;

#define MAX_DISTANCE_ARC 400
#define MAX_DISTANCE_SINGLE_ROTATION 360
#define MAX_ROTATION_SPEED 1000

int positionStepsArc = 0;
int positionStepsCircle = 0;

int stopsPerRotation = 1;
int stopsPerArc = 1;
int rotationSpeed = 1000;

bool isPaused = false;
bool isRunning = false;
bool isIdle = false;
bool forceStatus = false;

int outGoingCommand = -1;

void setup() {


  //INIT PINS FIRST
  //pinMode(RGB_PIN, OUTPUT);

  //init LEDS
  strip.begin();
  strip.show();   // make sure it is visible
  strip.clear(); 
  strip.setBrightness(ledPower);
  strip.show();

  for( int i = 0; i < LED_COUNT; i++ )
    strip.setPixelColor(i, colorR, colorG, colorB);

  strip.show();

    if (DEBUGGING_BASE) {
    USBSerialPrintln("Initializing serial ports");
  }

  //Serial.setFIFOSize(SERIAL_BUFFER_SIZE); not available for USBSerial looks like!!
  Serial.begin(USB_SERIAL_BAUD_RATE);
  while (!Serial) {}

  Serial1.setFIFOSize(SERIAL_BUFFER_SIZE);
  Serial1.begin(TEENSY_BAUD_RATE);
  while (!Serial1) {}

  if (DEBUGGING_BASE) { USBSerialPrintln("Serial ports initialized"); }

  resetUSBLine();
  resetTEENSYLine();
  resetBufferMacroLines();
  if (DEBUGGING_BASE) { USBSerialPrintln("buffer arrays cleared"); }

  //get the time from the finish of the init phase
  startTime = millis();

 if (DEBUGGING_DEEP) { USBSerialPrintln("Init finished"); }

}

////////////MAIN LOOP////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

  sendPing(100); //uncomment sendping here

  if(trackingPosition)
    askGRBLStatus(200);


  if(forceStatus){
    delay(200);
    askGRBLStatus();
    forceStatus = false;

  }
    

  if (BENCHMARKING)
    benchmarkTime = millis();

  if (startUpOnce) {

    currentTime = millis();

    if ((currentTime - startTime) < START_DELAY_TIME) {

      //STARTUP ONCE IF NEEDED GOES HERE

      startUpOnce = false;
    }
  }

  bufferCharUSB = readFromUSB();
  bufferByteUSB = bufferCharUSB;

  if (bufferCharUSB != NULL_CHAR) {

    if (DEBUGGING_BASE) { USBSerialPrint("from USB: "); }
    if (DEBUGGING_BASE) { USBSerialPrintln(bufferByteUSB); }



    //if (checkGRBLSpecialChar(bufferCharUSB) || checkGBRLSpecialByte(bufferByteUSB)) {
    if (false) {
      /*

    here if we need to do anything reacting to the special GRBL chars??

      */

    } else {

      USBLineArray[USBLinePos] = bufferCharUSB;
      ++USBLinePos;

      if (bufferCharUSB == NEWLINE) {

        handleUSBLine();
        resetUSBLine();
      }
    }
  }

  bufferCharTEENSY = readFromTeensy();
  bufferByteTEENSY = bufferCharTEENSY;

  if (bufferCharTEENSY != NULL_CHAR) {

    //if (DEBUGGING_BASE) { USBSerialPrint("from TEENSY: "); }
    //if (DEBUGGING_BASE) { USBSerialPrintln(bufferByteTEENSY); }


    //if (checkGRBLSpecialChar(bufferCharTEENSY) && checkGBRLSpecialByte(bufferByteTEENSY)) {
    if(false){
      /*

      if (DEBUGGING_BASE) { USBSerialPrintln("special byte from TEENSY"); }

      USBSerialWrite(bufferByteTEENSY);

      resetTEENSYLine();
    
      */


    } else {

      TEENSYLineArray[TEENSYLinePos] = bufferCharTEENSY;
      ++TEENSYLinePos;

      if (bufferCharTEENSY == NEWLINE) {

        handleTEENSYLine();
        resetTEENSYLine();
      }
    }
  }

  //here we deal with writing the macro lines that are present in the buffer, ready to be written to teensy
  bufferMacroLineIndex = firstMacroBufferLine();

  if (bufferMacroLineIndex > -1) {

    isRunning = true;
    trackingPosition = true;

    if (waitForOk <= 0) {

     outGoingCommand = checkOutgoingCommands(bufferMacroLines[bufferMacroLineIndex]);

      //here we check if the line is relative to an outgoing command or not
      if(outGoingCommand > -1){

         //if (DEBUGGING_DEEP) { USBSerialPrint("outGoingCommand:"); };
         //if (DEBUGGING_DEEP) { USBSerialPrintln(outGoingCommand); };

        //here we have to manage the outgoing commands if any is added to the macro
        //we also need to pause in case and wait for the resume from software
        if(outGoingCommand == 0){ //case take a picture

          //wait the machine is back in run after resume

          //wait the machine is in idle
          //if(isIdle && isPaused == false){
          if(isIdle){  
            //set pause
            isPaused = true;
            // USBSerialPrintln(PAUSE_OF_JOB);

            //send take picture
            USBSerialPrintln(TAKE_PICTURE);
            clearCharArray(bufferMacroLines[bufferMacroLineIndex], BUFFER_MACRO_CHARS);
            

          }


        }else
        if(outGoingCommand == 1){

          USBSerialPrintln(START_OF_JOB);
          clearCharArray(bufferMacroLines[bufferMacroLineIndex], BUFFER_MACRO_CHARS);

        }else
        if(outGoingCommand == 2){

          if(isIdle && !isPaused){
          
            USBSerialPrintln(END_OF_JOB);
            waitForOk = 0;
            isRunning = false;
            isPaused = false;
            trackingPosition = false;
            clearCharArray(bufferMacroLines[bufferMacroLineIndex], BUFFER_MACRO_CHARS);

          }

        }


      }else
      if(!isPaused){
        
        if (DEBUGGING_DEEP) { USBSerialPrint("Sending macro line: "); };
        if (DEBUGGING_DEEP) { USBSerialPrintln(bufferMacroLines[bufferMacroLineIndex]); };

        for (int C = 0; C < BUFFER_MACRO_CHARS; C++) {

        if (bufferMacroLines[bufferMacroLineIndex][C] == NULL_CHAR) {

          //USBSerialWrite(NEWLINE);
          TEENSYSerialWrite(NEWLINE);
          ++waitForOk;
          clearCharArray(bufferMacroLines[bufferMacroLineIndex], BUFFER_MACRO_CHARS);
          forceStatus = true;

          if (DEBUGGING_DEEP) { USBSerialPrintln("Macro line sent."); };

          break;
        }

        //USBSerialWrite(_array[C]);
        TEENSYSerialWrite(bufferMacroLines[bufferMacroLineIndex][C]);
      }
    }


  }

  } else{

    waitForOk = 0;
    isRunning = false;
    trackingPosition = false;
  
  }

  if (DEBUGGING_MACRO_BUFFER && waitForOk > 0) {
    USBSerialPrint("waitForOk:");
    USBSerialPrintln(waitForOk);
  }


  if (BENCHMARKING) {

    benchmarkTime = millis() - benchmarkTime;

    if (benchmarkTime > BENCHMARK_TRESHOLD_MS) {

      USBSerialPrint("loop ms: ");
      USBSerialPrintln(benchmarkTime);
    }
  }
}


/////////////////////////IMPLEMENTATION METHODS////////////////////////////////
char readFromUSB() {

  if (Serial.available())
    return Serial.read();
  else
    return NULL_CHAR;
}

char readFromTeensy() {

  if (Serial1.available())
    return Serial1.read();
  else
    return NULL_CHAR;
}


int checkOutgoingCommands(char macroLine[]){

  char* subString = NULL;

  for (int C = 0; C < OUTGOING_COMMAND_AMOUNT; C++) {

    subString = strstr(macroLine, OUTGOING_COMMANDS[C]);

  if (subString != NULL) {

      return C;

    }

  }

  return -1;

}

//Check inside the USBLineArray for pico commands
int checkPicoCommand() {

  char* subString = NULL;

  if (DEBUGGING_BASE) { USBSerialPrint("USBLineArray: "); }
  if (DEBUGGING_BASE) { USBSerialPrintln(USBLineArray); }

  for (int C = 0; C < PICO_COMMAND_AMOUNT; C++) {

    subString = strstr(USBLineArray, PICO_COMMANDS[C]);

    if (subString != NULL) {

      return C;
    }
  }

  return -1;
}

void writeLineTeensy() {

  if (DEBUGGING_BASE) { USBSerialPrint("usb line: "); }
  if (DEBUGGING_BASE) { USBSerialPrintln(USBLineArray); }

  //check for Mxxcommands

  char* subString = NULL;

  //subString = strstr(USBLineArray, M3);

  //if (subString != NULL)
    //digitalWrite(FAN_PIN, HIGH);

 



  for (int C = 0; C < LINE_BUFFER_CHARACTERS; C++) {

    TEENSYSerialWrite(USBLineArray[C]);

    if (USBLineArray[C] == NEWLINE)
      break;
  }
}

void writeLineUSB() {

  for (int C = 0; C < LINE_BUFFER_CHARACTERS; C++) {

    USBSerialWrite(TEENSYLineArray[C]);

    if (TEENSYLineArray[C] == NEWLINE)
      break;
  }
}

//check for extended ASCII bytes that are used by GRBL for real time commands
//note that those may not visible or writable characters
bool checkGBRLSpecialByte(byte _byte) {

  if ((_byte >= 128 && _byte <= 254) || _byte == GRBL_CTRLX)
    return true;

  return false;
}

//check for special GRBL chars used for real time operations
/*bool checkGRBLSpecialChar(char _char) {

  if (_char == GRBL_INTERROGATIVE)
    return true;

  if (_char == GRBL_TILDE)
    return true;

  if (_char == GRBL_ESCLAMATIVE)
    return true;

  return false;
}*/

void sendPing(int pollingInterval) {

  if (startPollingTime2 == 0) {

    startPollingTime2 = millis();
    return;
  }

  //here we also need to consider the waitForOk, sending a command only if we clear of waiting for ok, regardless the polling interval
  //WARNING, this maybe be better to be put in the loop? because sometimes this may not be asked because of waiting for ok!!
  if ((millis() - startPollingTime2) > pollingInterval) {
    startPollingTime2 = 0;

    USBSerialPrintln(PING);

  }
}





void askGRBLStatus() {

    TEENSYSerialWrite(GRBL_INTERROGATIVE);
    TEENSYSerialWrite(NEWLINE);
    ++waitForOk;
  
}



void askGRBLStatus(int pollingInterval) {

  if (startPollingTime == 0) {

    startPollingTime = millis();
    return;
  }

  //here we also need to consider the waitForOk, sending a command only if we clear of waiting for ok, regardless the polling interval
  //WARNING, this maybe be better to be put in the loop? because sometimes this may not be asked because of waiting for ok!!
  if ((millis() - startPollingTime) > pollingInterval && waitForOk <= 0) {
    startPollingTime = 0;
    TEENSYSerialWrite(GRBL_INTERROGATIVE);
    TEENSYSerialWrite(NEWLINE);
    ++waitForOk;
  }
}


void executeCycle(int toolNumber) {

  //int currentTool = handleReadSpindleTool(false);

  switch (toolNumber) {

    case 0:

      //returnTool(currentTool);
      break;

    case 1:
      //takeTool1(currentTool);

      break;

    default:
      //here we notifiy the server if the tool is out of range
      break;
  }
}


void takeTool1(int currentTool) {

/*

  char* subString = NULL;
  int syncActionIndex = -1;

  //if we already have the TX in spindle we do nothing
  if (currentTool == 1) {

    //here we notify the server the tool change is successfull becuase already in spindle
    return;
  }


  //first read what tool is in spindle, if T0 directly take the tool, otherwise bring back the current tool first
  //returnTool(currentTool);

  for (int C = 0; C < BUFFER_MACRO_LINES; C++) {

    if (MACRO_TAKE_T1[C][0] == NULL_CHAR)
      return;

    subString == NULL;

    //AIR_ON
    //subString = strstr(MACRO_TAKE_T1[C], AIR_ON);

    if (subString != NULL) {

      //add action to activate air, go to next cycle
      //if (DEBUGGING_DEEP) { USBSerialPrintln("AIR is on"); };

      //find the first free action slot/index
      syncActionIndex = firstEmptyAction();


      char actionType[SYNC_ACTION_CHARS] = {};
      //actionType = copyCharArray(AIR_ON);
      //WARNING here we need to match the position according to MACRO_TAKE_T1
      //strcpy(syncActions[syncActionIndex].type, AIR_ON);
      syncActions[syncActionIndex].positionX = -31.99;
      syncActions[syncActionIndex].positionY = -50.5;
      syncActions[syncActionIndex].positionZ = -87.5;

      syncActions[syncActionIndex].positionRange = 2;
      syncActions[syncActionIndex].pollingInterval = 100;
      continue;
    }

    subString == NULL;

    //AIR_OFF
    //subString = strstr(MACRO_TAKE_T1[C], AIR_OFF);

    if (subString != NULL) {

      //add action to de-activate air, go to next cycle
      //if (DEBUGGING_DEEP) { USBSerialPrintln("AIR is off"); };

      //find the first free action slot/index
      syncActionIndex = firstEmptyAction();



      //WARNING here we need to match the position according to MACRO_TAKE_T1
      //strcpy(syncActions[syncActionIndex].type, AIR_OFF);
      syncActions[syncActionIndex].positionX = -31.99;
      syncActions[syncActionIndex].positionY = -53.5;
      syncActions[syncActionIndex].positionZ = -102.5;

      syncActions[syncActionIndex].positionRange = 1;
      syncActions[syncActionIndex].pollingInterval = 100;
      continue;
    }

    subString == NULL;

    //SET_TOOL
    subString = strstr(MACRO_TAKE_T1[C], tools[1].toolSetAction);

    if (subString != NULL) {

      //find the first free action slot/index
      syncActionIndex = firstEmptyAction();


      //WARNING here we need to match the position according to MACRO_TAKE_T1
      strcpy(syncActions[syncActionIndex].type, tools[1].toolSetAction);
      syncActions[syncActionIndex].positionX = -31.99;
      syncActions[syncActionIndex].positionY = -75.5;
      syncActions[syncActionIndex].positionZ = -2;

      syncActions[syncActionIndex].positionRange = 1;
      syncActions[syncActionIndex].pollingInterval = 100;
      continue;
    }

    //if no special commands are found then we sent the injected macro to TEENSY
    writeMACROLine(MACRO_TAKE_T1[C]);
  }
*/

}

////////////////////////////////////////////////////////HANDLE METHODS////////////////////////////////////////////////////////////
void handleUSBLine() {

  if (DEBUGGING_BASE) { USBSerialPrintln("line read from USB"); }

  int commandIndex = -1;

  commandIndex = checkPicoCommand();

  if (DEBUGGING_BASE) { USBSerialPrint("commandIndex: "); }
  if (DEBUGGING_BASE) { USBSerialPrintln(commandIndex); }

  //check is there is GCODE command of the ones that the PICO has to use, otherwise send it to TEENSY
  if (commandIndex != -1) {

    if (DEBUGGING_DEEP) { USBSerialPrint("PICO command detected:"); }
    //if (DEBUGGING_DEEP) { USBSerialPrintln(PICO_COMMANDS[commandIndex]); }
    if (DEBUGGING_DEEP) { USBSerialPrintln(commandIndex); }

    switch (commandIndex) {

      case 0:  //STOPS_ROTATION,  //case 0 STOPS_ROTATION
        handleStopsRotation(); 
        if (DEBUGGING_DEEP) { USBSerialPrintln("STOPS_ROTATION"); }
        break;

      case 1:  //ROTATION_SPEED,  //case 1 ROTATION_SPEED
        handleSetRotationSpeed();
        if (DEBUGGING_DEEP) { USBSerialPrintln("ROTATION_SPEED"); }
        break;

      case 2:  //RGB_SETTINGS,    //case 2 RGB_SETTINGS
        handleRGBSettings();
        if (DEBUGGING_DEEP) { USBSerialPrintln("RGB_SETTINGS"); }
        break;

      case 3:  //HOMING,          //case 3 HOMING
        handleHoming();
        if (DEBUGGING_DEEP) { USBSerialPrintln("HOMING"); }
        break;

      case 4:  //PICTURES_ARC,    //case 4 PICTURES_ARC
        handleStopsPerArc();
        if (DEBUGGING_DEEP) { USBSerialPrintln("PICTURES_ARC"); }
        break;

      case 5:   //START,           //case 5 START
        handleStart();
        if (DEBUGGING_DEEP) { USBSerialPrintln("START"); }
        break;

      case 6:  //STOP,            //case 6 STOP
        handleStop();
        if (DEBUGGING_DEEP) { USBSerialPrintln("STOP"); }
        break;
      
      case 7:  //PAUSE,           //case 7 PAUSE
      handlePause();
        if (DEBUGGING_DEEP) { USBSerialPrintln("PAUSE"); }
        break;


      case 8:  //RESUME,           //case 8 RESUME
      handleResume();
        if (DEBUGGING_DEEP) { USBSerialPrintln("RESUME"); }
        break;

      case 9:  //PRINT_SETTING,           //case 8 PRINT_SETTING
      printSettings();
        if (DEBUGGING_DEEP) { USBSerialPrintln("PRINT_SETTING"); }
        break;

      case 10:  //RESUME PICTURE,           //case 8 RESUME PICTURE,
      handleResumePicture();
        if (DEBUGGING_DEEP) { USBSerialPrintln("RESUME_PICTURE,"); }
        break;

      default:
        break;
    }

  } else {

    writeLineTeensy();
  }
}


void printSettings(){

  USBSerialPrint("RGB Red: ");
  USBSerialPrintln(colorR);
  USBSerialPrint("RGB Green: ");
  USBSerialPrintln(colorG);
  USBSerialPrint("RGB Blue: ");
  USBSerialPrintln(colorB);
  USBSerialPrint("RGB Power: ");
  USBSerialPrintln(ledPower);

  USBSerialPrint("Steps per rotation: ");
  USBSerialPrintln(stopsPerRotation);

  USBSerialPrint("Steps per arc: ");
  USBSerialPrintln(stopsPerArc);

  USBSerialPrint("Speed: ");
  USBSerialPrintln(rotationSpeed);

  USBSerialPrint("Is paused: ");
  USBSerialPrintln(isPaused);
  
  USBSerialPrint("Is running: ");
  USBSerialPrintln(isRunning);

}

void handleResumePicture(){

  USBSerialPrintln(JOB_RESUMED);
  isPaused = false;

};


void handleResume(){

  TEENSYSerialWrite(GRBL_TILDE);
  TEENSYSerialWrite(NEWLINE);
  ++waitForOk;
  USBSerialPrintln(JOB_RESUMED);
  isPaused = false;

};

void handleHoming(){

  writeMACROLine(START_OF_JOB);
  writeMACROLine("$H");
  writeMACROLine(END_OF_JOB);

};

void handlePause(){

  TEENSYSerialWrite(GRBL_TILDE);
  TEENSYSerialWrite(NEWLINE);
  ++waitForOk;
  //set pause
  isPaused = true;
  USBSerialPrintln(PAUSE_OF_JOB);

}

void handleStop(){

  TEENSYSerialWrite(GRBL_CTRLX);
  TEENSYSerialWrite(NEWLINE);
  delay(100);
  TEENSYSerialPrintln("$X");
  resetBufferMacroLines();
  waitForOk = 0;

  if(isRunning)
    USBSerialPrintln(END_OF_JOB);

  isRunning = false;
  isPaused = false;
  trackingPosition = false;

}

void handleStart(){

  int circleStepsmm = 0;
  int arcStepsPermm = 0;
  int currentPosition = 0;

  char G1XString[10] = "G1 X";
  char G1YString[10] = "G1 Y";
  char speedString[10];
  char finalSpeedString[10];
  char fString[10] = " F";
  char xPosString[10];
  char yPosString[10];
  char xPositionString[10];

  //cumulative string for the motion command
  char moveString[50];

  //clear buffer arrays
  clearCharArray(speedString,10);
  clearCharArray(finalSpeedString,10);
  clearCharArray(xPosString,10);
  clearCharArray(yPosString,10);
  clearCharArray(xPositionString,10);

  clearCharArray(moveString,50);

  //here we create the required macro
  arcStepsPermm = MAX_DISTANCE_ARC / stopsPerArc;

  //calculate the steps in mm for the circle
  circleStepsmm = MAX_DISTANCE_SINGLE_ROTATION / stopsPerRotation;

  //convert the speed to string
  itoa(rotationSpeed,speedString,10);

  strcat(finalSpeedString,fString);
  strcat(finalSpeedString,speedString);

  //convert X position to string, we will use always the same by resetting to zero
  itoa(circleStepsmm,xPosString,10);

  //here we start the job
  writeMACROLine(START_OF_JOB);

  //here perform homing
  writeMACROLine("$H"); //uncomment homing here
  writeMACROLine("G92 Y0");

  //above is same as the old start-------------
  for(int I=0;I<stopsPerArc;I++){

      //move one step up
      currentPosition -= arcStepsPermm;

      if(currentPosition <= MAX_DISTANCE_ARC){

          clearCharArray(yPosString,10);
          itoa(currentPosition,yPosString,10);

          strcat(moveString, G1YString);
          strcat(moveString, yPosString);
          strcat(moveString, finalSpeedString);

          writeMACROLine(moveString);
          clearCharArray(moveString,50);

      }

        for(int C=0; C < stopsPerRotation; C++){

          writeMACROLine("G92 X0");
          
          //add move X string to the scanner
          strcat(moveString, G1XString);
          strcat(moveString, xPosString);
          strcat(moveString, finalSpeedString);

          writeMACROLine(moveString);
          clearCharArray(moveString,50);

          //here take picture
          writeMACROLine(TAKE_PICTURE);

      }

  }

  writeMACROLine(END_OF_JOB);

}

void handleStart1(){

  int circleStepsmm = 0;
  int arcStepsPermm = 0;

  char G1XString[10] = "G1 X";
  char G1YString[10] = "G1 Y";
  char speedString[10];
  char finalSpeedString[10];
  char fString[10] = " F";
  char xPosString[10];
  char yPosString[10];
  char xPositionString[10];

  //cumulative string for the motion command
  char moveString[50];

  //clear buffer arrays
  clearCharArray(speedString,10);
  clearCharArray(finalSpeedString,10);
  clearCharArray(xPosString,10);
  clearCharArray(yPosString,10);
  clearCharArray(xPositionString,10);

  clearCharArray(moveString,50);

  //here we create the required macro
  arcStepsPermm = MAX_DISTANCE_SINGLE_ROTATION / stopsPerArc;

  //calculate the steps in mm for the circle
  circleStepsmm = MAX_DISTANCE_ARC / stopsPerRotation;

  //convert the speed to string
  itoa(rotationSpeed,speedString,10);

  strcat(finalSpeedString,fString);
  strcat(finalSpeedString,speedString);

  //convert X position to string, we will use always the same by resetting to zero
  itoa(circleStepsmm,xPosString,10);

  //convert Y position to string, we will use always the same by resetting to zero
  itoa(arcStepsPermm,yPosString,10);

  //here we start the job
  writeMACROLine(START_OF_JOB);
  
  for(int C=0; C < stopsPerRotation; C++){
      
      //here reset both X to zero
      writeMACROLine("G92 X0");

      //add move X string to the scanner
      strcat(moveString, G1XString);
      strcat(moveString, xPosString);
      strcat(moveString, finalSpeedString);

      writeMACROLine(moveString);
      clearCharArray(moveString,50);

        //here perform homing
        //writeMACROLine("$H");

      //here move to the rotation step first    
      for(int I=0;I<stopsPerArc;I++){
        
        //here reset X to zero
        writeMACROLine("G92 Y0");

        strcat(moveString, G1YString);
        strcat(moveString, yPosString);
        strcat(moveString, finalSpeedString);

        writeMACROLine(moveString);
        clearCharArray(moveString,50);     

        //for each of these we need to send a command to take the picture to the software
        writeMACROLine(TAKE_PICTURE);

      }


  }

  writeMACROLine(END_OF_JOB);



}

void handleTEENSYLine() {

  //if (DEBUGGING_BASE) { USBSerialPrintln("line read from TEENSY"); }

  //here is important to distinguish between a pass trough command and when we are during a tool change
  //during tool change trackingPosition is active and also we maybe waiting for ok from GRBL
  //in both situation, during the tool change, we avoid data going to the server
  //we need to remember that an ok must be sent to the server after each successful tool change action set tool
  char* subString = NULL;

  //check for MPos and update positions in case is there and trackPosition is true
  if (trackingPosition) {

    //check if MPOS is in the line to detect report lines
    subString = strstr(TEENSYLineArray,"<");
 
    if (subString != NULL) {
      
        subString = strstr(TEENSYLineArray,"Idle");

        if (subString != NULL){
          
          isIdle = true;

        }else{

          isIdle = false;

        }

       //writeLineUSB(); //uncomment
        return;

      }
    }

    /*
    //also here consider the case of other reporting and avoid to send it to the server
    subString = strstr(TEENSYLineArray, GRBL_STATUS_START_STRING);

    if (subString != NULL)
      return;

    subString = strstr(TEENSYLineArray, GRBL_STATUS_END_STRING);

    if (subString != NULL)
      return;

    */

    //here return also if there is an ok
    subString = strstr(TEENSYLineArray, GRBL_OK);

    if (subString != NULL) {

      if (waitForOk > 0)
        --waitForOk;
      return;

    }
  

  //in case we are waiting for "ok" from grbl here we check if we have received one
  //additionally from when tracking position is false, just to clear the oks waiting
  if (waitForOk > 0) {

    //check if ok is in the line
    char* subString = strstr(TEENSYLineArray, GRBL_OK);

    if (subString != NULL) {

      --waitForOk;
      return;
    }
  }

  //if not during a tool changer or for other lines, we send them to the server
  writeLineUSB();
}

void handleStopsPerArc(){

  char* subString = NULL;

  subString = strchr(USBLineArray, EQUAL_CHAR);

  if(subString!=NULL){

      ++subString;
     stopsPerArc = atoi(subString);

    if (DEBUGGING_DEEP) { USBSerialPrintln(stopsPerArc); }

  }else
  if (DEBUGGING_DEEP) { USBSerialPrintln("Error in parsing stops per rotation command"); }

}

void handleStopsRotation(){

  char* subString = NULL;

  subString = strchr(USBLineArray, EQUAL_CHAR);

  if(subString!=NULL){

    ++subString;
    stopsPerRotation = atoi(subString);

    if (DEBUGGING_DEEP) { USBSerialPrintln(stopsPerRotation); }

  }else
  if (DEBUGGING_DEEP) { USBSerialPrintln("Error in parsing stops per rotation command"); }

}

void handleSetRotationSpeed(){

  //rotationSpeed;
  char* subString = NULL;
  int tempSpeed = 0;

  subString = strchr(USBLineArray, EQUAL_CHAR);

  if(subString!=NULL){

    ++subString;

    tempSpeed = atoi(subString);

    if(tempSpeed > 0 && tempSpeed < MAX_ROTATION_SPEED){

      rotationSpeed = tempSpeed;

    }
      
  }else
  if (DEBUGGING_DEEP) { USBSerialPrintln("Error in reading the speed"); }


}

void handleRGBSettings(){

    char* subString = NULL;
    char *setting;
    char *saveptr;

    subString = strchr(USBLineArray, EQUAL_CHAR);

    if(subString!=NULL){

      ++subString;

      setting = strtok_r(subString,",",&saveptr);
      if(setting != NULL)
        colorR = atoi(setting);

      setting = strtok_r(NULL,",",&saveptr);
      if(setting != NULL)
        colorG = atoi(setting);

      setting = strtok_r(NULL,",",&saveptr);
      if(setting != NULL)
        colorB = atoi(setting);

      setting = strtok_r(NULL,",",&saveptr);
      if(setting != NULL)
        ledPower = atoi(setting);

      for (int i = 0; i < LED_COUNT; i++)
        strip.setPixelColor(i, colorR, colorG, colorB);

        strip.setBrightness(ledPower);
        strip.show();
      
      strip.setBrightness(ledPower);

      if (DEBUGGING_DEEP) { USBSerialPrintln(colorR); }
      if (DEBUGGING_DEEP) { USBSerialPrintln(colorG); }
      if (DEBUGGING_DEEP) { USBSerialPrintln(colorB); }
      if (DEBUGGING_DEEP) { USBSerialPrintln(ledPower); }

    }else
      if (DEBUGGING_DEEP) { USBSerialPrintln("Error in parsing RGB settings command"); }

}



void replaceReturnAndNewlines(char* line, char replace) {

  char* subString = strchr(line, NEWLINE);

  while (subString != NULL) {

    subString[0] = replace;
    subString = strchr(line, NEWLINE);
  }

  subString = strchr(line, RETURN);

  while (subString != NULL) {

    subString[0] = replace;
    subString = strchr(line, RETURN);
  }
}



char* copyCharArray(const char* _array) {

  char* ret = new char[strlen(_array) + 1];
  strcpy(ret, _array);
  return ret;
}

void resetLastPostions() {

  lastPositionX = 0.0;
  lastPositionY = 0.0;
  lastPositionZ = 0.0;
}

int firstSyncAction() {

  for (int C = 0; C < SYNC_ACTION_AMOUNT; C++)
    if (syncActions[C].type[0] != NULL_CHAR)
      return C;

  return -1;
}

int lastSyncAction() {

  for (int C = SYNC_ACTION_AMOUNT - 1; C > -1; C--)
    if (syncActions[C].type[0] != NULL_CHAR)
      return C;

  return -1;
}

int firstMacroBufferLine() {

  for (int C = 0; C < BUFFER_MACRO_LINES; C++)
    if (bufferMacroLines[C][0] != NULL_CHAR)
      return C;

  return -1;
}



int firstEmptyAction() {

  for (int C = 0; C < SYNC_ACTION_AMOUNT; C++)
    if (syncActions[C].type[0] == NULL_CHAR)
      return C;

  return -1;
}


void writeMACROLine(const char _array[]) {


  if (DEBUGGING_DEEP) { USBSerialPrint("Macro Line: "); }
  if (DEBUGGING_DEEP) { USBSerialPrintln(_array); }
  //check first empty element in the bufferMacroLines
  //add the current macro line to that
  //the loop will take care of writing those to teensy waiting for the ok
  for (int C = 0; C < BUFFER_MACRO_LINES; C++) {

    if (bufferMacroLines[C][0] == NULL_CHAR) {
      strcpy(bufferMacroLines[C], _array);
      return;
    }

    //if there is no space in the buffer we skip the macro line
    //we absolutely avoid that because other wise some commands for the tool changer will not be executed
  }

}

int findCharPos(char _array[], int length, char toFind) {

  for (int C = 0; C < length; C++) {

    if (_array[C] == toFind || _array[C] != NULL_CHAR)
      return C;
  }

  return -1;
}

void resetTEENSYLine() {

  clearCharArray(TEENSYLineArray, LINE_BUFFER_CHARACTERS);
  TEENSYLinePos = 0;
  bufferCharTEENSY = NULL_CHAR;
}

void resetUSBLine() {

  clearCharArray(USBLineArray, LINE_BUFFER_CHARACTERS);
  USBLinePos = 0;
  bufferCharUSB = NULL_CHAR;
}

void resetBufferMacroLines() {

  for (int C = 0; C < BUFFER_MACRO_LINES; C++)
    clearCharArray(bufferMacroLines[C], BUFFER_MACRO_CHARS);
}

void clearCharArray(char _array[], int pos) {

  for (int C = 0; C < pos; C++) {

    if (_array[C] == NULL_CHAR)
      break;

    _array[C] = NULL_CHAR;
  }
}

void clearCharArray2D(char _array[BUFFER_MACRO_LINES][BUFFER_MACRO_CHARS], int pos1, int pos2) {

  for (int C = 0; C < pos1; C++) 
    for(int I=0;I<pos2;I++){


    if (_array[C][I] == NULL_CHAR)
      break;

    _array[C][I] = NULL_CHAR;
  }
}

void TEENSYSerialWrite(byte _byte) {

  Serial1.write(_byte);
  Serial1.flush();
}

void TEENSYSerialWrite(char _char) {

  Serial1.write(_char);
  Serial1.flush();
}

void TEENSYSerialPrintln(char* _char) {

  Serial1.print(_char);
  Serial1.println();
  Serial1.flush();
}

void TEENSYSerialPrint(char* _char) {

  Serial1.print(_char);
  Serial1.flush();
}


void TEENSYSerialPrintln(int _int) {

  Serial1.print(_int);
  Serial1.println();
  Serial1.flush();
}

void TEENSYSerialPrint(int _int) {

  Serial1.print(_int);
  Serial1.flush();
}


void USBSerialPrintln() {

  Serial.println();
  Serial.flush();
}

void USBSerialPrintln(char* _string) {

  Serial.print(_string);
  Serial.println();
  Serial.flush();
}

void USBSerialPrint(char* _string) {

  Serial.print(_string);
  Serial.flush();
}

void USBSerialPrintln(const char* _string) {

  Serial.print(_string);
  Serial.println();
  Serial.flush();
}

void USBSerialPrintln(int _int) {

  Serial.print(_int);
  Serial.println();
  Serial.flush();
}

void USBSerialPrint(double _double) {

  Serial.print(_double, 3);
  Serial.flush();
}

void USBSerialPrintln(double _double) {

  Serial.print(_double, 3);
  Serial.println();
  Serial.flush();
}

void USBSerialPrint(unsigned long _long) {

  Serial.print(_long);
  Serial.flush();
}

void USBSerialPrintln(unsigned long _long) {

  Serial.print(_long);
  Serial.println();
  Serial.flush();
}

void USBSerialWrite(byte _byte) {

  Serial.write(_byte);
  Serial.flush();
}

void USBSerialWrite(char _char) {

  Serial.write(_char);
  Serial.flush();
}
