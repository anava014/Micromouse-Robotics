#include <Servo.h>

#define bool unsigned char
#define VELOCITY .01755  //.0169
#define TRAVELING_CONSTANT .015
#define WALL_APPROACHING 145
#define LEFTWALLMISSING 350
#define RIGHTWALLMISSING 200
#define TURN_DELAY 394
#define LEFT_TIMER 800
#define LEFT_TIMER_WITH_WALL 650
#define RIGHT_TIMER 800
#define RIGHT_TIMER_WITH_WALL 650
#define IN_FRONT_OF_WALL 900

int currentMillis = 0;

int BUTTON_PIN = 18;
int gridCounter = 0;

int turnsInOneSecond = 0;
int leftTurnClock = 0;

int leftLed = 13;
int middleLed = 12;
int rightLed = 11;

int cellLED = 3;
int speakerTimer = 0;
int speakerIsOn = 0;

bool disabled = 1; //Disable sensors at start for 1000 ms
int disabledTimer = 0;

const int middleSensor = A2;
const int leftSensor = A1;
const int rightSensor = A0;

int incoming = 0;

int function = 0;
double totalError = 0;

double errorL = 0;
double prevErrorL = 0;
int noLeftWallApproaching = 0;
int preparingToTurnLeft = 0;
int leftWallApproaching = 0;
int leftClock = 0;

double errorR = 0;
double prevErrorR = 0;
int noRightWallApproaching = 0;
int preparingToTurnRight = 0;
int rightTimer = 0;
int rightWallApproaching = 0;
int rightClock = 0;

long int errorM = 0;
long int prevErrorM = 0;
int frontWallApproaching = 0;
int waitingForStop = 0;
int waitForStop = 0;

const double Kp = .4;
unsigned int counter = 0;

double skewL = 0;
double skewR = 0;
double skewM = 0;
const int maxSpeedL = 1700;
const int maxSpeedR = 1300;
unsigned char debugMode = 0;


Servo myServoL;
Servo myServoR;

int time = 0;
bool isTraveling = 1;
int timeToTravel = 0;

void setup(void)
{
  delay(1000);
  pinMode(leftLed, OUTPUT);
  pinMode(middleLed, OUTPUT);
  pinMode(rightLed, OUTPUT);
  pinMode(cellLED, OUTPUT);
  gridInit();
  
  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, HIGH); // pull-up
  
  Serial.begin(9600);
  myServoL.attach(9);  // Servo is connected to digital pin 9 
  myServoR.attach(10);  // Servo is connected to digital pin 10 
  stopServo();
  digitalWrite(middleLed, HIGH);
  waitForOK();
  digitalWrite(middleLed, LOW);
  calibrate();
  time = millis();
  //forward();
}

int handle_button()
{
  int button_pressed = !digitalRead(BUTTON_PIN); // pin low -> pressed
  return button_pressed;
}

void waitForOK()
{
  while(!handle_button()) {}
}

void pid(int totalError, unsigned char function)
{
  if(function == 0) // If it's closer to the left side
  {
    if(totalError * Kp < 200)
      myServoR.writeMicroseconds(maxSpeedR + (totalError * Kp));  // Counter clockwise
    else // To prevent it from going backwards
      myServoR.writeMicroseconds(maxSpeedR + 180);  // Counter clockwise
    myServoL.writeMicroseconds(maxSpeedL);
  }
  else if(function == 1) // If it's closer to the right side
  {
    if(totalError * Kp < 200)
      myServoL.writeMicroseconds(maxSpeedL - (totalError * Kp));  // Counter clockwise
    else // To Prevent it from going backwards
      myServoL.writeMicroseconds(maxSpeedL - 180);
    myServoR.writeMicroseconds(maxSpeedR);
  }
  else if(function == 2) // No Wall on Left Side
  {
    if(totalError > 0){ //Too close to Left Side
        myServoR.writeMicroseconds(maxSpeedR + (totalError * Kp));
        myServoL.writeMicroseconds(maxSpeedL);
    }
    else{ //Too close to Right Side
      myServoL.writeMicroseconds(maxSpeedL + (totalError * Kp * 3));  // Counter clockwise //FIX ME NEEDS BOUNDARIES SO IT WONT GO BACKWARDS
      myServoR.writeMicroseconds(maxSpeedR);
    }
 }
 else if(function == 3) // No Wall on Right Side
  {
    if(totalError > 0){ //Too close to Right Side
        myServoR.writeMicroseconds(maxSpeedR);
        myServoL.writeMicroseconds(maxSpeedL - (totalError * Kp * 3));
    }
    else{ //Too close to Left Side
      myServoL.writeMicroseconds(maxSpeedL);  // Counter clockwise //FIX ME NEEDS BOUNDARIES SO IT WONT GO BACKWARDS
      myServoR.writeMicroseconds(maxSpeedR - (totalError * Kp));
    }
 }
  else // Its Kind of in the middle
  {
    myServoR.writeMicroseconds(maxSpeedR);
    myServoL.writeMicroseconds(maxSpeedL);
  }
}

