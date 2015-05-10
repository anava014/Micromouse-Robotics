int mainGrid [16][16];
int posY = 15;
int posX = 0;
int facing = 0;
int firstTimeIn = 0;

void gridInit(){
  for(int i = 0; i < 16; ++i)
    for(int j = 0; j < 16; ++j)
      mainGrid[i][j] = '0';
      
  mainGrid[15][0] = '1'; //Initial Position
  //--posY;
}

void printGrid(){
  for(int i = 0; i < 16; ++i){
    for(int j = 0; j < 16; ++j){
      Serial1.print(mainGrid[i][j] - '0');
      Serial1.print(" ");
    }
    Serial1.println("");
  }
  Serial1.println("");
}

void stopAtCell(int posY, int posX){
  if(mainGrid[posY][posX] == '1'){//  || gridCount >= 12){
    stopServo();
    printGrid();
    while(1) {}
  } 
}

void markCell(){
  if(facing == 0 && posY > 0) //Going North
    --posY;
  else if(facing == 1 && posY < 15)//Going South
    ++posY;
  else if(facing == 2 && posX < 15)//Going East
    ++posX;
  else if(facing == 3 && posX > 0)//Going West
    --posX;
  mainGrid[posY][posX] = '1';
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

//void manualCell(){
//  markCell();
//  digitalWrite(cellLED, HIGH);
//  speakerTimer = millis();
//  isTraveling = 0;
//}

void callCellTimer(){
  time = millis();
}

//void traveledOneCell(int square){
//  if(isTraveling && !voidThisTravel){
//    if(VELOCITY * (millis() - time) >= square - TRAVELING_CONSTANT){
//      markCell();
//      //printGrid();
//      digitalWrite(cellLED, HIGH);
//      speakerTimer = millis();
//      isTraveling = 0;
//    }
//  }
//  if(!isTraveling){
//    time = millis();
//    isTraveling = 1;
//  }
//  
//}

void changeDirectionLeft(){
  if(facing == 0){
    westDirection();
  }
  else if(facing == 3){
    southDirection();
  }
  else if(facing == 2){
    northDirection();
  }
  else if(facing == 1){
    eastDirection();
  }
}

void changeDirectionRight(){
  if(facing == 0){
    eastDirection();
  }
  else if(facing == 3){
    northDirection();
  }
  else if(facing == 2){
    southDirection();
  }
  else if(facing == 1){
    westDirection();
  }
}

void eastDirection(){
  facing = 2;
}

void westDirection(){
  facing = 3;
}

void northDirection(){
  facing = 0;
}

void southDirection(){
  facing = 1;
}

void voidTravel(){
  voidThisTravel = 1;
  voidTime = millis();
}



