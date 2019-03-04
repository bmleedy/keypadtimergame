/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/




#include <Keypad.h>

#define TONE_PIN  10

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','a'},
  {'4','5','6','b'},
  {'7','8','9','c'},
  {'*','0','#','d'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad


#define FUSE_LENGTH_A  20
#define FUSE_LENGTH_B  40
#define FUSE_LENGTH_C  80
#define FUSE_LENGTH_D  160  // sec

unsigned int explosion_seconds = FUSE_LENGTH_B;

#define CODE_LEN 3
byte button_buffer[CODE_LEN];
byte code_buffer[CODE_LEN];

void update_buffer(char button){
  // slide all entries back one, expiring the oldest
  for(int i=0; i<(CODE_LEN-1); i++){
    button_buffer[i] = button_buffer[i+1];
  }
  // put the new code in the end
  button_buffer[CODE_LEN-1] = button;
}


void print_buffer(byte * buf){
  for(int i=0; i<CODE_LEN; i++){
    Serial.print("[");
    Serial.print(buf[i]);
    Serial.print("]");
  }
  Serial.println("");
}

void set_code(){
  for(int i=0; i<CODE_LEN; i++){
    code_buffer[i] = button_buffer[i]; //yes, I could memcpy as well!
  }
}

bool check_code(){
  for(int i=0; i<CODE_LEN; i++){
    if(button_buffer[i] != code_buffer[i])
      return false;
  }
  return true;
}

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup(){
  Serial.begin(57600);
}


bool is_armed = false;
unsigned long arm_time = 0;

void loop(){
  char key = keypad.getKey();
  
  if (key != NO_KEY){
    Serial.print(F("Pressed: "));
    Serial.println(key);
    
    switch(key){
      case 'a':
        if(is_armed)
          break;
        tone(TONE_PIN,180,200);
        explosion_seconds = FUSE_LENGTH_A;
        break;
      case 'b':
        if(is_armed)
          break;
        tone(TONE_PIN,180,200);
        explosion_seconds = FUSE_LENGTH_B;
        break;
      case 'c':
        if(is_armed)
          break;
        tone(TONE_PIN,180,200);
        explosion_seconds = FUSE_LENGTH_C;
        break;
      case 'd':
        if(is_armed)
          break;
        tone(TONE_PIN,180,200);
        explosion_seconds = FUSE_LENGTH_D;
        break;
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '0':
        update_buffer(key);
        tone(TONE_PIN, 500, 300);
        break;
      case '*':   // arm
        if(is_armed)
          break;
        tone(TONE_PIN, 700, 1000);
        set_code();
        Serial.println(F("Armed - Code set to:"));
        print_buffer(code_buffer);
        is_armed = true;
        arm_time = millis();
        break;
      case '#':   // try to disarm
        if(!is_armed)
          break;
        if(check_code()){
          is_armed = false;
          Serial.println(F("Disarmed!"));
          tone(TONE_PIN, 900, 1000);
        } else {
          Serial.println(F("Code Fail!"));
          tone(TONE_PIN, 90, 1000);
        }
        
        break;
  } // switch

  print_buffer(button_buffer);


  } //if key

  // Check for explosion every 1 second
  int time_armed_ms = millis() - arm_time;
  if(is_armed && (time_armed_ms % 2000)==0){
    if( time_armed_ms >= explosion_seconds * 1000){
      tone(TONE_PIN, 90, 10000);  // EXPLODE!!!!!
      is_armed = 0;
    } else {
      tone(TONE_PIN, 1700, 20);  // Just BEEP
    }
  } // if is armed
}


