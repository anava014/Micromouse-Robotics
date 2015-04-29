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

  skewL = 500 / errorL;
  skewR = 500 / errorR;
  skewM = 500 / errorM;
  
  Serial1.println("Skew: ");
  Serial1.print(skewL);
  Serial1.print("    ");
  Serial1.print(skewR);
  Serial1.print("    ");
  Serial1.println(skewM);
  
  Serial1.println("Final: ");
  Serial1.print(errorL * skewL);
  Serial1.print("    ");
  Serial1.print(errorR * skewR);
  Serial1.print("    ");
  Serial1.println(errorM * skewM);
  
  Serial1.println("Calibrating Complete");

  errorL = errorR = errorM = 0;
  delay(1000);
}

void calibrateTurns(){
  advanceLeft();
  stopServo();
  delay(1000);
  advanceLeft();
  stopServo();
  delay(1000);
  advanceLeft();
  stopServo();
  delay(1000);
  advanceLeft();
  stopServo();
  delay(1000);
  
  while(1) {}
}

void calibrate180Turns(){
  forward();
  delay(1000);
  advanceLeft();
  forward();
  delay(1000);
  advanceLeft();
  forward();
  delay(1000);
  advanceLeft();
  forward();
  delay(1000);
  advanceLeft(); 
}
