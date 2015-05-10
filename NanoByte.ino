#include <Servo.h>

#define bool unsigned char
#define VELOCITY .01755  //.0169
#define TRAVELING_CONSTANT .015
#define WALL_APPROACHING 78
#define LEFTWALLMISSING 215 //170
#define LEFTWALLRETURNS 295

#define RIGHTWALLMISSING 185 //150
#define TURN_DELAY 416
#define LEFT_TIMER 850
#define LEFT_TIMER_WITHOUT_WALL 680
#define RIGHT_TIMER 850
#define RIGHT_TIMER_WITHOUT_WALL 691
#define IN_FRONT_OF_WALL 950

int defaultErrorM = 0;

int currentMillis = 0;

int randNumber = 0;

int voidThisTravel = 0;
int voidTime = 0;

int turnsInOneSecond = 0;
int leftTurnClock = 0;

int leftLed = 22;
int middleLed = 18;
int rightLed = 20;

bool disabled = 0;
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

const double Kp = .45;
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

/* setup: Assign peripherals, calibrate, and move forward */
void setup(void)
{
  
  delay(1000);                                                                          //short delay from boot
  pinMode(leftLed, OUTPUT); pinMode(middleLed, OUTPUT); pinMode(rightLed, OUTPUT);      //Initializing LEDS Outputs

  //Serial1.begin(9600);

  myServoL.attach(9); myServoR.attach(10);//stopServo();          //Initialize//  CHECK IF THIS WORKS

  standBy();                                                      //Halt, Ready for calibration upon grant from owner
  calibrate();
  forward();                                                      //Initiate motor rotation
}

/* fingerWave: Returns True if detected finger wave */
int fingerWave()
{
  if(readErrorM() > defaultErrorM + 100)
   return 1;
  else
    return 0;
}

/* standBy: Holds READY LED and waits for user */
void standBy()
{
  digitalWrite(middleLed, HIGH);
  defaultErrorM = readErrorM();
  while(!fingerWave()) {}
  digitalWrite(middleLed, LOW);
  delay(500);
}

/* lightShow: Controls debbugging lights */
void lightShow() {

  // waitforStop, prepringToTurnLeft, preparingToTurnRight are all booleans that get switched on once
  // the appropriate signal is appoved.
  // The signals preparingToTurnLeft and preparingToTurn on hold true if there are no walls in their
  // appropriate direction
  // waitForStop, holds true once a wall confirmed is ahead

  digitalWrite(middleLed, (waitForStop) ? (HIGH) : (LOW));
  digitalWrite(leftLed, (preparingToTurnLeft) ? (HIGH) : (LOW));
  digitalWrite(rightLed, (preparingToTurnRight) ? (HIGH) : (LOW));
}

/* sets boolean waitForStop to true if it confirms there is a wall approaching
The wall approaching is manially set to a distance about 8.5/10 of a cell from the wall.
Confirmation is reached once the value is below the calibration setting a number of
n times */
void readFrontSensorForWall(int n){
  errorM = readErrorM();
  if(!waitForStop  && !disabled){                    // if its not already going to continue straight AND its NOT disabled. (meaning disabled = 0)
    if(errorM >= WALL_APPROACHING)
      ++frontWallApproaching;
    else 
      frontWallApproaching = 0;
    if(frontWallApproaching >= n) {
      waitForStop = 1;
      frontWallApproaching = 0;
    }
  }
  else if(waitForStop && errorM <= WALL_APPROACHING)
    waitForStop = 0;
}

/* sets boolean readLeftSensorForWall to true if it confirms there is no wall on the left side
The wall approaching value is set manually with careful debugging.
Confirmation is reached once the value is below the calibration setting a number of
n times */
void readLeftSensorForWall(int n){                  // Overall, Checks if Left sensor detects a wall or not and raises or drops flag
  errorL = readErrorL();
  if(!preparingToTurnLeft && !disabled){            // if its not already going to turn left AND its NOT disabled. (meaning disabled = 0)
    if(errorL <= LEFTWALLMISSING) {                 // There was a sudden Drop. Increment counter for Left Flag
      ++noLeftWallApproaching;
    }
    else {
      noLeftWallApproaching = 0;                    // Reset Counter
    }
   if(noLeftWallApproaching >= n) {                // Confirmed there is a Missing Left Wall Approaching
     preparingToTurnLeft = 1;                       // Raise Left Flag
     leftClock = millis();   
     noLeftWallApproaching = 0;                     // Reset Counter
    }
  }
  else{                                             // It is Preparing to turn Left, So we will check it has passed the wall to end it
    if(!waitForStop && errorL > LEFTWALLMISSING + 100)    // theres no wall in front and there is a wall on the left
      ++leftWallApproaching; 
    else
        leftWallApproaching = 0;
    if(leftWallApproaching >= 5)                    // confirmation step - confirming there is a wall on the left side
    {
      preparingToTurnLeft = 0;                      // turn flag off
      leftWallApproaching = 0;                      // reseting the counter
    }
  }
}

