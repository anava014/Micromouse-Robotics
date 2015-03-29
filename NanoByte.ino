#include <Servo.h>

#define bool unsigned char
#define VELOCITY .0134
#define TRAVELING_CONSTANT .01
#define WALL_APPROACHING 530

const int middleSensor = A2;
const int leftSensor = A1;
const int rightSensor = A0;

int hugging = 0;
int totalError = 0;
int errorL = 0;
int errorR = 0;
int errorM = 0;
const double Kp = 2;
unsigned int counter = 0;
unsigned int leftTimer = 0;
unsigned int skewL = 0;
unsigned int skewR = 0;
unsigned int skewM = 0;
const int maxSpeedL = 1700;
const int maxSpeedR = 1300;
unsigned char debugMode = 0;
unsigned char waitingForTimer = 0;

Servo myServoL;
Servo myServoR;

int time = 0;
bool isTraveling = 1;
int timeToTravel = 0;

void setup(void)
{
  gridInit();
  
  Serial1.begin(9600);
  myServoL.attach(9);  // Servo is connected to digital pin 9 
  myServoR.attach(10);  // Servo is connected to digital pin 10 
  stopServo();
  calibrate();
  time = millis();
  forward();
}

void Advance(int totalError, unsigned char hugging)
{
  if(errorM < WALL_APPROACHING)
  {
    if(hugging == 0) // If it's closer to the left side
    {
      if(totalError * Kp < 200)
        myServoR.writeMicroseconds(maxSpeedR + (totalError * Kp));  // Counter clockwise
      else // To prevent it from going backwards
        myServoR.writeMicroseconds(maxSpeedR + 180);  // Counter clockwise
      myServoL.writeMicroseconds(maxSpeedL);
    }
    else if(hugging == 1) // If it's closer to the right side
    {
      if(totalError * Kp < 200)
        myServoL.writeMicroseconds(maxSpeedL - (totalError * Kp));  // Counter clockwise
      else // To Prevent it from going backwards
        myServoL.writeMicroseconds(maxSpeedL - 180);
      myServoR.writeMicroseconds(maxSpeedR);
    }
    else if(hugging == 2) // Using for "Other" FOR NOW
    {
      
    }
    else // Its Kind of in the middle
    {
      myServoR.writeMicroseconds(maxSpeedR);
      myServoL.writeMicroseconds(maxSpeedL);
    }
  }
  else
  {
    myServoR.writeMicroseconds(maxSpeedR);
    myServoL.writeMicroseconds(maxSpeedL);
    if(errorM > 800)
    {
      stopAndDecide();
    }
  }
}

void stopAndDecide()
{
  stopServo();
  delay(50);
  errorL = readErrorL();
  errorR = readErrorR();
  
  if(errorR > errorL) //Theres a Wall on the Right Side
  {
    advanceLeft();
    Serial1.println("---------------------------"); Serial1.println("LEFT TURN"); Serial1.println("L    R    M");
    Serial1.print(errorL); Serial1.print("    "); Serial1.print(errorR); Serial1.print("    "); Serial1.println(errorM);
  }
  else if(errorL > errorR) //Theres a Wall on the Left Side
  {
    advanceRight();
//    Serial1.print("ErrorM: "); //Code for finding percentage formula
//    Serial1.println(errorM);
    Serial1.println("---------------------------"); Serial1.println("RIGHT TURN"); Serial1.println("L    R    M");
    Serial1.print(errorL); Serial1.print("    "); Serial1.print(errorR); Serial1.print("    "); Serial1.println(errorM);
  }
  else //both Sides are equal or empty
  {
    advanceRight(); // For now
    Serial1.println("---------------------------"); Serial1.println("SENSOR ERROR EQUAL"); Serial1.println("L    R    M");
    Serial1.print(errorL); Serial1.print("    "); Serial1.print(errorR); Serial1.print("    "); Serial1.println(errorM);
  }
}

void travelDistance(unsigned char centimeters){
  if(isTraveling){
    if(millis() >= time + timeToTravel)
    {
      isTraveling = 0;
      stopServo();
      while(1) {}
    }
  }
  else{
    time = millis();
    timeToTravel = centimeters/VELOCITY;
    isTraveling = 1;
  }
}

void traveledOneCell(int square){
  if(isTraveling){
    if(VELOCITY * (millis() - time) >= square - TRAVELING_CONSTANT){
      Serial1.println(VELOCITY * (millis() - time));
      markCell();
      printGrid();
      isTraveling = 0;
    }
  }
  if(!isTraveling){
    time = millis();
    isTraveling = 1;
  }
  
}



void loop()
{
  collectData();
  
  Advance(totalError, hugging);
  
  errorL = errorR = errorM = 0;
  //traveledOneCell(18);
  delay(100);
}
