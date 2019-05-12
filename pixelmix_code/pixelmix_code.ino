  #include <Adafruit_NeoPixel.h>

  #define PIXEL_COUNT = 60;
  
  bool gameInProgress; //Keep track of game events
  int playerLives, difficulty, score, highScore, playback; //Keep track of player values and highscore;

 
  
  const int numOfButtons = 3; //Change value depending on amount of buttons in system
  const int defaultLives = 3; //Default value for player's lives
  const int defaultDifficulty = 3; //Default value of starting difficulty
  const int defaultScore = 0; //Default value of starting score
  const int defaultPlayback = 500; //Default value for speed of button lights flash
  const int cMin = 120; //Minimum colour value
  const int cMax = 255; //Maximum colour value

  int sequence[numOfButtons]; //Store sequence of buttons to be played
  Adafruit_NeoPixel led[numOfButtons];
  

void setup() {
  resetGame(); //Set all values to default values when starting game
  setupLeds(); //Assign variable to all LEDs
  highScore = 0; //Set initial highscore to 0
  gameInProgress = true;
  Serial.begin(9600);
}

void loop() {
  if (gameInProgress) {
    while(gameInProgress) {
      generateSequence();
      showSequence();
      delay(2000);
    }
  }
}

void generateSequence() { //Function to generate a random sequence of buttons to flash
  for (int i=0; i<difficulty; i++) { //Run for loop difficulty times (if difficulty = 3, run 3 times).
    sequence[i] = random(numOfButtons); //Set value to random number in array;
  }
}

uint32_t randomColour(int num) {
  int r = random(255);
  int g = random(255);
  int b = random(255);
  while (r > 120 && g > 120 && b > 120) {
    switch(random(0, 2)){
      case 0:
        r = random(255);
      break;
      case 1:
        g = random(255);
      break;
      case 2:
        b = random(255);
      break;
    }
  }
  return led[num].Color(r, g, b);
}

void showSequence() {
  for (int i=0; i<difficulty; i++) {
    led[sequence[i]].setPixelColor(0, randomColour(sequence[i]));
    led[sequence[i]].show();
    delay(playback);
    led[sequence[i]].setPixelColor(0, 0, 0, 0);
    led[sequence[i]].show();
  }
}

void resetGame() {
  //Set values to default values
  playerLives = defaultLives;
  difficulty = defaultDifficulty;
  score = defaultScore;
  playback = defaultPlayback;

  for (int i=0; i<numOfButtons; i++) { //For loop to set all values in array to -1
    sequence[i] = -1;
  }
}

void setupLeds() {
  int bPin = 2;
  int lPin = 3;
  for (int i=0; i<numOfButtons; i++) {
    led[i] = Adafruit_NeoPixel(bPin, lPin, NEO_GRB + NEO_KHZ800);
    pinMode(bPin, INPUT_PULLUP);
    led[i].begin();
    led[i].clear();
    led[i].show();
    bPin+=2;
    lPin+=2;
  }
}
