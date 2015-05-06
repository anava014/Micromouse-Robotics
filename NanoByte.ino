#include <Servo.h>

#define bool unsigned char
#define VELOCITY .01755  //.0169
#define TRAVELING_CONSTANT .015
#define WALL_APPROACHING 90
#define LEFTWALLMISSING 205 //170
#define LEFTWALLRETURNS 295

#define RIGHTWALLMISSING 150
#define TURN_DELAY 416
#define LEFT_TIMER 850
#define LEFT_TIMER_WITHOUT_WALL 680
#define RIGHT_TIMER 850
#define RIGHT_TIMER_WITHOUT_WALL 691
#define IN_FRONT_OF_WALL 650

/*
Currently working on Left turns.
Too sensitive, Applyinh more confirmations (from 6 to 10)
And lowering LEFT.. From 200 to 190
*/

int defaultErrorM = 0;

int currentMillis = 0;

int randNumber = 0;

int voidThisTravel = 0;
int voidTime = 0;

int gridCount = 0;

int turnsInOneSecond = 0;
int leftTurnClock = 0;

int leftLed = 22;
int middleLed = 18;
int rightLed = 20;

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

void setup(void)
{
  delay(1000);
  pinMode(leftLed, OUTPUT);
  pinMode(middleLed, OUTPUT);
  pinMode(rightLed, OUTPUT);
  pinMode(cellLED, OUTPUT);
  //gridInit();

  //Serial1.begin(9600);
  myServoL.attach(9);  // Servo is connected to digital pin 9 
  myServoR.attach(10);  // Servo is connected to digital pin 10 
  stopServo();
  
  waitForOK();
  
  calibrate();
  time = millis();
  forward();
}

int fingerWave()
{
  if(readErrorM() > defaultErrorM + 100)
   return 1;
  else
    return 0;
}

void waitForOK()
{
  digitalWrite(middleLed, HIGH);
  defaultErrorM = readErrorM();
  while(!fingerWave()) {}
  digitalWrite(middleLed, LOW);
  delay(500);
}

void lightShow() {
  digitalWrite(middleLed, (waitForStop) ? (HIGH) : (LOW));
  digitalWrite(leftLed, (preparingToTurnLeft) ? (HIGH) : (LOW));
  digitalWrite(rightLed, (preparingToTurnRight) ? (HIGH) : (LOW));
  
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

void readFrontSensorForWall(){
  errorM = readErrorM();
  if(!waitForStop  && !disabled){                                 // if its not already going to continue straight AND its NOT disabled. (meaning disabled = 0)
    if(errorM >= WALL_APPROACHING)
      ++frontWallApproaching;
    else 
      frontWallApproaching = 0;
    if(frontWallApproaching >= 6) {
      waitForStop = 1;
      frontWallApproaching = 0;
    }
  }
  else if(waitForStop && errorM <= WALL_APPROACHING)
    waitForStop = 0;
}

void readLeftSensorForWall(){                      //Overall, Checks if Left sensor detects a wall or not and raises or drops flag
  errorL = readErrorL();
  if(!preparingToTurnLeft && !disabled){            // if its not already going to turn left AND its NOT disabled. (meaning disabled = 0)
    if(errorL <= LEFTWALLMISSING) {                 //There was a sudden Drop. Increment counter for Left Flag
      ++noLeftWallApproaching;
    }
    else {
      noLeftWallApproaching = 0;                     // Reset Counter
    }
   if(noLeftWallApproaching >= 15) {                  //Confirmed there is a Missing Left Wall Approaching
     preparingToTurnLeft = 1;                        //Raise Left Flag
     leftClock = millis();    
     noLeftWallApproaching = 0;                      //Reset Counter
    }
  }
  else if(preparingToTurnLeft && !waitForStop && errorL >= LEFTWALLRETURNS + 100)
    preparingToTurnLeft = 0;
}

void readRightSensorForWall(){                       //Overall, Checks if Right sensor detects a wall or not and raises or drops flag
  errorR = readErrorR();
  if(!preparingToTurnRight && !disabled){            // if its not already going to turn right AND its NOT disabled. (meaning disabled = 0)
    if(errorR <= RIGHTWALLMISSING)                   //There was a sudden Drop. Increment counter for Left Flag
      ++noRightWallApproaching;
    else
      noRightWallApproaching = 0;                    // Reset Counter
      
    if(noRightWallApproaching >= 15){                 //Confirmed there is a Missing Right Wall Approaching
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

void randomTurn(){
  randNumber = random(2);
  if(randNumber == 0)
    advanceLeftWithWall();
  else
    advanceRightWithWall();
}

void logic(){
  if(waitForStop){// || errorM >= IN_FRONT_OF_WALL){
    if(errorM >= IN_FRONT_OF_WALL) {
      if(preparingToTurnLeft && preparingToTurnRight)
        randomTurn();
      else if(preparingToTurnLeft)
        advanceLeftWithWall();
      else if(preparingToTurnRight)
        advanceRightWithWall();
      else
        advance180Deg();

      waitForStop = 0; 
      preparingToTurnLeft = 0;
      preparingToTurnRight = 0;
      disableLeftRight();
    }
  }
  else{ 
    if(preparingToTurnLeft && preparingToTurnRight)
    {}
    else if(preparingToTurnLeft)
      prepareForLeftTurnWithoutWall();
//    else if(preparingToTurnRight)
//      prepareForRightTurnWithoutWall();
  }
  enableLeftRight();
}

void disableLeftRight(){
  disabled = 1;
  disabledTimer = millis();
}
void enableLeftRight(){
  if(millis() > disabledTimer + 460)
    disabled = 0;
}

void turnOffCellLed(){
  if(millis() > speakerTimer + 100)
    digitalWrite(cellLED, LOW);
}

void turnCounter(){
  currentMillis = millis();
  if(turnsInOneSecond >= 2 && currentMillis < leftTurnClock + 1200){
    time += 200;
    turnsInOneSecond = 0;
  }
  else if(millis() > leftTurnClock + 1200)
    turnsInOneSecond = 0;
}

void disableTravelVoid(){
  if(millis() > voidTime + 150){
    voidThisTravel = 0;
  }
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
