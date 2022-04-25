// include libraries
#include <LiquidCrystal.h>

int intGameSpeedEasy = 10;
int intGameSpeedMedium = 25;
int intGameSpeedHard = 80;

// Define pins
#define btnEnter A0
#define btn1 15
#define btn2 16
#define btn3 17
#define btn4 18

// create LCD objects (n, ~, n, ~, ~, n)
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// set up game array
int arrGame[16][2] = {   // 2 rows 16 columns on one lcd
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0}
};

//set up variables for the game
boolean bolPlay;            //tracks if the player 
int intScore;               //tracks the player's score
int intDiff;                //just an estetic thing for telling what difficulty the game is on</p><p>//set up variables for input
int intEnter;               //trackes if the user presses the enter button
int intInput;               //tracks which buttons the user presses
boolean bolTilePressed;     //ensure the player doesn't accidently press a button 5x and lose</p><p>//set up variables for turn
int intTick;                //counts up millies (per loop) until intDelay
int intDelay;               //the time the program waits till the next turn in millis
int intGameSpeed;

//abit of debug options
boolean bolSerialBoard;     //when true will print the board in the serial monitor</p><p>//the setup that will be run once
void setup() {
  Serial.begin(9600);       //start the serial monitor
  
  //set up the variables
  bolPlay = false;
  intScore = 0;
  intTick = 0;
  intDelay = 1000;
  intDiff = 1;
  intGameSpeed = intGameSpeedMedium;
  //begin lcd's
  lcd.begin(16,2);
}

//the loop which will be run every 10 millisecons
void loop() {
  input();                        //check for playing input
  if (bolPlay == true){     
    if (intTick >= intDelay){     //check to see if the game should play a turn or continue to wait
      Serial.println("~~~~~~~~~");//print to signify the board moving on
      //writeSerial();              //if option is enabled write the board into serial
      buttonsGame();              //check for player inputs
      playBoard();                //move the board and add a new tile
      clearLcd();                 //clean the LCDs before drawing
      drawBoard();                //draw the board onto the lcd's
      bottomCheck();
      intTick = 0;                //reset intTick
    } else {
      buttonsGame();                         //check for player inputs
      clearLcd();                            //clean the LCDs before drawing
      drawBoard();                           //draw the board onto the lcd's
      intTick = intTick + intGameSpeed;      //add to tick
    }
  } else {
    clearLcd();                   //clean the LCDs before drawing
    title();                      //display title and score info
    buttonsMenu();                //read player input
    clearBoard();                 //ensure the whole board = 0
  }
  delay(10);                      //delay the arduino by a short moment
}

//cleans the lcd, so any unentered cells arn't left there
void clearLcd() {
  for (int i = 0; i <= 15; i++){
    for (int ii = 0; ii <= 1; ii++){
      lcd.setCursor(i, ii);
      lcd.write(" ");
    }
  }
}

//draws the board onto the lcd's
void drawBoard() {
  for (int i = 0; i <= 15; i++){    
    //draw collums 1 and 2 on the left LCD
    //if the tile = 0 write nothing, = 1 write "#", = 2 write "@"
    lcd.setCursor(i, 1);                        //set to the first collumn (bottom row)
    if (arrGame[i][0] == 1) {lcd.write("X");}
    if (arrGame[i][0] == 2) {lcd.write("@");}
    lcdLeft.setCursor(i, 0);                        //set to the second collumn (top row)
    if (arrGame[i][1] == 1) {lcd.write("X");}
    if (arrGame[i][1] == 2) {lcd.write("@");}
  }
  
  /*
  lcd.setCursor(0, 0); // top left
  lcd.setCursor(15, 0); // top right
  lcd.setCursor(0, 1); // bottom left
  lcd.setCursor(15, 1); // bottom right
  */
  
}

//moves the board down and places a random value to be a tile
void playBoard() {
  
  int tileArr = {2, 5, 8, 11}; // the tiles corresponding to each button
  
  for (int i = 0; i < 15; i++){   // clear the top row
    arrGame[i][1] = 0;
  }
  
  int tileIndex = random(0,4);
  arrGame[tileIndex][1] = 1;
  /* for (int i = 0; i < 4; i++){
    int currentIndex = tileArr[i];
    arrGame[currentIndex][1] = random(0,2);   // on the tile points on the top row we each set whether it's a tile or no
  }
  */
  
  // moving the board down
  for (int i = 0; i < 15 ; i++){
    arrGame[i][0] = arrGame[i][1];
  }
  
}

//sets the whole board to 0 and resets variables to pregame
void clearBoard() {
  //reset tick and delay values
  intTick = 0;
  intDelay = 1000;
  //go through the board and set everything to 0
  for (int i = 0; i <= 15; i++){
    for (int ii = 0; ii <= 3; ii++){
      arrGame[i][ii] = 0;
    }
  }
}

// displays the Main Menu on the LCD's
void title() {
  //write title onto LCD and space for score
  lcd.setCursor(0, 0);
  lcd.write("JATs-Piano Tiles");
  lcd.setCursor(0, 1);
  lcd.write("Score: ");
  //convert the score into a string
  char strScore[3];
  sprintf(strScore, "%d", intScore);
  //display score onto LCD
  lcd.write(strScore);
  //add the diffictuly
  lcd.setCursor(10, 1);
  if (intDiff == 0){
    lcdRight.write("Easy");
  }
  if (intDiff == 1){
    lcdRight.write("Medium");
  }
  if (intDiff == 2){
    lcdRight.write("Hard");
  }
  
}

