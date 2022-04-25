#include <LiquidCrystal.h>

#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>

#define  DEFAULT_I2C_ADDR 0x3A

#define  SWITCH1  18  // PA01
#define  SWITCH2  19 // PA02
#define  SWITCH3  20 // PA03
#define  SWITCH4  2 // PA06
#define  PWM1  12  // PC00
#define  PWM2  13 // PC01
#define  PWM3  0 // PA04
#define  PWM4  1 // PA05

Adafruit_seesaw ss;

int intGameSpeedEasy = 10;
int intGameSpeedMedium = 25;
int intGameSpeedHard = 80;

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

int arrGame[16][2] = {   // 2 rows 16 columns on one lcd
  {0, 0},
  {0, 0},
  {3, 3},
  {0, 0},
  {3, 3},
  {0, 0},
  {3, 3},
  {0, 0},
  {3, 3},
  {0, 0},
  {3, 3},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0}
};

int tileIndex[4] = {3,5,7,9};

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
boolean bolSerialBoard;     //when true will print the board in the serial monitor 

uint8_t incr = 0;

//the setup that will be run once
void setup() {
  seesawsetup();
  
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

void loop(){
  input();
  if (bolPlay == true){
    if (intTick >= intDelay){     //check to see if the game should play a turn or continue to wait
      buttonsGame();              //check for player inputs
      playBoard();                //move the board and add a new tile
      clearLcd();                 //clean the LCDs before drawing
      drawBoard();                //draw the board onto the lcd's
      intTick = 0;
    } else {
      buttonsGame();                         //check for player inputs
      clearLcd();                            //clean the LCDs before drawing
      drawBoard();                           //draw the board onto the lcd's
      intTick = intTick + intGameSpeed;      //add to tick
    }
  } else{
    clearLcd();                   //clean the LCDs before drawing
    title(); 
    buttonsMenu();
    clearBoard();                 //ensure the whole board = 0
  }
  delay(10);                      //delay the arduino by a short moment
}

void seesawsetup(){
  Serial.begin(115200);
  
  while (!Serial) delay(10);   // wait until serial port is opened

  Serial.println(F("Adafruit PID 5296 I2C QT 4x LED Arcade Buttons test!"));
  
  if (!ss.begin(DEFAULT_I2C_ADDR)) {
    Serial.println(F("seesaw not found!"));
    while(1) delay(10);
  }

  uint16_t pid;
  uint8_t year, mon, day;
  
  ss.getProdDatecode(&pid, &year, &mon, &day);
  Serial.print("seesaw found PID: ");
  Serial.print(pid);
  Serial.print(" datecode: ");
  Serial.print(2000+year); Serial.print("/"); 
  Serial.print(mon); Serial.print("/"); 
  Serial.println(day);

  if (pid != 5296) {
    Serial.println(F("Wrong seesaw PID"));
    while (1) delay(10);
  }

  Serial.println(F("seesaw started OK!"));
  ss.pinMode(SWITCH1, INPUT_PULLUP);
  ss.pinMode(SWITCH2, INPUT_PULLUP);
  ss.pinMode(SWITCH3, INPUT_PULLUP);
  ss.pinMode(SWITCH4, INPUT_PULLUP);
  ss.analogWrite(PWM1, 127);
  ss.analogWrite(PWM2, 127);
  ss.analogWrite(PWM3, 127);
  ss.analogWrite(PWM4, 127);
}

//checks for player input
void input() {
  boolean triggerintInput4 = true;
  bolTilePressed = false;
  if (! ss.digitalRead(SWITCH1)) {
    intInput = 0;
    bolTilePressed = true;
    triggerintInput4 = false;
    Serial.println("Switch 1 pressed");
    ss.analogWrite(PWM1, incr);
    incr += 5;
  } else {
    ss.analogWrite(PWM1, 0);
  }
  
  if (! ss.digitalRead(SWITCH2)) {
    intInput = 1;
    bolTilePressed = true;
    triggerintInput4 = false;
    Serial.println("Switch 2 pressed");
    ss.analogWrite(PWM2, incr);
    incr += 5;
  } else {
    ss.analogWrite(PWM2, 0);
  }
  
  if (! ss.digitalRead(SWITCH3)) {
    intInput = 2;
    bolTilePressed = true;
    triggerintInput4 = false;
    Serial.println("Switch 3 pressed");
    ss.analogWrite(PWM3, incr);
    incr += 5;
  } else {
    ss.analogWrite(PWM3, 0);
  }
  
  if (! ss.digitalRead(SWITCH4)) {
    intInput = 3;
    bolTilePressed = true;
    triggerintInput4 = false;
    Serial.println("Switch 4 pressed");
    ss.analogWrite(PWM4, incr);
    incr += 5;
    // return;   
  } else {
    ss.analogWrite(PWM4, 0);
  }
  
  if (triggerintInput4 == true){
    intInput = 4;
  }
  delay(10);
}

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
    lcd.write("Easy");
  }
  if (intDiff == 1){
    lcd.write("Medium");
  }
  if (intDiff == 2){
    lcd.write("Hard");
  }
  
}

