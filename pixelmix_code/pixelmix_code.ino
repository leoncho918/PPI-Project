  bool gameStarted, gameOver;
  int playerLives, difficulty, score, highScore, playback, numOfButtons;

void setup() {
  const int defaultLives = 3;
  const int defaultDifficulty = 3;
  const int defaultScore = 0;
  const int defaultPlayback = 500;
  
  highScore = 0;
  numOfButtons = 4;
}

void loop() {
  
}

int[] generateSequence(int difficulty) {
  int sequence[difficulty];
  for (int i=0; i<sequence.size(); i++) {
    sequence[i] = random(0, 15);
  }
  return sequence;
}

void resetGame() {
  playerLives = defaultLives;
  difficulty = defaultDifficulty;
  score = defaultScore;
  playback = defaultPlayback;
}