void lightShow() {
  digitalWrite(middleLed, (waitForStop) ? (HIGH) : (LOW));
  digitalWrite(leftLed, (preparingToTurnLeft) ? (HIGH) : (LOW));
  digitalWrite(rightLed, (preparingToTurnRight) ? (HIGH) : (LOW));
  turnOffCellLed();
}

void readFrontSensorForWall(){
  errorM = readErrorM();
  if(!waitForStop){                                 // if its not already going to continue straight AND its NOT disabled. (meaning disabled = 0)
    if(errorM >= WALL_APPROACHING)
      ++frontWallApproaching;
    else 
      frontWallApproaching = 0;
    if(frontWallApproaching >= 5) {
      waitForStop = 1;
      frontWallApproaching = 0;
    }
  }
}

void readLeftSensorForWall(){  //Overall, Checks if Left sensor detects a wall or not and raises or drops flag
  errorL = readErrorL();
  if(!preparingToTurnLeft && !disabled){            // if its not already going to turn left AND its NOT disabled. (meaning disabled = 0)
    if(errorL <= LEFTWALLMISSING) {                 //There was a sudden Drop. Increment counter for Left Flag
      ++noLeftWallApproaching;
    }
    else {
      noLeftWallApproaching = 0;                     // Reset Counter
    }
   if(noLeftWallApproaching >= 5) {                  //Confirmed there is a Missing Left Wall Approaching
     preparingToTurnLeft = 1;                        //Raise Left Flag
     leftClock = millis();    
     noLeftWallApproaching = 0;                      //Reset Counter
    }
  }
}

void readRightSensorForWall(){                       //Overall, Checks if Right sensor detects a wall or not and raises or drops flag
  errorR = readErrorR();
  if(!preparingToTurnRight && !disabled){            // if its not already going to turn right AND its NOT disabled. (meaning disabled = 0)
    if(errorR <= RIGHTWALLMISSING)                   //There was a sudden Drop. Increment counter for Left Flag
      ++noRightWallApproaching;
    else
      noRightWallApproaching = 0;                    // Reset Counter
      
    if(noRightWallApproaching >= 5){                 //Confirmed there is a Missing Right Wall Approaching
      preparingToTurnRight = 1;                      //Raise Left Flag
      rightClock = millis();
      noRightWallApproaching = 0;                    //Reset Counter
    }
    
  }
  else{                                              // It is Preparing to turn Right, So we will check it has passed the wall to end it
    if(!waitForStop && errorR > RIGHTWALLMISSING)    // theres no wall in front and there is a wall on the right
      ++rightWallApproaching; 
    else
        rightWallApproaching = 0;
    if(rightWallApproaching >= 5)                    // confirmation step - confirming there is a wall on the right side
    {
      preparingToTurnRight = 0;                      // turn flag off
      rightWallApproaching = 0;                      // reseting the counter
    }
  }
}

void logic(){
  if(waitForStop || errorM >= IN_FRONT_OF_WALL){
    if(errorM >= IN_FRONT_OF_WALL) {
      if(preparingToTurnLeft)
        advanceLeft();
      else if(preparingToTurnRight)
        advanceRight();
      else
        advance180Deg();

      waitForStop = 0; 
      preparingToTurnLeft = 0;
      preparingToTurnRight = 0;
      disableLeftRight();
      prevErrorM = errorM;
    }
  }
  else{ 
    if(preparingToTurnLeft)
      prepareForLeftTurnWithoutWall();
    else if(preparingToTurnRight)
      prepareForRightTurnWithoutWall();
  }
  enableLeftRight();
}

void disableLeftRight(){
  disabled = 1;
  disabledTimer = millis();
}
void enableLeftRight(){
  if(millis() > disabledTimer + 400)
    disabled = 0;
}

void turnOffCellLed(){
  if(millis() > speakerTimer + 100)
    digitalWrite(cellLED, LOW);
}

void turnCounter(){
  currentMillis = millis();
  if(turnsInOneSecond >= 2){// && currentMillis < leftTurnClock + 500){
    time -= 130;
    turnsInOneSecond = 0;
  }
//  else if(currentMillis > leftTurnClock + 500)
//    turnsInOneSecond = 0;
}

void loop()
{
  collectData();            // PID for center control
  readFrontSensorForWall(); //Searching for Wall Approaching
  readLeftSensorForWall(); // Searching for blank left Wall, will flag If True
  readRightSensorForWall(); // Searching for blank right wall, will flag If True
  logic();
  
  turnCounter();
  traveledOneCell(18);
  lightShow();
}
