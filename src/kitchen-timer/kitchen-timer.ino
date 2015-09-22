#include <Time.h>

// pins for 7-segments (4-digits)
int dataPin = 2;
int clockPin = 3;
int latchPin = 4;

int digit1Pin = 5;
int digit2Pin = 6;
int digit3Pin = 7;
int digit4Pin = 8;

// digits data
byte digits_data[10] = { 0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE4, 0xFE, 0xE6};

// time values
int t_min = 0;
int t_sec = 0;
long t_started = 0;
long t_time = 0;

// piezo
int piezo = 9;

// buttons
int btn1 = 10;
int btn1p = LOW; // previous
int btn1c = LOW; // current

int btn2 = 11;
int btn2p = LOW; // previous
int btn2c = LOW; // current

int btn3 = 12;
int btn3p = LOW; // previous
int btn3c = LOW; // current

int btn4 = 13;
int btn4p = LOW; // previous
int btn4c = LOW; // current

// mode
int mode = 0; // 0 = ready mode, 1 = running mode

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(digit1Pin, OUTPUT);
  pinMode(digit2Pin, OUTPUT);
  pinMode(digit3Pin, OUTPUT);
  pinMode(digit4Pin, OUTPUT);

  pinMode(piezo, OUTPUT);

  pinMode(btn1, INPUT);
  pinMode(btn2, INPUT);
  pinMode(btn3, INPUT);
  pinMode(btn4, INPUT);

  t_min = 0;
  t_sec = 0;
}

void selectDigit(int digit) {
  digitalWrite(digit1Pin, (digit == (digit1Pin - 5)) ? LOW : HIGH);
  digitalWrite(digit2Pin, (digit == (digit2Pin - 5)) ? LOW : HIGH);
  digitalWrite(digit3Pin, (digit == (digit3Pin - 5)) ? LOW : HIGH);
  digitalWrite(digit4Pin, (digit == (digit4Pin - 5)) ? LOW : HIGH);
}

void showDigits() {
  int min_digit1 = t_min / 10;
  int min_digit2 = t_min % 10;
  int sec_digit1 = t_sec / 10;
  int sec_digit2 = t_sec % 10;
  
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, digits_data[min_digit1]);
  selectDigit(0);
  digitalWrite(latchPin, HIGH);
  delay(5);
  
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, digits_data[min_digit2] | B00000001);
  selectDigit(1);
  digitalWrite(latchPin, HIGH);
  delay(5);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, digits_data[sec_digit1]);
  selectDigit(2);
  digitalWrite(latchPin, HIGH);
  delay(5);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, digits_data[sec_digit2]);
  selectDigit(3);
  digitalWrite(latchPin, HIGH);
  delay(5);

}

void beep() {
  tone(piezo, 261);
  delay(50);
  noTone(piezo);    
}

void startBeep() {
  tone(piezo, 261);
  delay(100);
  tone(piezo, 392);
  delay(100);
  noTone(piezo);  
}

void cancelBeep() {
  tone(piezo, 392);
  delay(100);
  tone(piezo, 261);
  delay(100);
  noTone(piezo);  
}

void finishBeep() {
  tone(piezo, 261);
  delay(500);
  tone(piezo, 330);
  delay(500);
  tone(piezo, 392);
  delay(500);
  noTone(piezo);  
}

void loop() {
  // compute time
  
  if (mode == 0) { // ready mode

    // add 1 minute 
    btn1c = digitalRead(btn1);
    if (btn1c != btn1p) {
      btn1p = btn1c;
      if (btn1c == HIGH) {
        t_min = t_min + 1;
        beep();
      }
    }

    // add 5 minutes
    btn2c = digitalRead(btn2);
    if (btn2c != btn2p) {
      btn2p = btn2c;
      if (btn2c == HIGH) {
        t_min = t_min + 5;
        beep();
      }
    }

    // clear time
    btn3c = digitalRead(btn3);
    if (btn3c != btn3p) {
      btn3p = btn3c;
      if (btn3c == HIGH) {
        t_min = 0;
        t_sec = 0;
        beep();
      }
    }

    // start timer
    btn4c = digitalRead(btn4);
    if (btn4c != btn4p) {
      btn4p = btn4c;
      if (btn4c == HIGH) {
        t_started = millis();
        t_time = (t_min * 60000) + (t_sec * 1000);
        mode = 1; // go to runing mode
        startBeep();
      }
    }    

  } else if (mode == 1) { // running mode

    // compute time
    long t_now = millis();
    long t_remained = t_time - (t_now - t_started);
    t_min = t_remained / 60000;
    t_sec = (t_remained % 60000) / 1000;

    // timer done.
    if (t_remained <= 0) {
      t_min = 0;
      t_sec = 0;
      showDigits();
      mode = 0; // go to ready mode
      finishBeep();
    }

    // cancel timer
    btn3c = digitalRead(btn3);
    if (btn3c != btn3p) {
      btn3p = btn3c;
      if (btn3c == HIGH) {
        t_min = 0;
        t_sec = 0;
        showDigits();
        mode = 0; // goto ready mode
        cancelBeep();
      }
    }

  }
  
  showDigits();  
}

