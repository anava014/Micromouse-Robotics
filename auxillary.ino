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
  time += TURN_DELAY;
  stopServo();
  turnsInOneSecond++;
  leftTurnClock = millis();
}

void advance180Deg(){
  myServoR.writeMicroseconds(maxSpeedR);
  myServoL.writeMicroseconds(maxSpeedR); 
  delay(TURN_DELAY * 2);
  time += TURN_DELAY * 2;
  //time += 200;
  stopServo();
//  turnsInOneSecond++;
//  turnsInOneSecond++;
//  leftTurnClock = millis();
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
  time += TURN_DELAY;
  stopServo();
  //eastDirection();
}



void prepareForLeftTurn(){
    if(millis() > leftClock + LEFT_TIMER){
      advanceLeft();
      preparingToTurnLeft = 0;
      preparingToTurnRight = 0;
      disableLeftRight();
    }
}

void prepareForRightTurn(){
    if(millis() > rightClock + RIGHT_TIMER){
      advanceRight();
      preparingToTurnLeft = 0;
      preparingToTurnRight = 0;
      disableLeftRight();
    }
}

void prepareForRightTurnWithoutWall(){
  if(millis() > rightClock + RIGHT_TIMER_WITH_WALL){
      advanceRight();
      preparingToTurnLeft = 0;
      preparingToTurnRight = 0;
      disableLeftRight();
    }
}

void collectData(){
  errorL = readErrorL();
  errorR = readErrorR();
  errorM = readErrorM();
  
//  counter = (counter + 1) % 10000; //Timer
//  
//  if(counter == 0)
//  {
//    Serial.println("L    R    M");
//    Serial.print(errorL);
//    Serial.print("    ");
//    Serial.print(errorR);
//    Serial.print("    ");
//    Serial.println(errorM);
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
  else if(errorR <= RIGHTWALLMISSING || preparingToTurnRight) // Missing Right Wall, PID Concentrates only on Left Error
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
  return analogRead(leftSensor) * skewL;
}

int readErrorR(){
  return analogRead(rightSensor) * skewR;
}

int readErrorM(){
  return analogRead(middleSensor);
}
