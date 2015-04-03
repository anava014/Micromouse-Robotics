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
      delay(delayTime);    
  }
}

void advanceLeft()
{
  myServoR.writeMicroseconds(maxSpeedR);
  myServoL.writeMicroseconds(maxSpeedR); ///NEED TO WORK ON THIS
  delay(TURN_DELAY);
  stopServo();
//  delay(50);
}

void advance180Deg(){
  myServoR.writeMicroseconds(maxSpeedR);
  myServoL.writeMicroseconds(maxSpeedR); 
  delay(TURN_DELAY * 2);
}

void advanceLeftNoDelay()
{
  myServoR.writeMicroseconds(maxSpeedR);
  myServoL.writeMicroseconds(maxSpeedR); ///NEED TO WORK ON THIS
  delay(TURN_DELAY);
}

void advanceRight()
{
  myServoR.writeMicroseconds(maxSpeedL); ///NEED TO WORK ON THIS
  myServoL.writeMicroseconds(maxSpeedL);
  delay(TURN_DELAY);
  stopServo();
//  delay(50);
  eastDirection();
}



void prepareForLeftTurn(){
    leftTimer = (leftTimer + 1) % LEFT_TIMER; //Timer
    if(leftTimer == 0){
      advanceLeft();
      preparingToTurnLeft = 0;
      preparingToTurnRight = 0;
      disableLeftRight();
    }
}

void prepareForRightTurn(){
  //waitingForRightTimer = 1;
    rightTimer = (rightTimer + 1) % 75; //Timer
    if(rightTimer == 0){
      Serial1.print("There is no wall on the Left side ");
      advanceRight();
      //waitingForRightTimer = 0;
      //waitingForRightRespawn = 1;
    }
    totalError = 0;
    function = 'F';
}

void collectData(){
  errorL = analogRead(leftSensor) + skewL;
  errorR = analogRead(rightSensor) + skewR;
  errorM = analogRead(middleSensor) + skewM;
//  
//  counter = (counter + 1) % 10; //Timer
//  
//  if(counter == 0)
//  {
//    Serial1.println("L    R    M");
//    Serial1.print(errorL);
//    Serial1.print("    ");
//    Serial1.print(errorR);
//    Serial1.print("    ");
//    Serial1.println(errorM);
//  }
  
  if((errorL <= LEFTWALLMISSING || preparingToTurnLeft) && (errorR <= RIGHTWALLMISSING || preparingToTurnRight)) // Missing Both walls!!! >:/
  {
    totalError = 0;
    function = 'F';
  }
  else if(errorL <= LEFTWALLMISSING || preparingToTurnLeft) // Missing Left Wall, PID Concentrates only on Right Error
  {
    totalError = 500 - errorR;
    function = 2;
  }
  else if(errorR <= RIGHTWALLMISSING || preparingToTurnRight) // Missing Left Wall, PID Concentrates only on Left Error
  {
    totalError = 500 - errorL;
    function = 3;
  }
  else if(errorL > errorR && (errorL-errorR > 7))
  {
    totalError = errorL - errorR;
    function = 0; // Left
  }
  else if(errorR > errorL && (errorR-errorL > 7))
  {
    totalError = errorR - errorL;
    function = 1; // Right
  }
  else
  {
    totalError = 0;
    function = 'F';
  }
  
  pid(totalError, function);
}

void measureOneSecond(){
  forward();
  if(millis() >= time + 1000)
  {
    time = millis() - time;
    stopServo();
    Serial1.println(time);
    while(1) {}
  }
}

int readErrorL(){
  return analogRead(leftSensor) + skewL;
}

int readErrorR(){
  return analogRead(rightSensor) + skewR;
}

int readErrorM(){
  return analogRead(middleSensor) + skewM;
}
