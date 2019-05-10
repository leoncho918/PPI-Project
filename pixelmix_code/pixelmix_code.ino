  bool gameStarted, gameOver;
  int playerLives, difficulty, score, highScore, playBackRate;
  
void setup() {
  
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
