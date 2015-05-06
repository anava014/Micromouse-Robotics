void pid(int totalError, unsigned char function)
{
  if(function == 0) // If it's closer to the left side
  {
    if(totalError * Kp < 200)
      myServoR.writeMicroseconds(maxSpeedR + (totalError * Kp));  // Counter clockwise
    else // To prevent it from going backwards
      myServoR.writeMicroseconds(maxSpeedR + 180);  // Counter clockwise
    myServoL.writeMicroseconds(maxSpeedL);
  }
  else if(function == 1) // If it's closer to the right side
  {
    if(totalError * Kp < 200)
      myServoL.writeMicroseconds(maxSpeedL - (totalError * Kp));  // Counter clockwise
    else // To Prevent it from going backwards
      myServoL.writeMicroseconds(maxSpeedL - 180);
    myServoR.writeMicroseconds(maxSpeedR);
  }
  else if(function == 2) // No Wall on Left Side
  {
    if(totalError > 0){ //Too close to Left Side
        if((totalError * Kp) < 200)
          myServoR.writeMicroseconds(maxSpeedR + (totalError * Kp));
        else
          myServoR.writeMicroseconds(maxSpeedR + 180);
        myServoL.writeMicroseconds(maxSpeedL);
    }
    else{ //Too close to Right Side
      if((totalError * Kp * 100) > -200)
        myServoL.writeMicroseconds(maxSpeedL + (totalError * Kp * 100));
      else
        myServoL.writeMicroseconds(maxSpeedL + 180);
      myServoR.writeMicroseconds(maxSpeedR);
    }
 }
 else if(function == 3) // No Wall on Right Side
  {
    if(totalError > 0){ //Too close to Right Side
        myServoR.writeMicroseconds(maxSpeedR);
        if((totalError * Kp * 1.5) < 200)
          myServoL.writeMicroseconds(maxSpeedL - (totalError * Kp * 1.5));
        else 
           myServoL.writeMicroseconds(maxSpeedL - 180);
    }
    else{ //Too close to Left Side
      myServoL.writeMicroseconds(maxSpeedL);  // Counter clockwise //FIX ME NEEDS BOUNDARIES SO IT WONT GO BACKWARDS
      if((totalError * Kp) > -200)
        myServoR.writeMicroseconds(maxSpeedR - (totalError * Kp));
      else
        myServoR.writeMicroseconds(maxSpeedR - 180);
    }
 }
 else if(function == 4) //Dragging on Left Side
 {
   myServoR.writeMicroseconds(maxSpeedR + 190);
   myServoL.writeMicroseconds(maxSpeedL);
 }
 else if(function == 5) //Dragging on Right Side
 {
   myServoR.writeMicroseconds(maxSpeedR);
   myServoL.writeMicroseconds(maxSpeedL - 190);
 }
  else // Its Kind of in the middle
  {
    myServoR.writeMicroseconds(maxSpeedR);
    myServoL.writeMicroseconds(maxSpeedL);
  }
}
