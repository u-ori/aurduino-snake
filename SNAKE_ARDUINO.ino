#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN 3

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

int snake_head_x = 4;
int snake_head_y = 4;
int snake_dir = 0; // 0 - UP, 1 - RIGHT, 2 - DOWN, 3 - LEFT
int snake_dir_prev = 0;
int snake_pieces[64][2];
int snake_length = 0;
int apple_x = 0;
int apple_y = 0;
int tick_speed = 750;
int seed = 0;
bool boost_enabled = false;

void setup() {
  pinMode(9, INPUT_PULLUP);
  matrix.begin();
  matrix.fillScreen(matrix.Color(0,0,0));
  matrix.drawPixel(0,3,matrix.Color(40,40,40));
  matrix.drawPixel(1,2,matrix.Color(40,40,40));
  matrix.drawPixel(1,3,matrix.Color(40,40,40));
  matrix.drawPixel(2,3,matrix.Color(40,40,40));
  matrix.drawPixel(3,3,matrix.Color(40,40,40));
  
  matrix.drawPixel(4,0,matrix.Color(40,40,40));
  matrix.drawPixel(4,1,matrix.Color(40,40,40));
  matrix.drawPixel(5,1,matrix.Color(40,40,40));
  matrix.drawPixel(4,2,matrix.Color(40,40,40));
  matrix.drawPixel(4,3,matrix.Color(40,40,40));
  
  matrix.drawPixel(7,4,matrix.Color(40,40,40));
  matrix.drawPixel(6,4,matrix.Color(40,40,40));
  matrix.drawPixel(6,5,matrix.Color(40,40,40));
  matrix.drawPixel(5,4,matrix.Color(40,40,40));
  matrix.drawPixel(4,4,matrix.Color(40,40,40));
  
  matrix.drawPixel(3,7,matrix.Color(40,40,40));
  matrix.drawPixel(3,6,matrix.Color(40,40,40));
  matrix.drawPixel(2,6,matrix.Color(40,40,40));
  matrix.drawPixel(3,5,matrix.Color(40,40,40));
  matrix.drawPixel(3,4,matrix.Color(40,40,40));
  matrix.show();
  Serial.begin(9600);
  while (!(analogRead(A0) < 250 || analogRead(A0) > 750 || analogRead(A1) < 250 || analogRead(A1) > 750)) {
    seed++;
  }
  Serial.println("Game Started");
  randomSeed(seed);
  matrix.fillScreen(matrix.Color(0,0,0));
  addApple();
}

int timer = 0;

void loop() {
  if (analogRead(A0) < 250 && snake_dir_prev!=0 && snake_dir != 2) {
    snake_dir = 2;
    Serial.println("down");
  } else if (analogRead(A0) > 750 && snake_dir_prev!=2 && snake_dir != 0) {
    snake_dir = 0;
    Serial.println("up");
  } else if (analogRead(A1) < 250 && snake_dir_prev!=1 && snake_dir != 3) {
    snake_dir = 3;
    Serial.println("left");
  } else if (analogRead(A1) > 750 && snake_dir_prev!=3 && snake_dir != 1) {
    snake_dir = 1;
    Serial.println("right");
  }
  
  if (timer == tick_speed || (digitalRead(9)==LOW && timer >= tick_speed/3)) {
    if (digitalRead(9)==LOW && !boost_enabled) {
      Serial.println("BOOST");
      boost_enabled = true;
    } else if (digitalRead(9) == HIGH && boost_enabled) {
      boost_enabled = false;
      Serial.println("BOOST stopped");
    }
    matrix.drawPixel(snake_head_x, snake_head_y, matrix.Color(0, 50, 0));
    snake_pieces[snake_length][0] = snake_head_x;
    snake_pieces[snake_length][1] = snake_head_y;
    
    if (snake_dir == 0) {
      snake_head_y--;
    } else if (snake_dir == 1) {
      snake_head_x++;
    } else if (snake_dir == 2) {
      snake_head_y++;
    } else {
      snake_head_x--;
    }

    matrix.drawPixel(snake_pieces[0][0], snake_pieces[0][1], matrix.Color(0,0,0));
    if (!(snake_head_x==apple_x && snake_head_y==apple_y)) {
      for (int i=0; i<snake_length+1; i++) {
        snake_pieces[i][0] = snake_pieces[i+1][0];
        snake_pieces[i][1] = snake_pieces[i+1][1];
      }
    }
    else {
      addApple();
      Serial.println("YUUUUUM");
      snake_length++;
      tick_speed = tick_speed-(tick_speed/20);
    }

    if (snake_head_x < 0 || snake_head_x > 7 || snake_head_y < 0 || snake_head_y > 7) {
      endGame();
    }

    for (int i=0;i<8;i++) {
      if (snake_head_x == snake_pieces[i][0] && snake_head_y == snake_pieces[i][1]) {
        endGame();
      }
    }

    snake_dir_prev = snake_dir;
    
    matrix.drawPixel(snake_head_x, snake_head_y, matrix.Color(0, 120, 0));
    matrix.show();
    timer = 0;
  }
  timer++;
  delay(1);
}

