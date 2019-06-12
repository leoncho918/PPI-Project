#include <rgb_lcd.h>

  #include "Adafruit_NeoTrellis.h"
  #include <Wire.h>
  #include "rgb_lcd.h"
  #include <EEPROM.h>
  #include "notes.h"

  //Array to get heart custom character
  byte heart[8] = {
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000
  };

  
  bool gameInProgress, waitingInput, resettingGame; //Keep track of game events
  int playerLives, difficulty, score, highScore, playback, gameLevel, gameRound; //Keep track of player values and highscore;
  int checkedButton;
  
  const int numOfButtons = 16; //Change value depending on amount of buttons in system
  const int defaultLives = 3; //Default value for player's lives
  const int defaultDifficulty = 3; //Default value of starting difficulty
  const int defaultScore = 0; //Default value of starting score
  const int defaultPlayback = 750; //Default value for speed of button lights flash
  const int cMin = 120; //Minimum colour value
  const int cMax = 255; //Maximum colour value
  const int eeAddress = 0;
  const int defaultGameLevel = 10;
  const int defaultGameRound = 1;
  const int buzzerPin = 5;
  const int ledPin = 6; 
  const int buttonPin = 7;

  int sequence[numOfButtons]; //Store sequence of buttons to be played
  int prevButtonState = HIGH;
  int buttonState;
  unsigned long lastDebounceTime = 0;
  unsigned long debounceDelay = 0;
  Adafruit_NeoTrellis trellis;
  rgb_lcd lcd;

  void(* resetFunc) (void) = 0;


//define a callback for key presses
TrellisCallback blink(keyEvent evt){
  // Check is the pad pressed?
  if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
    Serial.println("Button Pressed");
    //Only flash the buttons when the we are waiting for the user to press buttons or the game hasn't started yet
    if(waitingInput||!gameInProgress) {
      trellis.pixels.setPixelColor(evt.bit.NUM, trellis.pixels.Color(255, 255, 255)); //on rising
    }
  } else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) {
  // or is the pad released?
    trellis.pixels.setPixelColor(evt.bit.NUM, trellis.pixels.Color(0, 0, 0)); //off falling
    if(!gameInProgress) {
      gameInProgress = true;
    }
    if(waitingInput) {
      if(correctInput(evt)) {
        Serial.println("Correct button");
        if(checkedButton < 2)
          buzz(buzzerPin, NOTE_A5, 250);
        score+=100;
        checkedButton++;
      }
      else {
        Serial.println("Incorrect Button, Showing Sequence Again");
        playerLives--;
        checkedButton = 0;
        
        if(playerLives <= 0) {
          lcd.clear();
          lcd.setCursor(5, 0);
          lcd.print("Gameover");
          buzz(buzzerPin, NOTE_F3, 250);
          delay(50);
          buzz(buzzerPin, NOTE_D2, 250);
          delay(50);
          buzz(buzzerPin, NOTE_B1, 250);
        }
        else {
          buzz(buzzerPin, NOTE_A2, 150);
          delay(50);
          buzz(buzzerPin, NOTE_A1, 150);
        }
        showSequence();
      }
    }
    updateLCD();
    Serial.println("Button Released");
  }

  // Turn on/off the neopixels!
  trellis.pixels.show();
  delay(50);
  return 0;
}

void setup() {
  Serial.begin(9600);

  Serial.println("Setting up devices");
  setupLCD();

  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  
  EEPROM.get(eeAddress, highScore);
  if(highScore < 0)
    highScore = 0;
  Serial.print("Highscore: ");
  Serial.println(highScore);
  
  resetGame(); //Set all values to default values when starting game
  
  setupTrellis();
  
  gameInProgress = false;
  waitingInput = false;
}

void setupLCD() {
  lcd.begin(16, 2);

  lcd.createChar(0, heart);
}

void setupTrellis() {
  trellis.begin();

  //activate all keys and set callbacks
  for(int i=0; i<NEO_TRELLIS_NUM_KEYS; i++){
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_RISING);
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING);
    trellis.registerCallback(i, blink);
  }

  for (int i=0; i<numOfButtons; i++) {
    trellis.pixels.setPixelColor(i, Wheel(map(i, 0, trellis.pixels.numPixels(), 0, 255)));
    trellis.pixels.show();
    delay(50);
  }
  for (int i=0; i<numOfButtons; i++) {
    trellis.pixels.setPixelColor(i, 0x000000);
    trellis.pixels.show();
    delay(50);
  }
}

void loop() {
  if (!gameInProgress) {
    trellis.read();
  }
  if (gameInProgress) {
    activateButtons(false);
    updateDifficulty();
    updateLCD();
    generateSequence();
    showSequence();
    
    waitingInput = true;
    checkedButton = 0;
    int prevLife = playerLives;
    activateButtons(true);
    while(checkedButton<difficulty && playerLives > 0) {
      trellis.read();
      checkReset();
    }
    activateButtons(false);
    waitingInput = false;
    if (playerLives <= 0 && !resettingGame) {
      gameInProgress = false;
      waitingInput = false;
      gameOver();
    }
    else if (!resettingGame){
      winRound();
    }
    else {
      resetGame();
    }
  }
}

