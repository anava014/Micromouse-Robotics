unsigned char mainGrid [16][16];
unsigned char posY = 15;
unsigned char posX = 0;
unsigned char direction = 0;

void gridInit(){
  for(unsigned char i = 0; i < 16; ++i)
    for(unsigned char j = 0; j < 16; ++j)
      mainGrid[i][j] = '0';
      
  mainGrid[15][0] = '1'; //Initial Position
}

void printGrid(){
  for(unsigned char i = 0; i < 16; ++i){
    for(unsigned char j = 0; j < 16; ++j){
      Serial1.print(mainGrid[i][j] - '0');
      Serial1.print(" ");
    }
    Serial1.println("");
  }
  Serial1.println("");
}

void markCell(){
  mainGrid[posY][posX] = '1';
  if(direction == 0 && posY > 0) //Going North
    --posY;
  else if(direction == 1 && posY < 15)//Going South
    ++posY;
  else if(direction == 2 && posX < 15)//Going East
    ++posX;
  else if(direction == 3 && posX > 0)//Going West
    --posX;
}

void eastDirection(){
  direction = 2;
}

void westDirection(){
  direction = 3;
}

void northDirection(){
  direction = 0;
}

void southDirection(){
  direction = 1;
}