/* sets boolean readRightSensorForWall to true if it confirms there is no wall on the right side
The wall approaching value is set manually with careful debugging.
Confirmation is reached once the value is below the calibration setting a number of
n times */
void readRightSensorForWall(int n){                  // Overall, Checks if Right sensor detects a wall or not and raises or drops flag
  errorR = readErrorR();
  if(!preparingToTurnRight && !disabled){            // if its not already going to turn right AND its NOT disabled. (meaning disabled = 0)
    if(errorR <= RIGHTWALLMISSING)                   // There was a sudden Drop. Increment counter for Left Flag
      ++noRightWallApproaching;
    else
      noRightWallApproaching = 0;                    // Reset Counter
      
    if(noRightWallApproaching >= n){                 // Confirmed there is a Missing Right Wall Approaching
      preparingToTurnRight = 1;                      // Raise Left Flag
      rightClock = millis();
      noRightWallApproaching = 0;                    // Reset Counter
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

/* At judgment of a wall with a left and right opening, choose randoly which path to take. */
void randomTurn(){
  randNumber = random(2);
  if(randNumber == 0)
    advanceLeftWithWall();
  else
    advanceRightWithWall();
}

/* Given the boolean left right and middle flags, We will choose whether to turn Left or Right*/
void logic(){
  if(waitForStop){ 
    if(errorM >= IN_FRONT_OF_WALL) {                        // We are dead in front of a wall
      if(preparingToTurnLeft && preparingToTurnRight)       // Both options are available, choose a random one.
        randomTurn();
      else if(preparingToTurnLeft)                          // Only left is available. Turn immediately
        advanceLeftWithWall();
      else if(preparingToTurnRight)                         // Only right is available. Turn immediately
        advanceRightWithWall();
      else
        advance180Deg();                                    // None are avaiable so turn around

      // Turn off all Flags
      waitForStop = 0; 
      preparingToTurnLeft = 0;
      preparingToTurnRight = 0;

      // Disable the left and right sensors for a given time
      disableLeftRight();
    }
  }
  else{                                                       // There is no wall in front but there might be an opening on the left or right side
    if(preparingToTurnLeft && preparingToTurnRight)           // Both are open, forget it lets go straight
    {}
    else if(preparingToTurnLeft)
      prepareForLeftTurnWithoutWall();                        // Left side is open, take it.
//    else if(preparingToTurnRight)                           /// FIXME: Right side not ready yet.
//      prepareForRightTurnWithoutWall();
  }
  enableLeftRight();                                          // After a given time, enable the left and right sensors again
}

/* Disables the left and right sensors */
void disableLeftRight(){
  disabled = 1;
  disabledTimer = millis();
}

/* Enables the left and right sensors */
void enableLeftRight(){
  if(millis() >= disabledTimer + 460)                          //After 460 from when it was disabled, enable it
    disabled = 0;
}

/* Prints Serial information */
void debugging(){
  if(preparingToTurnLeft){
    Serial1.println("L       R       M");
    Serial1.print(errorL);
    Serial1.print("    ");
    Serial1.print(errorR);
    Serial1.print("    ");
    Serial1.println(errorM);
  }
  else{
    counter = (counter + 1) % 1000; //Timer
    
    if(counter == 0){
      Serial1.println("");
    }
  }
}

void loop()
{
  collectData();                    // PID for center control
  readFrontSensorForWall(6);        // Searching for Wall Approaching
  readLeftSensorForWall(50);        // Searching for blank left Wall, will flag If True //20
  readRightSensorForWall(50);       // Searching for blank right wall, will flag If True //15
   
  logic();                          // Choose whether to turn left, right, or none
  lightShow();                      // Control Debuggging lights
  debugging();                      // Print Serial Debugging information
}
