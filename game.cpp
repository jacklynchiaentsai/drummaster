// include libraries: telling the arduino to use the library "LiquidCrystal.h"
#include <LiquidCrystal.h>

// **potential modification** game speed variables: higher numbers = faster game speed up
int intGameSpeedEasy = 10;
int intGameSpeedMedium = 25;
int intGameSpeedHard = 80; 

// **potential modification** Define pins we are using for our buttons
//format: define [variablename] [portNum]
/* Although we are calling the ports port 15, 16, 17 and 18, they are labelled on the arduino as A1 A2 A3 and A4, 
this is because they are ports used specifically for analog inputs, although we are only using them for digital inputs. */
#define btnEnter A0
#define btn1 15
#define btn2 16
#define btn3 17
#define btn4 18 // create LCD objects (n, ~, n, ~, ~, n)

/* tells the arduino that when we call lcdLeft or lcdRight we are refering to a LiquidCrystal object. 
The numbers in the attached brackets tell the arduino which ports the object should be using to send messages to the LCD when we use their functions. */
LiquidCrystal lcdLeft(8, 9, 12, 10, 11, 13);
LiquidCrystal lcdRight(2, 3, 4, 5, 6, 7);

// set up game array ** potential modification to correspond to tempo of game **
int arrGame[16][4] = {
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0}
};

//set up variables for the game
boolean bolPlay;            // tells the program if the menu should be displayed or if the actual game should be running 
int intScore;               // tracks the player's score
int intDiff;                // telling what difficulty the game is on: used on the main menu to tell the program what bit of text to print on the LCD's

//set up variables for input
int intEnter;               //trackes if the user presses the enter button(furthest left?) ** potential modification** - designating the enter button
int intInput;               //tracks which buttons the user presses
boolean bolTilePressed;     //ensure the player doesn't accidently press a button 5x and lose: make sure the program only reads on when the button is pressed and not when it is held

//set up variables for turn: stop the program from moving the board every time it loops
int intTick;                //counts up millies (per loop) until intDelay
int intDelay;               //the time the program waits till the next turn in millis
int intGameSpeed; //abit of debug options
boolean bolSerialBoard;     //when true will print the board in the serial monitor

//the setup that will be run once: setting default values
void setup() {
  Serial.begin(9600);       //start the serial monitor
  //set up the variables
  bolPlay = false;    // the game doesn't start playing immediately
  intScore = 0;
  intTick = 0;
  intDelay = 1000;
  intDiff = 1;
  intGameSpeed = intGameSpeedMedium;
  
  //begin lcd's
  lcdLeft.begin(16,2);
  lcdRight.begin(16,2);
}

