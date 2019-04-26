#include <Servo.h>
#include <Wire.h>
#include "rgb_lcd.h"

Servo Pan, Tilt;
rgb_lcd LCD; //connect to the 12C port

long double blink_eye_count = 0;

const int LCD_red = 0;
const int LCD_green = 0;
const int LCD_blue = 255;
const int BUZZER = 8;

int x_coordinate_face;
int y_coordinate_face;

int LeftLED = 12;
int RightLED = 13;

const int x_center = 250;
const int y_center = 250;

int previous_x = 100;
int previous_y = 100;

int error = 12;

int updated_x_angle;
int updated_y_angle;

int count_x = 100;
int count_y = 100;

int motor_speed_x = 1;
int motor_speed_y = 1;

int emotion_index;


void setup() {
  Serial.begin(9600);
  pinMode(BUZZER, OUTPUT);
  Pan.attach(2);
  Tilt.attach(3);
  Pan.write(previous_x);
  Tilt.write(previous_y);
  LCD.begin(15, 1); //set number of col & row
  LCD.setRGB(LCD_red, LCD_green, LCD_blue);
  LCD.print("hello!");
  pinMode(LeftLED,OUTPUT);
  pinMode(RightLED,OUTPUT);
  digitalWrite(LeftLED,HIGH);
  digitalWrite(RightLED,HIGH);
}
  /* play tone */
void playTone(int tone, int duration) {
  bool one_time = false;
  while(one_time == false){
    for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(BUZZER, HIGH);
    delayMicroseconds(tone);
    digitalWrite(BUZZER, LOW);
    delayMicroseconds(tone);
  }
  one_time = true;
  }
  
}
void Original_text(){
  LCD.clear();
  LCD.setRGB(0, 0, 200);
  LCD.setCursor(0, 0);
  LCD.print("I am Cutiepie!");
}

void LCD_sad(){
  // set the cursor to column 0, first row
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.setRGB(160, 0, 0);
  LCD.print("Be happy! Smile!");
  delay(3000);
  Original_text();
}


void LCD_happy(){
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.setRGB(0, 200, 0);
  LCD.print("Happy! Yea!");
  delay(3000);
  Original_text();
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  //int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
  int tones[] = { 130, 138, 164, 174, 185, 207, 246, 270 }; 
  
  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
} 

void Emotion_action(){
  // x = 0, neutral, x = 1, sad, x = 2, happy
  int length_sad = 4;
  char notes_sad[] = "CCC' '";
  int beats_sad[] = {1,2,1,1};
  int tempo_sad = 300;

  int length_happy = 10;
  char notes_happy[] = "Cfdcgegag' ' ";
  int beats_happy[] = { 2, 1, 1, 2, 2,2,1,1,2,3};
  int tempo_happy = 100;
  if(emotion_index != 0){
      for(int i=0;i<3;i++){
      digitalWrite(LeftLED,LOW);
      digitalWrite(RightLED,LOW);
      delay(100);
      digitalWrite(LeftLED,HIGH);
      digitalWrite(RightLED,HIGH);
      delay(100);
    }
  }

  if(emotion_index == 1){
        for(int i = 0; i < length_sad; i++) {
        if(notes_sad[i] == ' ') {
            delay(beats_sad[i] * tempo_sad);
        } else {
            playNote(notes_sad[i], beats_sad[i] * tempo_sad);
        }
        delay(tempo_sad / 2);    /* delay between notes */
    }
    LCD_sad();
  }
  else if(emotion_index == 2){
    for(int i = 0; i < length_happy; i++) {
        if(notes_happy[i] == ' ') {
            delay(beats_happy[i] * tempo_happy);
        } else {
            playNote(notes_happy[i], beats_happy[i] * tempo_happy);
        }
        delay(tempo_happy / 2);    /* delay between notes */
    }
    LCD_happy();
  }
  
}

