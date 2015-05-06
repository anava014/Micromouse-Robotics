void stopServo()
{
    myServoL.writeMicroseconds(1500);  // Stopped
    myServoR.writeMicroseconds(1500);  // Stopped
}

void forward(){
    myServoR.writeMicroseconds(maxSpeedR);
    myServoL.writeMicroseconds(maxSpeedL);
}

void reverse(){
    myServoR.writeMicroseconds(maxSpeedL - 100);
    myServoL.writeMicroseconds(maxSpeedR + 100);
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



void advance180Deg(){
  myServoR.writeMicroseconds(maxSpeedL);
  myServoL.writeMicroseconds(maxSpeedL); 
  delay(TURN_DELAY * 2);
  stopServo();
  manualCell();
  //bumpIt();
}

void bumpIt(){
  reverse();
  delay(300);
  stopServo();
  //callCellTimer();
}

void advanceLeft()
{
  stopServo();
  delay(100);
  myServoR.writeMicroseconds(maxSpeedR);
  myServoL.writeMicroseconds(maxSpeedR);
  delay(TURN_DELAY);
  stopServo();
  delay(100);
}


void advanceRight()
{
  stopServo();
  delay(100);
  myServoR.writeMicroseconds(maxSpeedL); ///NEED TO WORK ON THIS
  myServoL.writeMicroseconds(maxSpeedL);
  delay(TURN_DELAY);
  stopServo();
  delay(100);
}

void advanceRightWithWall(){
  advanceRight();
}

void advanceLeftWithWall(){
  advanceLeft();
}

void prepareForLeftTurnWithoutWall(){
  //if(millis() > leftClock + LEFT_TIMER_WITHOUT_WALL){
    if(readErrorL() > LEFTWALLRETURNS){
      delay(100);
      advanceLeft();
      preparingToTurnLeft = 0;
      preparingToTurnRight = 0;
      disableLeftRight();
      forward();
   }
}

void prepareForRightTurnWithoutWall(){
  if(millis() > rightClock + RIGHT_TIMER_WITHOUT_WALL){
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
//    Serial1.println("L    R    M");
    //Serial1.println(errorL);
//    Serial1.print("    ");
//    Serial1.print(errorR);
//    Serial1.print("    ");
//    Serial1.println(errorM);
  //}
  if(errorL >= 2300){ //Dragging on Left Side
    totalError = 0;
    function = 4;
  }
  else if(errorR >= 1200){ //Dragging on Right Side
    totalError = 0;
    function = 5;
  }
  
  else if(!disabled){
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
  }
  else{
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

double readErrorL(){
  return analogRead(leftSensor) * skewL;
}

double readErrorR(){
  return analogRead(rightSensor) * skewR;
}

double readErrorM(){
  return analogRead(middleSensor);
}