void addApple() {
  int possible_apples[64][2];
  for (int x=0;x<8;x++) {
    for (int y=0;y<8;y++) {
      possible_apples[x*8+y][0]=x;
      possible_apples[x*8+y][1]=y;
    }
  }
  for (int i=0;i<64;i++) {
    if (possible_apples[i][0] == snake_head_x && possible_apples[i][1] == snake_head_y) {
      for (int j=0;j<64-i;j++) {
        possible_apples[j+i][0] = possible_apples[j+i+1][0];
        possible_apples[j+i][1] = possible_apples[j+i+1][1];
      }
      break;
    }
  }
  for (int k=0;k<snake_length;k++){
    for (int i=0;i<63-k;i++) {
      if (possible_apples[i][0] == snake_pieces[k][0] && possible_apples[i][1] == snake_pieces[k][1]) {
        for (int j=0;j<63-i-k;j++) {
          possible_apples[j+i][0] = possible_apples[j+i+1][0];
          possible_apples[j+i][1] = possible_apples[j+i+1][1];
        }
        break;
      }
    } 
  }
  int val = random(0,63-snake_length);
  apple_x = possible_apples[val][0];
  apple_y = possible_apples[val][1];
  matrix.drawPixel(apple_x, apple_y, matrix.Color(120,0,0));
}

void endGame() {
  matrix.fillScreen(matrix.Color(120,80,20));
  
  matrix.drawPixel(2,2,matrix.Color(80,0,0));
  matrix.drawPixel(5,2,matrix.Color(80,0,0));
  matrix.drawPixel(2,5,matrix.Color(80,0,0));
  matrix.drawPixel(3,4,matrix.Color(80,0,0));
  matrix.drawPixel(4,4,matrix.Color(80,0,0));
  matrix.drawPixel(5,5,matrix.Color(80,0,0));
  matrix.show();
  Serial.print("score: ");
  Serial.println(snake_length);
  delay(500);
  while (digitalRead(9)==HIGH) {}
  Serial.println("Game Restarted");
  restart();
}

void restart() {
  snake_head_x = 4;
  snake_head_y = 4;
  snake_dir = 0; // 0 - UP, 1 - RIGHT, 2 - DOWN, 3 - LEFT
  snake_dir_prev = 0;
  snake_length = 0;
  apple_x = 0;
  apple_y = 0;
  tick_speed = 750;
  boost_enabled = false;
  matrix.fillScreen(matrix.Color(0,0,0));
  matrix.drawPixel(0,3,matrix.Color(40,40,40));
  matrix.drawPixel(1,2,matrix.Color(40,40,40));
  matrix.drawPixel(1,3,matrix.Color(40,40,40));
  matrix.drawPixel(2,3,matrix.Color(40,40,40));
  matrix.drawPixel(3,3,matrix.Color(40,40,40));
  
  matrix.drawPixel(4,0,matrix.Color(40,40,40));
  matrix.drawPixel(4,1,matrix.Color(40,40,40));
  matrix.drawPixel(5,1,matrix.Color(40,40,40));
  matrix.drawPixel(4,2,matrix.Color(40,40,40));
  matrix.drawPixel(4,3,matrix.Color(40,40,40));
  
  matrix.drawPixel(7,4,matrix.Color(40,40,40));
  matrix.drawPixel(6,4,matrix.Color(40,40,40));
  matrix.drawPixel(6,5,matrix.Color(40,40,40));
  matrix.drawPixel(5,4,matrix.Color(40,40,40));
  matrix.drawPixel(4,4,matrix.Color(40,40,40));
  
  matrix.drawPixel(3,7,matrix.Color(40,40,40));
  matrix.drawPixel(3,6,matrix.Color(40,40,40));
  matrix.drawPixel(2,6,matrix.Color(40,40,40));
  matrix.drawPixel(3,5,matrix.Color(40,40,40));
  matrix.drawPixel(3,4,matrix.Color(40,40,40));
  matrix.show();
  while (!(analogRead(A0) < 250 || analogRead(A0) > 750 || analogRead(A1) < 250 || analogRead(A1) > 750)) {}
  Serial.println("Game Started");
  matrix.fillScreen(matrix.Color(0,0,0));
  addApple();
}

