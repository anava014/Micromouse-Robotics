#include <Servo.h>

#define bool unsigned char
#define VELOCITY .0134
#define TRAVELING_CONSTANT .01

const int middleSensor = A2;
const int leftSensor = A1;
const int rightSensor = A0;

const int leftLED = 7;
const int rightLED = 6;

int hugging = 0;
unsigned int totalError = 0;
unsigned int errorL = 0;
unsigned int errorR = 0;
unsigned int errorM = 0;
const double Kp = 1.3;
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
  pinMode(leftLED, OUTPUT);
  pinMode(rightLED, OUTPUT);
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
  if(errorM < 580)
  {
    if(hugging == 0) // If it's closer to the left side
    {
      if(debugMode){
        digitalWrite(leftLED, HIGH);
        digitalWrite(rightLED, LOW);
      }
      if(totalError * Kp < 200)
        myServoR.writeMicroseconds(maxSpeedR + (totalError * Kp));  // Counter clockwise
      else // To prevent it from going backwards
        myServoR.writeMicroseconds(maxSpeedR + 180);  // Counter clockwise
      myServoL.writeMicroseconds(maxSpeedL);
    }
    else if(hugging == 1) // If it's closer to the right side
    {
      if(debugMode){
        digitalWrite(rightLED, HIGH);
        digitalWrite(leftLED, LOW);
      }
      if(totalError * Kp < 200)
        myServoL.writeMicroseconds(maxSpeedL - (totalError * Kp));  // Counter clockwise
      else // To Prevent it from going backwards
        myServoL.writeMicroseconds(maxSpeedL - 180);
      myServoR.writeMicroseconds(maxSpeedR);
    }
    else // Its Kind of in the middle
    {
      if(debugMode){
        digitalWrite(leftLED, LOW);
        digitalWrite(rightLED, LOW);
      }
      myServoR.writeMicroseconds(maxSpeedR);
      myServoL.writeMicroseconds(maxSpeedL);
    }
  }
  else
  {
    if(debugMode){
      digitalWrite(leftLED, LOW);
      digitalWrite(rightLED, LOW);
    }
    myServoR.writeMicroseconds(maxSpeedR);
    myServoL.writeMicroseconds(maxSpeedL);
    if(errorM > 800)
    {
      stopAndDecide();
//      Serial1.print("ErrorM: "); //Code for finding percentage formula
//      Serial1.println(errorM);
    }
  }
}

void stopAndDecide()
{
  stopServo();
  delay(50);
  errorL = analogRead(leftSensor) + skewL;
  errorR = analogRead(rightSensor) + skewR;
  
  if(errorR > errorL) //Theres a Wall on the Right Side
  {
    turnLeft();
    Serial1.println("---------------------------");
    Serial1.println("LEFT TURN");
    Serial1.println("L    R    M");
    Serial1.print(errorL);
    Serial1.print("    ");
    Serial1.print(errorR);
    Serial1.print("    ");
    Serial1.println(errorM);
  }
  else if(errorL > errorR) //Theres a Wall on the Left Side
  {
    turnRight();
//    Serial1.print("ErrorM: "); //Code for finding percentage formula
//    Serial1.println(errorM);
    Serial1.println("---------------------------");
    Serial1.println("RIGHT TURN");
    Serial1.println("L    R    M");
    Serial1.print(errorL);
    Serial1.print("    ");
    Serial1.print(errorR);
    Serial1.print("    ");
    Serial1.println(errorM);
  }
  else //both Sides are equal or empty
  {
    turnRight(); // For now
    Serial1.println("---------------------------");
    Serial1.println("SENSOR ERROR EQUAL");
    Serial1.println("L    R    M");
    Serial1.print(errorL);
    Serial1.print("    ");
    Serial1.print(errorR);
    Serial1.print("    ");
    Serial1.println(errorM);
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
  traveledOneCell(18);
  delay(100);
}
