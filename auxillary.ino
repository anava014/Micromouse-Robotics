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
  delay(515);
  stopServo();
  delay(50);
}

void advanceLeftNoDelay()
{
  myServoR.writeMicroseconds(maxSpeedR);
  myServoL.writeMicroseconds(maxSpeedR); ///NEED TO WORK ON THIS
  delay(515);
}

void advanceRight()
{
  myServoR.writeMicroseconds(maxSpeedL); ///NEED TO WORK ON THIS
  myServoL.writeMicroseconds(maxSpeedL);
  delay(515);
  stopServo();
  delay(50);
  eastDirection();
}

void collectData(){
  errorL = analogRead(leftSensor) + skewL;
  errorR = analogRead(rightSensor) + skewR;
  errorM = analogRead(middleSensor) + skewM;
  
  counter = (counter + 1) % 10; //Timer
  
  if(counter == 0)
  {
    Serial1.println("L    R    M");
    Serial1.print(errorL);
    Serial1.print("    ");
    Serial1.print(errorR);
    Serial1.print("    ");
    Serial1.println(errorM);
  }
  
//  if(errorR < 475 && errorM < errorL < 475) //There is no wall on both sides
//  {}

  if(waitingForTimer || ((errorR - errorL) > 40  && errorM < WALL_APPROACHING)) //There is no wall on the left side
  {
    waitingForTimer = 1;
    leftTimer = (leftTimer + 1) % 15; //Timer
    if(leftTimer == 0){
      Serial1.print("There is no wall on the Left side ");
      advanceLeft();
      waitingForTimer = 0;
    }
    totalError = 0;
    hugging = 2; //2 Does nothing for now
  }
  
  if(errorR < 410 && errorM < WALL_APPROACHING) //There is no wall on the right side
  {
      Serial1.print("There is no wall on the Right side ");
      Serial1.println(errorL);
      totalError = 500 - errorL;
      if(totalError < 0){ //Too close to Left Side
        myServoR.writeMicroseconds(maxSpeedR + (totalError * Kp));
        myServoL.writeMicroseconds(maxSpeedL);
      }
      else{ //Too close to Right Side
        myServoL.writeMicroseconds(maxSpeedL - (totalError * Kp));  // Counter clockwise
        myServoR.writeMicroseconds(maxSpeedR);
      }
      hugging = 2;
      return;
  }
  
  else if(errorL > 500 && errorR > 500 && errorM > 700) //DO 180 turn
  {
    advanceLeftNoDelay();
    advanceLeft();
    totalError = 0;
    hugging = 2; //2 Does nothing for now
  }
  
  else if(errorL > errorR && (errorL-errorR > 7))
  {
    Serial1.println("Closer to Left Side");
    totalError = errorL - errorR;
    hugging = 0; // Left
  }
  else if(errorR > errorL && (errorR-errorL > 7))
  {
    Serial1.println("Closer to Right Side");
    totalError = errorR - errorL;
    hugging = 1; // Right
  }
  else
  {
    totalError = 0;
    hugging = 'F';
  }
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

unsigned int readErrorL(){
  return analogRead(leftSensor) + skewL;
}

unsigned int readErrorR(){
  return analogRead(rightSensor) + skewR;
}
