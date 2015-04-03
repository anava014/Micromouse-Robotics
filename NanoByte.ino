#include <Servo.h>

#define bool unsigned char
#define VELOCITY .0134
#define TRAVELING_CONSTANT .01
#define WALL_APPROACHING 550
#define LEFTWALLMISSING 470
#define RIGHTWALLMISSING 460
#define TURN_DELAY 522
#define LEFT_TIMER 160 //Adjust for Left Turning Timer

int leftLed = 13;
int middleLed = 12;
int rightLed = 11;

bool disabled = 1; //Disable sensors at start for 1000 ms
int disabledTimer = 0;

const int middleSensor = A2;
const int leftSensor = A1;
const int rightSensor = A0;

int function = 0;
int totalError = 0;

int errorL = 0;
int prevErrorL = 0;
int noLeftWallApproaching = 0;
int preparingToTurnLeft = 0;
int leftTimer = 0;
int leftWallApproaching = 0;

int errorR = 0;
int prevErrorR = 0;
int noRightWallApproaching = 0;
int preparingToTurnRight = 0;
int rightTimer = 0;
int rightWallApproaching = 0;

int errorM = 0;
int prevErrorM = 0;
int frontWallApproaching = 0;
int waitingForStop = 0;
int waitForStop = 0;

const double Kp = 2;
unsigned int counter = 0;

unsigned int skewL = 0;
unsigned int skewR = 0;
unsigned int skewM = 0;
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
  gridInit();
  
  Serial1.begin(9600);
  myServoL.attach(9);  // Servo is connected to digital pin 9 
  myServoR.attach(10);  // Servo is connected to digital pin 10 
  stopServo();
  calibrate();
  time = millis();
  forward();
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
}

void readFrontSensorForWall(){
  errorM = readErrorM();
  if(errorM >= WALL_APPROACHING) //Delays Left Turning Timer Because A wall is approaching // BETA
  {
    Serial1.println("VOID"); 
    leftTimer = 1;
  }
  if(!waitForStop){             // if its not already going to continue straight AND its NOT disabled. (meaning disabled = 0)
     Serial1.println("IN IF"); 
    if(errorM > prevErrorM && errorM >= WALL_APPROACHING){
      ++frontWallApproaching;
    }
    else 
      frontWallApproaching = 0;
    
    //Confirming there is a Wall Approaching, now checking Left and Right for Empty Walls
    if(frontWallApproaching >= 8) {
      waitForStop = 1;
      frontWallApproaching = 0;
    }
    prevErrorM = errorM;
  }
}

void readLeftSensorForWall(){  //Overall, Checks if Left sensor detects a wall or not and raises or drops flag
  errorL = readErrorL();
  if(!preparingToTurnLeft && !disabled){            // if its not already going to turn left AND its NOT disabled. (meaning disabled = 0)
    if(errorL <= LEFTWALLMISSING) {                //There was a sudden Drop. Increment counter for Left Flag
      ++noLeftWallApproaching;
    }
    else {
      noLeftWallApproaching = 0;      // Reset Counter
    }
   if(noLeftWallApproaching >= 5) {   //Confirmed there is a Missing Left Wall Approaching
     preparingToTurnLeft = 1;         //Raise Left Flag
     noLeftWallApproaching = 0;       //Reset Counter
    }
  }
}

void readRightSensorForWall(){ //Overall, Checks if Right sensor detects a wall or not and raises or drops flag
  errorR = readErrorR();
  if(!preparingToTurnRight && !disabled){         // if its not already going to turn right AND its NOT disabled. (meaning disabled = 0)
    if(errorR <= RIGHTWALLMISSING)                //There was a sudden Drop. Increment counter for Left Flag
      ++noRightWallApproaching;
    else
      noRightWallApproaching = 0;      // Reset Counter
      
    if(noRightWallApproaching >= 5){    //Confirmed there is a Missing Left Wall Approaching
      preparingToTurnRight = 1;         //Raise Left Flag
      noRightWallApproaching = 0;       //Reset Counter
    }
    
  }
  else{ // It is Preparing to turn Right, So we will check it has passed the wall to end it
    if(!waitForStop && errorR > RIGHTWALLMISSING)   // theres no wall in front and there is a wall on the right
      ++rightWallApproaching; 
    else
        rightWallApproaching = 0;
    if(rightWallApproaching >= 5)     // confirmation step - confirming there is a wall on the right side
    {
      preparingToTurnRight = 0;    // turn flag off
      rightWallApproaching = 0;    // reseting the counter
    }
  }
}

void logic(){
  if(waitForStop){
    //Focus on Front wall and Look out for flags whether to turn left or right
    if(errorM >= 1300) {//Right in front of fall
//      if(preparingToTurnLeft && preparingToTurnRight) {
//        //Both No Walls (Turn Left)
//        advanceLeft();
//        preparingToTurnLeft = 0; //Turn this into function for CLear Left. For timer and everything
//        preparingToTurnRight = 0;
//      }
      if(preparingToTurnLeft)
        advanceLeft();
      else if(preparingToTurnRight)
        advanceRight();
      else
        advance180Deg();

      waitForStop = 0;           //Turn this into function for Clear everything
      preparingToTurnLeft = 0;
      preparingToTurnRight = 0;
      leftTimer = 1;
      disableLeftRight();
    }
  }
  else{ //Start a timer if left or right flags rise. Consider cancelling if a wall suddenly approaches
//    if(preparingToTurnLeft && preparingToTurnRight) {
//      //Both No Walls
//    }
    if(preparingToTurnLeft) {
      prepareForLeftTurn();
    }
//    else if(preparingToTurnRight) {
//      //PID Control Towards Left
//    }
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

void loop()
{
  collectData();            // PID for center control
  readFrontSensorForWall(); //Searching for Wall Approaching
  readLeftSensorForWall(); // Searching for blank left Wall, will flag If True
  readRightSensorForWall(); // Searching for blank right wall, will flag If True
  logic();
  lightShow(); 
}