// checks the buttons and what to do for them while out of game
void buttonsMenu(){
  // when enter is pressed start the game and reset score value
  if (intEnter == 1){
    bolPlay = true;
    intScore = 0;
    playBoard();
    drawBoard();
  }
  
  //set game speed to easy difficulty
  if (intInput == 0){
    Serial.print("Game set to easy (");
    Serial.print(intGameSpeedEasy);
    Serial.println("ms acceleration)");
    intDiff = 0;
    intGameSpeed = intGameSpeedEasy;
  }
  
  //set game speed to medium difficulty
  if (intInput == 1){
    Serial.print("Game set to medium (");
    Serial.print(intGameSpeedMedium);
    Serial.println("ms acceleration)");
    intDiff = 1;
    intGameSpeed = intGameSpeedMedium;
  }
  
  //set game speed to hard difficulty
  if (intInput == 2){
    Serial.print("Game set to hard (");
    Serial.print(intGameSpeedHard);
    Serial.println("ms acceleration)");
    intDiff = 2;
    intGameSpeed = intGameSpeedHard;
  }
}

// checks the buttons and what to do for them while in game
void buttonsGame(){
  if (intInput != 4){
    if (bolTilePressed == false){
      bolTilePressed = true;
      
    }
  }
  
  if (intInput != 4) {                              //if a button is pressed
    if (bolTilePressed == false){                   //only if bolTilePressed is false trigger action for checking a button press
      bolTilePressed = true;                        //then set bolTilePressed to true to make sure it isn't acidentilly triggered again
      int intLowestTile = 0;                        //to be set to the tile with the lowest tile
      int intCheckedTile = 15;                      //to keep track of which tiles have been checked
      while (intLowestTile == 0) {                  //as long as it isn't set to anything check tiles
        for (int i = 0; i <= 3; i++){               //check through the row
          if (arrGame[intCheckedTile][i] == 1){     //check to see if a tile is untouched
            intLowestTile = intCheckedTile;         //set lowest tile to equal the checked row
          }
        }
        intCheckedTile--;                           //take one from checkedTile and move onto the next row
        if (intCheckedTile == -1){                  //if it goes off the board trigger game over and set lowest tile to 16 to avoid a forever loop
          Serial.println("No Tile to press");
          gameOver();
          intLowestTile = 16;
        }
      }
      if (intLowestTile != 16){                     //ensure no check is done off the board
        if (arrGame[intLowestTile][intInput] == 1){ //if the tile pressed is an unclicked one
          arrGame[intLowestTile][intInput] = 2;     //set to 2 to say it's been press
          intScore++;                               //add one to score
          if (intDelay > 100){                      //as long as int delay is not lower than 100
            intDelay = intDelay - 20;               //take a value from it
          }
        } else {
          Serial.println("Wrong button pressed");
          gameOver();                               //otherwise game over
        }
      }
    }
  }
}</p><p>void gameOver() {
  Serial.println("Game Over!");
  Serial.print("Your score was: ");
  Serial.println(intScore);
  Serial.print("Your speed was: ");
  Serial.println(intDelay);
  bolPlay = false;
}</p><p>// checks for player input
void input() {
  intEnter = digitalRead(btnEnter);   //read enter
  //read which of the other inputs, or if none set to 4
  if (digitalRead(btn1) == HIGH){intInput = 0;} else {
  if (digitalRead(btn2) == HIGH){intInput = 1;} else {
  if (digitalRead(btn3) == HIGH){intInput = 2;} else {
  if (digitalRead(btn4) == HIGH){intInput = 3;} else {
    intInput = 4;
  }}}}
  //serial print the inputs
  if (intEnter == 1){Serial.println("Enter Pressed! ");}
  if (intInput != 4){
    Serial.print("Button Press: ");
    Serial.println(intInput);
  } else {
    //if no button is pressed reset bolTilePressed
    bolTilePressed = false;
  }
}</p><p>//checks the bottom of the board for failure
void bottomCheck() {
  for (int i = 0; i <= 3; i++){         //for the 4 collumns
    if (arrGame[15][i] == 1){           //if a tile is at the bottom
      Serial.println("Tile at bottom");
      arrGame[15][i] = 2;
      drawBoard();
      delay(400);
      arrGame[15][i] = 1;
      drawBoard();
      delay(400);
      arrGame[15][i] = 2;
      drawBoard();
      delay(400);
      arrGame[15][i] = 1;
      drawBoard();
      delay(400);
      gameOver();
    }
  }
}</p><p>//prints the board into the serial monitor if bolSerialBoard is true
void writeSerial() {
  if (bolSerialBoard == true){
    for (int i = 0; i <= 15; i ++){
      for (int ii = 0; ii <= 3; ii++){
        Serial.print(arrGame[i][ii]);
        Serial.print(",");
      }
      Serial.println("");
    }
  }
}</p>