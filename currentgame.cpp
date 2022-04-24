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
  bolTilePressed = false;
  if (! ss.digitalRead(SWITCH1)) {
    intInput = 0;
    bolTilePressed = true;
    Serial.println("Switch 1 pressed");
    ss.analogWrite(PWM1, incr);
    incr += 5;
  } else {
    ss.analogWrite(PWM1, 0);
  }
  
  if (! ss.digitalRead(SWITCH2)) {
    intInput = 1;
    bolTilePressed = true;
    Serial.println("Switch 2 pressed");
    ss.analogWrite(PWM2, incr);
    incr += 5;
  } else {
    ss.analogWrite(PWM2, 0);
  }
  
  if (! ss.digitalRead(SWITCH3)) {
    intInput = 2;
    bolTilePressed = true;
    Serial.println("Switch 3 pressed");
    ss.analogWrite(PWM3, incr);
    incr += 5;
  } else {
    ss.analogWrite(PWM3, 0);
  }
  
  if (! ss.digitalRead(SWITCH4)) {
    intEnter = 1;
    bolTilePressed = true;
    Serial.println("Switch 4 pressed");
    ss.analogWrite(PWM4, incr);
    incr += 5;
    return;
    
  } else {
    ss.analogWrite(PWM4, 0);
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
    lcd.write("Easy  ");
  }
  if (intDiff == 1){
    lcd.write("Medium");
  }
  if (intDiff == 2){
    lcd.write("Hard  ");
  }
  
}

void buttonsMenu(){
  // when enter is pressed start the game and reset score value
  if (intEnter == 1){
    bolPlay = true;
    intScore = 0;
    // playBoard();
    // drawBoard();
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
  //go through the board and set everything to 0
  for (int i = 0; i <= 15; i++){
    for (int ii = 0; ii <= 1; ii++){
      arrGame[i][ii] = 0;
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
