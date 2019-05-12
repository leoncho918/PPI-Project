  bool gameStarted, gameOver; //Keep track of game events
  int playerLives, difficulty, score, highScore, playback; //Keep track of player values and highscore;

 
  
  const int numOfButtons = 4; //Change value depending on amount of buttons in system
  const int defaultLives = 3; //Default value for player's lives
  const int defaultDifficulty = 3; //Default value of starting difficulty
  const int defaultScore = 0; //Default value of starting score
  const int defaultPlayback = 500; //Default value for speed of button lights flash

  int sequence[numOfButtons]; //Store sequence of buttons to be played
  

void setup() {
  resetGame(); //Set all values to default values when starting game
  
  highScore = 0; //Set initial highscore to 0
}

void loop() {
  
}

void generateSequence(int difficulty) { //Function to generate a random sequence of buttons to flash
  for (int i=0; i<difficulty; i++) { //Run for loop difficulty times (if difficulty = 3, run 3 times).
    sequence[i] = random(0, numOfButtons-1); //Set value to random number in array;
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