void buttonsMenu(){
  // when enter is pressed start the game and reset score value
  if (intInput == 3){
    bolPlay = true;
    intScore = 0;
    clearLcd(); // note this is additional!
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

//sets the whole board to 0 and resets variables to pregame
void clearBoard() {
  //reset tick and delay values
  intTick = 0;
  intDelay = 1000;
  //go through the board and set everything pregame arrGame
  for (int i = 0; i <= 15; i++){
    for (int ii = 0; ii <= 1; ii++){
      if (i == 2 || i == 4 || i == 6 || i == 8 || i == 10){
        arrGame[i][ii] = 3;
      } else{
        arrGame[i][ii] = 0;
      }
    }
  }
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

void playBoard() {
  
  for (int i = 0 ; i <= 15 ; i++){   //clear the top row
    if (i == 2 || i == 4 || i == 6 || i == 8 || i == 10){
        arrGame[i][0] = 3;
    } else{
        arrGame[i][0] = 0;
    }
  }
  
  int selectedTile = tileIndex[random(0,4)];
  arrGame[selectedTile][0] = 1;     //set a random point on the top row to be a tile
  
  for (int i = 0; i <= 15; i++){
    arrGame[i][1] = arrGame[i][0];    // setting the row to equal whatever the row above it is equal to, making the board move down the LCD's
  }
  
}

void drawBoard() {
  for (int i = 0; i <= 15; i++){
    //draw collums 1 and 2 on the left LCD
    //if the tile = 0 write nothing, = 1 write "X", = 2 write "@"
    lcd.setCursor(i, 0);                        //set to the top row
    if (arrGame[i][0] == 1) {lcd.write("X");}
    if (arrGame[i][0] == 2) {lcd.write("@");}
    if (arrGame[i][0] == 3) {lcd.write("|");}
    lcd.setCursor(i, 1);                        //set to the second collumn (centre left)
    if (arrGame[i][1] == 1) {lcd.write("X");}
    if (arrGame[i][1] == 2) {lcd.write("@");}
    if (arrGame[i][1] == 3) {lcd.write("|");}
  }
}

void buttonsGame(){
  if (intInput != 4){   //if a button is pressed
    if (bolTilePressed == false){                   //only if bolTilePressed is false trigger action for checking a button press
      bolTilePressed = true;                        //then set bolTilePressed to true to make sure it isn't acidentilly triggered again
      int inputToTile = 3 - intInput;   //setting the button input that corresponds to its tileIndex
      inputToTile = abs(inputToTile);
      if (arrGame[tileIndex[inputToTile]][1] == 1){ //if the tile pressed is an unclicked one
          arrGame[tileIndex[inputToTile]][1] = 2;     //set to 2 to say it's been press
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

void gameOver() {
  clearLcd();
  lcd.setCursor(0, 0);
  lcd.write("Game Over-");
  lcd.setCursor(10, 0);
  if (intDiff == 0){
    lcd.write("Easy");
  }
  if (intDiff == 1){
    lcd.write("Medium");
  }
  if (intDiff == 2){
    lcd.write("Hard");
  }
  
  lcd.setCursor(0, 1);
  lcd.write("Score: ");
  //convert the score into a string
  char strScore[3];
  sprintf(strScore, "%d", intScore);
  //display score onto LCD
  lcd.write(strScore);
 
  Serial.print("Your speed was: ");
  Serial.println(intDelay);
  bolPlay = false;
}
