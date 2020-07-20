//Pin connected to ST_CP of 74HC595
int latchPin = 8;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 11;

int modeButtonPin = 4;

int manualButtonPin = 7;

int dimPin = 5;

unsigned long ts;

enum State {
  ALL_ON,
  ALL_OFF,
  STROBE,
  SCAN,
  BOUNCE,
  FADE,
  BLINK,
  MANUAL
};

State state = FADE;

int index = 0;

byte scan[6];
byte bounce[12];

void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(dimPin, OUTPUT);
  pinMode(modeButtonPin, INPUT_PULLUP);
  pinMode(manualButtonPin, INPUT_PULLUP);

  digitalWrite(latchPin, LOW);
  // shift out the bits:
  shiftOut(dataPin, clockPin, MSBFIRST, 0xFF);  

  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);
  ts = millis();

  scan[0] = 0x02;
  scan[1] = 0x04;
  scan[2] = 0x08;
  scan[3] = 0x10;
  scan[4] = 0x20;
  scan[5] = 0x40;

  bounce[0] = 0x02;
  bounce[1] = 0x04;
  bounce[2] = 0x08;
  bounce[3] = 0x10;
  bounce[4] = 0x20;
  bounce[5] = 0x40;
  bounce[11] = 0x02;
  bounce[10] = 0x04;
  bounce[9] = 0x08;
  bounce[8] = 0x10;
  bounce[7] = 0x20;
  bounce[6] = 0x40;
}

bool buttonLock = false;
void loop() {

  if (digitalRead(modeButtonPin) == LOW && !buttonLock){
    buttonLock = true;
     switch(state){

      case ALL_ON:
        state = ALL_OFF;
        break;
      case ALL_OFF:
        state = STROBE;
        break;
      case STROBE:
        state = SCAN;
        break;
      case SCAN:
        state = BOUNCE;
        break;
      case BOUNCE:
        state = FADE;
        break;
      case FADE:
        state = ALL_ON;
        break;
      
      case MANUAL:
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, MSBFIRST, 0x7E);  
        digitalWrite(latchPin, HIGH);
        analogWrite(dimPin, 255);
        state = ALL_ON;
        break;
    }
    delay(250);
  }
  else{
    if (digitalRead(manualButtonPin) == LOW){
      state = MANUAL;
    }
    buttonLock = false;
  }

  

  

  switch(state){

    case ALL_ON:
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, MSBFIRST, 0x7E);  
        digitalWrite(latchPin, HIGH);
        analogWrite(dimPin, 0);
      break;
    case ALL_OFF:
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, MSBFIRST, 0x00);  
        digitalWrite(latchPin, HIGH);
      break;
    case STROBE:
      if (millis() - ts > 50){
        ts = millis();
        index ++;
        if (index >= 2){
          index = 0;
        }
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, MSBFIRST, index == 0? 0x00 : 0x7E);  
        digitalWrite(latchPin, HIGH);
      }
      break;
    case SCAN:
      if (millis() - ts > 100){
        ts = millis();
        index ++;
        if (index >= 6){
          index = 0;
        }
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, MSBFIRST, scan[index]);  
        digitalWrite(latchPin, HIGH);
      }
      break;
    case BOUNCE:
      if (millis() - ts > 100){
        ts = millis();
        index ++;
        if (index >= 12){
          index = 0;
        }
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, MSBFIRST, bounce[index]);  
        digitalWrite(latchPin, HIGH);
      }
      break;
    case FADE:
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 0x7E);  
      digitalWrite(latchPin, HIGH);
      analogWrite(dimPin, cos(millis()*0.004)*128 + 128);
      break;
      
    case MANUAL:
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 0x7E);  
      digitalWrite(latchPin, HIGH);
      if (digitalRead(manualButtonPin) == HIGH){
        index += (millis()-ts)*1;
        if (index > 255) index = 255;
        
      }
      else{
        index -= (millis()-ts)*2;
        if (index < 0) index = 0;
        
      }
      analogWrite(dimPin, index);
      ts = millis();
      break;
  }


}
