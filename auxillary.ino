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

void collectData(){
  errorL = analogRead(leftSensor);
  errorR = analogRead(rightSensor);
  errorM = analogRead(middleSensor);
  
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
