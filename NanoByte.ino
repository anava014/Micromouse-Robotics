#include <Servo.h>

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
const double Kp = 1;
unsigned int counter = 0;
unsigned int skewL = 0;
unsigned int skewR = 0;
unsigned int skewM = 0;
const int maxSpeedL = 1700;
const int maxSpeedR = 1300;
unsigned char debugMode = 0;

Servo myServoL;
Servo myServoR;

void setup(void)
{
  pinMode(leftLED, OUTPUT);
  pinMode(rightLED, OUTPUT);
  
  Serial1.begin(9600);
  myServoL.attach(9);  // Servo is connected to digital pin 9 
  myServoR.attach(10);  // Servo is connected to digital pin 10 
  stopServo();
  calibrate();
  accelerate(1);
}

void calibrate(){
  for(int i = 0; i < 100; ++i)
  {
    errorL += analogRead(leftSensor);
    errorR += analogRead(rightSensor);
    errorM += analogRead(middleSensor);
    Serial1.print(errorL);
    Serial1.print("    ");
    Serial1.print(errorR);
    Serial1.print("    ");
    Serial1.println(errorM);
  }
  
  errorL /= 100;
  errorR /= 100;
  errorM /= 100;
  Serial1.println("Error: ");
  Serial1.print(errorL);
  Serial1.print("    ");
  Serial1.print(errorR);
  Serial1.print("    ");
  Serial1.println(errorM);

  skewL = 500 - errorL;
  skewR = 500 - errorR;
  skewM = 500 - errorM;
  
  Serial1.println("Skew: ");
  Serial1.print(skewL);
  Serial1.print("    ");
  Serial1.print(skewR);
  Serial1.print("    ");
  Serial1.println(skewM);
  
  Serial1.println("Final: ");
  Serial1.print(errorL + skewL);
  Serial1.print("    ");
  Serial1.print(errorR + skewR);
  Serial1.print("    ");
  Serial1.println(errorM + skewM);
  
  Serial1.println("Calibrating Complete");
  if(debugMode){
    digitalWrite(leftLED, HIGH);
    digitalWrite(rightLED, HIGH);
    delay(250);
    digitalWrite(leftLED, LOW);
    digitalWrite(rightLED, LOW);
    delay(250);
    digitalWrite(leftLED, HIGH);
    digitalWrite(rightLED, HIGH);
    delay(250);
    digitalWrite(leftLED, LOW);
    digitalWrite(rightLED, LOW);
    delay(250);
  }
  errorL = errorR = errorM = 0;
  delay(2000);
}

void stopServo()
{
   myServoL.writeMicroseconds(1500);  // Stopped
   myServoR.writeMicroseconds(1500);  // Stopped
}

void forward(){
  myServoR.writeMicroseconds(maxSpeedR);
  myServoL.writeMicroseconds(maxSpeedL);
}

void accelerate(int delayTime){
  for(int i = 0; i < 200; ++i)
  {
    myServoR.writeMicroseconds(1500 - i);
    myServoL.writeMicroseconds(1500 + i);
    delay(delayTime);    
  }
}

void decelerate(int delayTime){
  for(int i = 0; i < 200; ++i)
  {
    myServoR.writeMicroseconds(maxSpeedR + i);
    myServoL.writeMicroseconds(maxSpeedL - i);
    //delay(delayTime);    
  }
}

void moveStraight(int totalError, unsigned char hugging)
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

void turnLeft()
{
  digitalWrite(leftLED, HIGH);
  digitalWrite(rightLED, LOW);
  myServoR.writeMicroseconds(maxSpeedR);
  myServoL.writeMicroseconds(maxSpeedR);
  delay(495);
  stopServo();
  delay(50);
  digitalWrite(leftLED, LOW);
  digitalWrite(rightLED, LOW);
}

void turnRight()
{
  digitalWrite(leftLED, LOW);
  digitalWrite(rightLED, HIGH);
  myServoR.writeMicroseconds(maxSpeedL);
  myServoL.writeMicroseconds(maxSpeedL);
  delay(495);
  stopServo();
  delay(50);
  digitalWrite(leftLED, LOW);
  digitalWrite(rightLED, LOW);
}

unsigned int readAverage(int averageOf){
  for(int i = 0; i < averageOf; ++i)
  {
    errorL += analogRead(leftSensor);
    errorR += analogRead(rightSensor);
    errorM += analogRead(middleSensor);
  }
  errorL = (errorL/averageOf) + skewL;
  errorR = (errorR/averageOf) + skewR;
  errorM = (errorM/averageOf) + skewM;
}

void loop()
{
  readAverage(10);
  
  counter = (counter + 1) % 10; //Timer
  
  if(counter == 0)
  {
    Serial1.print("sensor middle: ");
    Serial1.println(errorM);
  }
  
   if(counter == 0) {
    
    Serial1.print("sensor left: ");
    Serial1.println(errorL);
    Serial1.print("sensor right: ");
    Serial1.println(errorR);
}
  if(errorL > errorR && (errorL-errorR > 7))
  {
    totalError = errorL - errorR;
    hugging = 0; // Left
    
//    if(counter == 0) {
//      Serial1.print("Total error: ");
//      Serial1.println(totalError);
//      Serial1.println("||          .           .");
//      }
  }
  else if(errorR > errorL && (errorR-errorL > 7))
  {
    totalError = errorR - errorL;
    hugging = 1; // Right
//    if(counter == 0) {
//      Serial1.print(" Total error: ");
//      Serial1.println(totalError);
//      Serial1.println(".           .          ||");
//    }
  }
  else
  {
    totalError = 0;
    hugging = 'F';
//    if(counter == 0) 
//      Serial1.println(".           ||           .");
  }
  
  moveStraight(totalError, hugging);
  
  errorL = errorR = errorM = 0;
  delay(100);
  
}