void Pos(){
  //the function used: map(value, fromLow, fromHigh, toLow, toHigh)
  /*
  if(x_coordinate_face == 0 && y_coordinate_face == 0){
    count_x = count_x;
    Pan.write(count_x);
    count_y = count_y;
    Tilt.write(count_y);
  }
  //left side
  if (x_coordinate_face < 240){
   count_x += 1;
   if(count_x > 140){
    count_x = 140;
   }
   Pan.write(count_x);
  }
  //right side
  else if (x_coordinate_face > 260){
    count_x -= 1;
    if(count_x < 40){
      count_x = 40;
    }
   Pan.write(count_x);
  }
  else if(x_coordinate_face > 240 && x_coordinate_face < 260){
    count_x = count_x;
    Pan.write(count_x);
  }
  //upper
  if(y_coordinate_face < 240){
    count_y -= 1;
    if(count_y < 40){
      count_y = 40;
    }
   Tilt.write(count_y);
  }
  //lower
  else if(y_coordinate_face > 260){
    count_y += 1;
    if(count_y > 120){
      count_y = 120;
    }
   Pan.write(count_y);
  }
    else if(y_coordinate_face > 240 && y_coordinate_face < 260){
    count_y = count_y;
    Tilt.write(count_y);
  }
  */
  if (x_coordinate_face == 0 && y_coordinate_face == 0){
    if (count_x > 100){
      count_x -= motor_speed_x;
    }
    else if(count_x < 100){
      count_x += motor_speed_x;
    }
    else if(count_x == 100){
      count_x = count_x;
    }
    Pan.write(count_x);
    if(count_y > 120){
      count_y -= motor_speed_y;
    }
    else if(count_y < 120){
      count_y += motor_speed_y;
    }
    else if(count_y == 120){
      count_y = count_y;
    }

    Tilt.write(count_y);
  }
  if (y_coordinate_face < (y_center + error)){
    if(count_y >= 5){
      count_y -= motor_speed_y;
      previous_y = count_y;
      Tilt.write(count_y);
    }
  }
  else if (y_coordinate_face > (y_center - error)){
    if (count_y <= 175){
      count_y += motor_speed_y;
      previous_y = count_y;
      Tilt.write(count_y);
    }
  }
  if (x_coordinate_face < (x_center + error)){
    if(count_x <= 175){
      count_x += motor_speed_x;
      previous_x = count_x;
      Pan.write(count_x);
    }
  }
  else if (x_coordinate_face > (x_center - error)){
    if(count_x >= 5){
      count_x -= motor_speed_x;
      previous_x = count_x;
      Pan.write(count_x);
    }
  }
/*
  //if differences between x_coordinate and y_coordinate is detected
  if(x_center != x_coordinate_face || y_center != y_coordinate_face){

    //case 1: on top left of screen capture
    if (x_coordinate_face < x_center && y_coordinate_face < y_center){
      //pan first
      //to left
      updated_x_angle = map(x_coordinate_face, 0, 500, 180, 0);
      if(updated_x_angle < 150){
        Pan.write(updated_x_angle);
      }
      else if(updated_x_angle > 150){
        Pan.write(150);
      }
      //now tilt
      //upper
      updated_y_angle = map(y_coordinate_face, 0, 500, 0, 180);
      if(updated_y_angle > 30 ){
        count_y++;
        Tilt.write(count_y); 
      }
      else if(updated_y_angle < 30){
        Tilt.write(30);
      }
    }
    
    
  }
  */
}

void loop() {
  blink_eye_count++;
  if (blink_eye_count >= 250000){
      digitalWrite(LeftLED,LOW);
      digitalWrite(RightLED,LOW);
      delay(50);
      digitalWrite(LeftLED,HIGH);
      digitalWrite(RightLED,HIGH);
      blink_eye_count = 0;
  }

  if(Serial.available() > 0){
    if(Serial.read() == 'X'){
      x_coordinate_face = Serial.parseInt();
      if(Serial.read() == 'Y'){
        y_coordinate_face = Serial.parseInt();
        Pos();
        if(Serial.read() == 'E'){
          emotion_index = Serial.parseInt();
          Emotion_action();
        }
      }
    }
    while(Serial.available() > 0){
      Serial.read();
    }
  }

}