void checkReset() {
  digitalWrite(ledPin, HIGH);
  int reading = digitalRead(buttonPin);
  if (reading == LOW) {
    digitalWrite(ledPin, LOW);
    resetFunc();
  }
}

void winRound() {
  score+=500;

  gameRound++;

  if (gameRound > 5) {
    gameLevel++;
    gameRound = 1;
  }

  updateDifficulty();

  buzz(buzzerPin, NOTE_A5, 250);
  delay(25);
  buzz(buzzerPin, NOTE_A6, 250);
  
  updateLCD();
  for (int i=0; i<numOfButtons; i++) {
  trellis.pixels.setPixelColor(i, Wheel(map(i, 0, trellis.pixels.numPixels(), 0, 255)));
  trellis.pixels.show();
  delay(50);
  }
  for (int i=0; i<numOfButtons; i++) {
    trellis.pixels.setPixelColor(i, 0x000000);
    trellis.pixels.show();
    delay(50);
  }
}

void activateButtons(bool isActivated) {
  for(int i=0;i<numOfButtons;i++) {
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_RISING, isActivated);
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING, isActivated);
  }
}

void updateDifficulty() {
  switch(gameLevel) {
      case 1:
        playback = 750;
        break;
      case 2:
        difficulty = 4;
        break;
      case 3:
        playback = 650;
        break;
      case 4:
        difficulty = 5;
        break;
      case 5:
        playback = 550;
        break;
      case 6:
        difficulty = 6;
        break;
      case 7:
        playback = 450;
        break;
      case 8:
        difficulty = 7;
        playback = 350;
        break;
      case 9:
        playback = 250;
        break;
      case 10:
        playback = 250;
        difficulty = 8;
        break;
      default:
        difficulty++;
        break;
    }
}

void gameOver() {
  digitalWrite(ledPin, LOW);
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("Gameover");
  delay(5000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("HS:");
  lcd.print(highScore);
  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(score);
  delay(2000);
  if (score > highScore) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("NEW HIGHSCORE!");
    highScore = score;
    EEPROM.put(eeAddress, highScore);
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("HS:");
    lcd.print(highScore);
    lcd.setCursor(0, 1);
    lcd.print("S:");
    lcd.print(score);
  }
  delay(5000);
  resetGame();
}

void generateSequence() { //Function to generate a random sequence of buttons to flash
  for (int i=0; i<difficulty; i++) { //Run for loop difficulty times (if difficulty = 3, run 3 times).
    sequence[i] = random(numOfButtons); //Set value to random number in array;
  }
}

// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return trellis.pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return trellis.pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return trellis.pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  return 0;
}

void showSequence() {
  activateButtons(false);
  //For loop to go over all buttons in the sequence
  for (int i=0; i<difficulty; i++) {
    checkReset();
    trellis.pixels.setPixelColor(sequence[i], trellis.pixels.Color(255, 255, 255));
    trellis.pixels.show();
    //Pause for so the user can see the led
    delay(playback);
    //Turn off the led
    trellis.pixels.setPixelColor(sequence[i], trellis.pixels.Color(0, 0, 0));
    trellis.pixels.show();
    //Pause incase the same button is in the sequence twice
    delay(250);
  }
  activateButtons(true);
}

bool correctInput(keyEvent evt) {
    if(sequence[checkedButton] == evt.bit.NUM) {
    return true;
  }
  else {
    for(int i=0;i<numOfButtons;i++) {
      trellis.pixels.setPixelColor(i, trellis.pixels.Color(255, 0, 0));
    }
    trellis.pixels.show();
    delay(500);
    for(int i=0;i<numOfButtons;i++) {
      trellis.pixels.setPixelColor(i, trellis.pixels.Color(0, 0, 0));
    }
    return false;
  }
}

void updateLCD() {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("H:");
  lcd.print(highScore);
  lcd.setCursor(9, 0);
  
  if(gameLevel < 10) {
    lcd.print("L:");
    lcd.print(gameLevel);
    lcd.setCursor(13, 0);
    lcd.print("R:");
    lcd.print(gameRound);
  }
  else {
    lcd.print("L:9000+");
  }
  
  lcd.setCursor(0, 1);
  lcd.print("L:");
  for(int i=0;i<playerLives;i++) {
    lcd.write((unsigned char)0);
  }

  lcd.setCursor(6, 1);
  lcd.print("S:");
  lcd.print(score);
}

//Used then the when reset button pressed or when game first started
void resetGame() {
  //Set values to default values
  playerLives = defaultLives;
  difficulty = defaultDifficulty;
  score = defaultScore;
  playback = defaultPlayback;
  gameLevel = defaultGameLevel;
  gameRound = defaultGameRound;
  gameInProgress = false;
  waitingInput = false;
  resettingGame = false;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press any button");
  lcd.setCursor(0, 1);
  lcd.print("to start.");
  
  for (int i=0; i<numOfButtons; i++) { //For loop to set all values in array to -1
    sequence[i] = -1;
    trellis.pixels.setPixelColor(i, trellis.pixels.Color(0, 0, 0));
  }

  trellis.pixels.show();
}

void buzz(int targetPin, long frequency, long length) {
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }
}
