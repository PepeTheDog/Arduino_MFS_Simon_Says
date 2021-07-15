int latchPin = 4;
int dataPin = 8;
int clockPin = 7;

#define LEFT_BTN A1
#define SELECT_BTN A2
#define RIGHT_BTN A3

uint8_t lBtnPressed = 0;
uint8_t sBtnPressed = 0;
uint8_t rBtnPressed = 0;
unsigned long lUnpressTimer;
unsigned long sUnpressTimer;
unsigned long rUnpressTimer;

byte slot[] = {0b00000001, 0b00000010, 0b00000100, 0b00001000};

int outputs[100];
int inputs[100];
int selection = 0;
int ansCheckAux;
bool isWrong = false;

char state = 's';
uint16_t level = 1;
int sStateAux1 = 0;
int sStateAux2 = 0;
int pStateAux = 0;

bool calcLevel = true;

unsigned long displayOutputsTimer = 0;
bool resetOutputsTimer = true;

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  Serial.begin(9600);
  randomSeed(analogRead(A0));
}

void checkLBtn(){
  unsigned long pressTimer;
  if ((digitalRead(LEFT_BTN) == LOW) && (lBtnPressed == 0)){
    lBtnPressed = 1;
    pressTimer = millis();
  }
  if ((digitalRead(LEFT_BTN) == HIGH) && (lBtnPressed == 1) && (millis() - pressTimer > 200))
    lBtnPressed = 0;
  if (lBtnPressed == 2 && (millis() - lUnpressTimer > 300))
    lBtnPressed = 0;
}

void checkSBtn(){
  unsigned long pressTimer;
  if ((digitalRead(SELECT_BTN) == LOW) && (sBtnPressed == 0)){
    sBtnPressed = 1;
    pressTimer = millis();
  }
  if ((digitalRead(SELECT_BTN) == HIGH) && (sBtnPressed == 1) && (millis() - pressTimer > 200))
    sBtnPressed = 0;
  if (sBtnPressed == 2 && (millis() - sUnpressTimer > 300))
    sBtnPressed = 0;
}

void checkRBtn(){
  unsigned long pressTimer;
  if ((digitalRead(RIGHT_BTN) == LOW) && (rBtnPressed == 0)){
    rBtnPressed = 1;
    pressTimer = millis();
  }
  if ((digitalRead(RIGHT_BTN) == HIGH) && (rBtnPressed == 1) && (millis() - pressTimer > 200))
    rBtnPressed = 0;
  if (rBtnPressed == 2 && (millis() - rUnpressTimer > 300))
    rBtnPressed = 0;
}

void displayObj(int slotNum){
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0b11000000);
  shiftOut(dataPin, clockPin, MSBFIRST, slot[slotNum]);
  digitalWrite(latchPin, HIGH);
}

void displayDot(int slotNum){
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0b01111111);
  shiftOut(dataPin, clockPin, MSBFIRST, slot[slotNum]);
  digitalWrite(latchPin, HIGH);
}

void loop() {
  if (state == 's'){
    if (calcLevel == true){
      for (sStateAux1; sStateAux1<level; sStateAux1++){
        outputs[sStateAux1] = random(4);
        // Serial.println(outputs[sStateAux1]);
      }
      calcLevel = false;
    }
    //Serial.println(sStateAux2);
    if (level >= sStateAux2+1){
      displayObj(outputs[sStateAux2]);
      if (resetOutputsTimer == true){
        displayOutputsTimer = millis();
        resetOutputsTimer = false;
      }
      if (millis() - displayOutputsTimer > 1000){
        sStateAux2++;
        resetOutputsTimer = true;
      }
      // Serial.println(level);
      // Serial.println(sStateAux2);
    }
    else state = 'p';
  }
  else if (state == 'p'){
    // Serial.println("p-body");
    if (level >= pStateAux+1){
      if (lBtnPressed == 1){
        selection--;
        if (selection < 0)
          selection = 3;
        lBtnPressed = 2;
        lUnpressTimer = millis();
      }
      if (rBtnPressed == 1){
        selection++;
        if (selection > 3)
          selection = 0;
        rBtnPressed = 2;
        rUnpressTimer = millis();
      }
      if (sBtnPressed == 1){
        inputs[pStateAux] = selection;
        pStateAux++;
        sBtnPressed = 2;
        sUnpressTimer = millis();
      }
      displayDot(selection);
    }
    else{
      state = 'c';
      pStateAux = 0;
    }
  }
  else if (state == 'c'){
    for (ansCheckAux = 0; ansCheckAux<level; ansCheckAux++){
      if (outputs[ansCheckAux] != inputs[ansCheckAux]){
        isWrong = true;
        break;
      }
    }
    ansCheckAux = 0;
    if (isWrong){
      level = 1;
      sStateAux1 = 0;
      sStateAux2 = 0;
      calcLevel = true;
      isWrong = false;
      state = 's';
    }
    else{
      level++;
      sStateAux2 = 0;
      calcLevel = true;
      state = 's';
    }
  }
  //Serial.println(state);
  //Serial.println(outputs[1]);
  //Serial.println(inputs[1]);
  checkLBtn();
  checkSBtn();
  checkRBtn();
}