//the loop which will be run every 10 millisecons
void loop() {
  input();                        //check for playing input: reads which buttons the user has pressed
  if (bolPlay == true){     
    if (intTick >= intDelay){     //check to see if the game should play a turn or continue to wait
      Serial.println("~~~~~~~~~"); //print to signify the board moving on
      //writeSerial();              //if option is enabled write the board into serial
      buttonsGame();              //check for player inputs: controls what the buttons do when in the game, and not in the menu
      playBoard();                //adds a new tile to the board, and then moves everything in the board down one space
      clearLcd();                 //clean the LCDs before drawing
      drawBoard();                //draw the board onto the lcd's
      bottomCheck();              // checks the bottom of arrGame for a failure condition
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
    buttonsMenu();                // controls what the user's inputs do when in the menu
    clearBoard();                 //ensure the whole board = 0
  }
  delay(10);                      //delay the arduino by a short moment
}

//cleans the lcd, so any unentered cells arn't left there
// Without being reset to nothing the LCD's will maintain whatever was written previously
void clearLcd() {
  for (int i = 0; i <= 15; i++){
    for (int ii = 0; ii <= 1; ii++){
      lcdLeft.setCursor(i, ii);
      lcdLeft.write(" ");
      lcdRight.setCursor(i, ii);
      lcdRight.write(" ");
    }
  }
}

//draws the board onto the lcd's
// # for a tile yet to be hit, or an @ for a hit tile.
void drawBoard() {
  for (int i = 1; i <= 15; i++){
    //draw collums 1 and 2 on the left LCD
    //if the tile = 0 write nothing, = 1 write "#", = 2 write "@"
    lcdLeft.setCursor(i, 1);                        //set to the first collumn (furthest left)
    if (arrGame[i][0] == 1) {lcdLeft.write("#");}
    if (arrGame[i][0] == 2) {lcdLeft.write("@");}
    lcdLeft.setCursor(i, 0);                        //set to the second collumn (centre left)
    if (arrGame[i][1] == 1) {lcdLeft.write("#");}
    if (arrGame[i][1] == 2) {lcdLeft.write("@");}
    lcdRight.setCursor(i, 1);                       //set to the third collumn (centre right)
    if (arrGame[i][2] == 1) {lcdRight.write("#");}
    if (arrGame[i][2] == 2) {lcdRight.write("@");}
    lcdRight.setCursor(i, 0);                       //set to the fourth collumn (furthest right)
    if (arrGame[i][3] == 1) {lcdRight.write("#");}
    if (arrGame[i][3] == 2) {lcdRight.write("@");}
  }
}

//moves the board down and places a random value to be a tile -> ** potential modification we don't want it to be random ** 
void playBoard() {
  for (int i = 0; i <= 3; i++){arrGame[0][i] = 0;}      //clear the top row
  arrGame[0][random(0, 4)] = 1;                         //set a random point on the top row to be a tile
  for (int i = 15; i >= 1; i--){                        //working from the bottom of the board to the top
    for (int ii = 0; ii <= 3; ii ++){                   //for each collum
      arrGame[i][ii] = arrGame[i - 1][ii];              // setting the row to equal whatever the row above it is equal to, making the board move down the LCD's
    }
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
  lcdRight.setCursor(0, 0);
  lcdRight.write("Piano Tiles"); //name of game
  lcdRight.setCursor(0, 1);
  lcdRight.write("Score: ");
  //convert the score into a string
  char strScore[3];
  sprintf(strScore, "%d", intScore);
  //display score onto LCD
  lcdRight.write(strScore);
  //add the diffictuly
  lcdRight.setCursor(10, 1);
  if (intDiff == 0){
    lcdRight.write("Easy");
  }
  if (intDiff == 1){
    lcdRight.write("Medium");
  }
  if (intDiff == 2){
    lcdRight.write("Hard");
  }
  
  //Press a little bit of instruction ** potential modification: we might be able to skip the enter **
  lcdLeft.setCursor(0, 0);
  lcdLeft.write("Press Enter");
  lcdLeft.setCursor(0, 1);
  lcdLeft.write("to begin!");
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
  /* The program then reads what intInput is equal to. 
  if it is equal to 0 the first button from the left is being pressed, going up to the right up to 3. If intInput is equal to 4 no button is being pressed. */
  // when button 3 is pressed turn on debug option of printing the board in serial -> do we need this???
  /* if button 3 is pressed the game will activate or deactivate a debug mode 
  where the whole board is printed in the serial monitor to assist in finding issues in the program. */
  if (intInput == 3){
    if (bolSerialBoard == false){
      Serial.println("Serial Board Active");
      bolSerialBoard = true;
    } else {
      Serial.println("Serial Board Disabled");
      bolSerialBoard = false;
    }
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
        intCheckedTile--;                           //take one from checkedTile and move onto the next row -> consider expanding the number of rows!!!
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
}

void gameOver() {
  Serial.println("Game Over!");
  Serial.print("Your score was: ");
  Serial.println(intScore);
  Serial.print("Your speed was: ");
  Serial.println(intDelay);
  bolPlay = false;
}

// checks for player input
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
}

/* using a loop this code checks the bottom row of arrGame for any unhit tiles (tiles equal to 1),
if there is an unhit tile at the bottom of the screen it will flash the tile and then trigger the game over function. */
//checks the bottom of the board for failure
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
}

//prints the board into the serial monitor if bolSerialBoard is true
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
}
