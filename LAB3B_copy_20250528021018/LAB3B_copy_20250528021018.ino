const byte ANODE_PINS[8]  = {13, 12, 11, 10,  9,  8,  7,  6}; 
const byte CATHODE_PINS[8] = {A3, A2, A1, A0, 5, 4, 3, 2};  

const byte LEFT_BUTTON = A5;  
const byte RIGHT_BUTTON = A4; 

bool leftButtonState = false;
bool rightButtonState = false;
bool lastLeftButtonState = false;
bool lastRightButtonState = false;
unsigned long leftButtonPressTime = 0;
unsigned long rightButtonPressTime = 0;
unsigned long leftButtonReleaseTime = 0;
unsigned long rightButtonReleaseTime = 0;
bool leftButtonPressed = false;
bool rightButtonPressed = false;
bool leftHoldSent = false;
bool rightHoldSent = false;

const unsigned long DEBOUNCE_DELAY = 50;
const unsigned long DOUBLE_CLICK_TIME = 300;
const unsigned long HOLD_TIME = 1000;       
const unsigned long HOLD_REPEAT = 200;      

byte pattern[8][8] = {{0,0,0,0,0,0,0,0},                       
                      {0,0,0,0,0,0,0,0},                       
                      {0,0,0,0,0,0,0,0},                       
                      {0,0,0,0,0,0,0,0},                       
                      {0,0,0,0,0,0,0,0},                       
                      {0,0,0,0,0,0,0,0},                       
                      {0,0,0,0,0,0,0,0},                       
                      {0,0,0,0,0,0,0,0}};

const byte FONT_A[] = {0b111110, 0b001001, 0b111110}; // 3 wide
const byte FONT_B[] = {0b111111, 0b101001, 0b010110}; // 3 wide
const byte FONT_C[] = {0b011110, 0b100001, 0b100001}; // 3 wide
const byte FONT_D[] = {0b111111, 0b100001, 0b011110}; // 3 wide
const byte FONT_E[] = {0b111111, 0b101001, 0b100001}; // 3 wide
const byte FONT_F[] = {0b111111, 0b001001, 0b000001}; // 3 wide
const byte FONT_G[] = {0b011110, 0b100001, 0b101001, 0b111001}; // 4 wide
const byte FONT_H[] = {0b111111, 0b001000, 0b111111}; // 3 wide
const byte FONT_I[] = {0b111111}; // 1 wide
const byte FONT_J[] = {0b110000, 0b100000, 0b111111}; // 3 wide
const byte FONT_K[] = {0b111111, 0b001000, 0b110111}; // 3 wide
const byte FONT_L[] = {0b111111, 0b100000, 0b100000}; // 3 wide
const byte FONT_M[] = {0b111111, 0b000110, 0b000110, 0b111111}; // 4 wide
const byte FONT_N[] = {0b111111, 0b000011, 0b001100, 0b110000, 0b111111}; // 5 wide
const byte FONT_O[] = {0b011110, 0b100001, 0b011110}; // 3 wide
const byte FONT_P[] = {0b111111, 0b001001, 0b000110}; // 3 wide
const byte FONT_Q[] = {0b011110, 0b100001, 0b101001, 0b011110}; // 4 wide
const byte FONT_R[] = {0b111111, 0b001001, 0b110110}; // 3 wide
const byte FONT_S[] = {0b100110, 0b101001, 0b011001}; // 3 wide
const byte FONT_T[] = {0b000001, 0b111111, 0b000001}; // 3 wide
const byte FONT_U[] = {0b011111, 0b100000, 0b011111}; // 3 wide
const byte FONT_V[] = {0b001111, 0b110000, 0b001111}; // 3 wide
const byte FONT_W[] = {0b011111, 0b100000, 0b011110, 0b100000, 0b011111}; // 5 wide
const byte FONT_X[] = {0b110111, 0b001000, 0b110111}; // 3 wide
const byte FONT_Y[] = {0b000111, 0b111000, 0b000111}; // 3 wide
const byte FONT_Z[] = {0b110001, 0b101001, 0b100111}; // 3 wide
const byte FONT_SPACE[] = {0b000000}; // 1 wide

const byte FONT_0[] = {0b011110, 0b100001, 0b011110}; // 3 wide
const byte FONT_1[] = {0b100010, 0b111111, 0b100000}; // 3 wide
const byte FONT_2[] = {0b110010, 0b101001, 0b100110}; // 3 wide
const byte FONT_3[] = {0b100010, 0b101001, 0b010110}; // 3 wide
const byte FONT_4[] = {0b001111, 0b001000, 0b111111}; // 3 wide
const byte FONT_5[] = {0b100111, 0b101001, 0b011001}; // 3 wide
const byte FONT_6[] = {0b011110, 0b101001, 0b011000}; // 3 wide
const byte FONT_7[] = {0b000001, 0b111001, 0b000111}; // 3 wide
const byte FONT_8[] = {0b010110, 0b101001, 0b010110}; // 3 wide
const byte FONT_9[] = {0b000110, 0b101001, 0b011110}; // 3 wide

const byte FONT_WIDTHS[] = {
  3,3,3,3,3,3,4,3,1,3,3,3,4,5,3,3,4,3,3,3,3,3,5,3,3,3, // a-z
  3,3,3,3,3,3,3,3,3,3, // 0-9
  1 // space
};

String displayText = "NO SONG PLAYING";
int scrollPosition = 0;
unsigned long lastScrollTime = 0;
const int scrollDelay = 100;

bool showingAnimation = false;
unsigned long animationStartTime = 0;
const int animationDuration = 1000;
String currentAnimation = "";

void setup() {
  for (byte i = 0; i < 8; i++) {
    pinMode(ANODE_PINS[i], OUTPUT);
    pinMode(CATHODE_PINS[i], OUTPUT);
    digitalWrite(ANODE_PINS[i], LOW);
    digitalWrite(CATHODE_PINS[i], HIGH);
  }
  
  pinMode(LEFT_BUTTON, INPUT_PULLUP);   // Pins pulled HIGH, buttons pull LOW when pressed
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  
  Serial.begin(115200);
  Serial.println("Arduino LED Display Ready");
  Serial.println("Button controls enabled:");
  Serial.println("- Single press: Play/Pause");
  Serial.println("- Double press L/R: Skip back/forward");
  Serial.println("- Hold L/R: Volume down/up");
  Serial.println("- Buttons now use pull-up resistors (connect to GND when pressed)");
  Serial.println("Waiting for song data...");
}

const byte* getCharacterFont(char c) {
  if (c >= 'A' && c <= 'Z') {
    switch(c) {
      case 'A': return FONT_A;
      case 'B': return FONT_B;
      case 'C': return FONT_C;
      case 'D': return FONT_D;
      case 'E': return FONT_E;
      case 'F': return FONT_F;
      case 'G': return FONT_G;
      case 'H': return FONT_H;
      case 'I': return FONT_I;
      case 'J': return FONT_J;
      case 'K': return FONT_K;
      case 'L': return FONT_L;
      case 'M': return FONT_M;
      case 'N': return FONT_N;
      case 'O': return FONT_O;
      case 'P': return FONT_P;
      case 'Q': return FONT_Q;
      case 'R': return FONT_R;
      case 'S': return FONT_S;
      case 'T': return FONT_T;
      case 'U': return FONT_U;
      case 'V': return FONT_V;
      case 'W': return FONT_W;
      case 'X': return FONT_X;
      case 'Y': return FONT_Y;
      case 'Z': return FONT_Z;
    }
  } else if (c >= '0' && c <= '9') {
    switch(c) {
      case '0': return FONT_0;
      case '1': return FONT_1;
      case '2': return FONT_2;
      case '3': return FONT_3;
      case '4': return FONT_4;
      case '5': return FONT_5;
      case '6': return FONT_6;
      case '7': return FONT_7;
      case '8': return FONT_8;
      case '9': return FONT_9;
    }
  }
  return FONT_SPACE;
}

byte getCharacterWidth(char c) {
  if (c >= 'A' && c <= 'Z') {
    return FONT_WIDTHS[c - 'A'];
  } else if (c >= '0' && c <= '9') {
    return FONT_WIDTHS[26 + (c - '0')];
  }
  return FONT_WIDTHS[36];
}

void generateTextPattern(String text, int offset) {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      pattern[row][col] = 0;
    }
  }
  
  int currentX = 8 - offset;
  
  for (int i = 0; i < text.length(); i++) {
    char c = text.charAt(i);
    if (c >= 'a' && c <= 'z') c -= 32;
    
    const byte* charFont = getCharacterFont(c);
    byte charWidth = getCharacterWidth(c);
    
    for (int col = 0; col < charWidth; col++) {
      int displayCol = currentX + col;
      if (displayCol >= 0 && displayCol < 8) {
        byte columnData = charFont[col];
        for (int row = 0; row < 6; row++) {
          if (columnData & (1 << row)) {
            pattern[row][displayCol] = 1;
          }
        }
      }
    }
    
    currentX += charWidth + 1;
    
    if (currentX > 8) break;
  }
  
  for (int col = 0; col < 8; col++) {
    pattern[6][col] = 0;
    pattern[7][col] = 1;
  }
}

int getTotalTextWidth(String text) {
  int totalWidth = 0;
  for (int i = 0; i < text.length(); i++) {
    char c = text.charAt(i);
    if (c >= 'a' && c <= 'z') c -= 32;
    totalWidth += getCharacterWidth(c) + 1;
  }
  return totalWidth - 1;
}

void showPlayAnimation() {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      pattern[row][col] = 0;
    }
  }
  
  pattern[2][2] = 1;
  pattern[3][2] = 1; pattern[3][3] = 1;
  pattern[4][2] = 1; pattern[4][3] = 1; pattern[4][4] = 1;
  pattern[5][2] = 1; pattern[5][3] = 1;
  pattern[6][2] = 1;
  
  for (int col = 0; col < 8; col++) {
    pattern[7][col] = 1;
  }
}

void showPauseAnimation() {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      pattern[row][col] = 0;
    }
  }
  
=  for (int row = 2; row <= 5; row++) {
    pattern[row][2] = 1; pattern[row][3] = 1;
    pattern[row][5] = 1; pattern[row][6] = 1;
  }
  
  for (int col = 0; col < 8; col++) {
    pattern[7][col] = 1;
  }
}

void showSkipForwardAnimation() {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      pattern[row][col] = 0;
    }
  }
  
  pattern[2][1] = 1;
  pattern[3][1] = 1; pattern[3][2] = 1;
  pattern[4][1] = 1; pattern[4][2] = 1; pattern[4][3] = 1;
  pattern[5][1] = 1; pattern[5][2] = 1;
  pattern[6][1] = 1;
  
  pattern[2][4] = 1;
  pattern[3][4] = 1; pattern[3][5] = 1;
  pattern[4][4] = 1; pattern[4][5] = 1; pattern[4][6] = 1;
  pattern[5][4] = 1; pattern[5][5] = 1;
  pattern[6][4] = 1;
  
  for (int col = 0; col < 8; col++) {
    pattern[7][col] = 1;
  }
}

void showSkipBackAnimation() {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      pattern[row][col] = 0;
    }
  }
  
  pattern[2][2] = 1;
  pattern[3][1] = 1; pattern[3][2] = 1;
  pattern[4][0] = 1; pattern[4][1] = 1; pattern[4][2] = 1;
  pattern[5][1] = 1; pattern[5][2] = 1;
  pattern[6][2] = 1;
  
  // Second arrow (pointing left)
  pattern[2][5] = 1;
  pattern[3][4] = 1; pattern[3][5] = 1;
  pattern[4][3] = 1; pattern[4][4] = 1; pattern[4][5] = 1;
  pattern[5][4] = 1; pattern[5][5] = 1;
  pattern[6][5] = 1;
  
  for (int col = 0; col < 8; col++) {
    pattern[7][col] = 1;
  }
}

void showVolumeUpAnimation() {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      pattern[row][col] = 0;
    }
  }
  
  pattern[5][1] = 1;
  pattern[4][3] = 1; pattern[5][3] = 1;
  pattern[2][5] = 1; pattern[3][5] = 1; pattern[4][5] = 1; pattern[5][5] = 1;
  
  pattern[1][7] = 1; // Top of +
  pattern[2][6] = 1; pattern[2][7] = 1;
  
  for (int col = 0; col < 8; col++) {
    pattern[7][col] = 1;
  }
}

void showVolumeDownAnimation() {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      pattern[row][col] = 0;
    }
  }
  
  pattern[2][1] = 1; pattern[3][1] = 1; pattern[4][1] = 1; pattern[5][1] = 1; // Tall bar
  pattern[4][3] = 1; pattern[5][3] = 1;
  pattern[5][5] = 1;
  
  pattern[2][7] = 1;
  
  for (int col = 0; col < 8; col++) {
    pattern[7][col] = 1;
  }
}

void startAnimation(String animationType) {
  showingAnimation = true;
  animationStartTime = millis();
  currentAnimation = animationType;
  
  if (animationType == "PLAY") {
    showPlayAnimation();
  } else if (animationType == "PAUSE") {
    showPauseAnimation();
  } else if (animationType == "SKIP_FORWARD") {
    showSkipForwardAnimation();
  } else if (animationType == "SKIP_BACK") {
    showSkipBackAnimation();
  } else if (animationType == "VOLUME_UP") {
    showVolumeUpAnimation();
  } else if (animationType == "VOLUME_DOWN") {
    showVolumeDownAnimation();
  }
}

void checkForSerialData() {
  if (Serial.available()) {
    String newText = Serial.readStringUntil('\n');
    newText.trim();
    
    if (newText.length() > 0) {
      if (newText.startsWith("ANIM:")) {
        String animationType = newText.substring(5);
        startAnimation(animationType);
        Serial.println("Animation started: " + animationType);
      } else {
        displayText = newText;
        scrollPosition = 0;
        Serial.println("Updated display text: " + displayText);
      }
    }
  }
}

void handleButtons() {
  unsigned long currentTime = millis();

  bool currentLeftState = !digitalRead(LEFT_BUTTON);
  bool currentRightState = !digitalRead(RIGHT_BUTTON); 
  
  static unsigned long lastLeftRelease = 0;
  static unsigned long lastRightRelease = 0;
  static bool leftSinglePending = false;
  static bool rightSinglePending = false;
  
  if (currentLeftState != lastLeftButtonState) {
    if (currentLeftState == HIGH) {
      leftButtonPressTime = currentTime;
      leftButtonPressed = true;
      leftHoldSent = false;
      leftSinglePending = false; 
    } else {
      leftButtonReleaseTime = currentTime;
      unsigned long pressDuration = leftButtonReleaseTime - leftButtonPressTime;
      
      if (pressDuration >= HOLD_TIME || leftHoldSent) {
        leftSinglePending = false;
      } else {
        if (currentTime - lastLeftRelease < DOUBLE_CLICK_TIME && lastLeftRelease > 0) {
          Serial.println("CMD:SKIP_BACK");
          startAnimation("SKIP_BACK");
          lastLeftRelease = 0; 
          leftSinglePending = false; 
        } else {
          lastLeftRelease = currentTime;
          leftSinglePending = true;
        }
      }
      leftButtonPressed = false;
    }
    lastLeftButtonState = currentLeftState;
  }
  
  if (currentRightState != lastRightButtonState) {
    if (currentRightState == HIGH) {
      rightButtonPressTime = currentTime;
      rightButtonPressed = true;
      rightHoldSent = false;
      rightSinglePending = false;
    } else {
      rightButtonReleaseTime = currentTime;
      unsigned long pressDuration = rightButtonReleaseTime - rightButtonPressTime;
      
      if (pressDuration >= HOLD_TIME || rightHoldSent) {
        rightSinglePending = false;
      } else {
        if (currentTime - lastRightRelease < DOUBLE_CLICK_TIME && lastRightRelease > 0) {
          Serial.println("CMD:SKIP_FORWARD");
          startAnimation("SKIP_FORWARD");
          lastRightRelease = 0;
          rightSinglePending = false;
        } else {
          lastRightRelease = currentTime;
          rightSinglePending = true;
        }
      }
      rightButtonPressed = false;
    }
    lastRightButtonState = currentRightState;
  }
  
  if (leftSinglePending && (currentTime - lastLeftRelease > DOUBLE_CLICK_TIME)) {
    Serial.println("CMD:PLAY_PAUSE");
    startAnimation("PAUSE");
    leftSinglePending = false;
  }
  
  if (rightSinglePending && (currentTime - lastRightRelease > DOUBLE_CLICK_TIME)) {
    Serial.println("CMD:PLAY_PAUSE");
    startAnimation("PAUSE");
    rightSinglePending = false;
  }
  
  // Handle hold actions
  if (leftButtonPressed && (currentTime - leftButtonPressTime > HOLD_TIME)) {
    static unsigned long lastLeftHold = 0;
    if (currentTime - lastLeftHold > HOLD_REPEAT) {
      Serial.println("CMD:VOLUME_DOWN");
      startAnimation("VOLUME_DOWN");
      lastLeftHold = currentTime;
      leftHoldSent = true;
      leftSinglePending = false;
    }
  }
  
  if (rightButtonPressed && (currentTime - rightButtonPressTime > HOLD_TIME)) {
    static unsigned long lastRightHold = 0;
    if (currentTime - lastRightHold > HOLD_REPEAT) {
      Serial.println("CMD:VOLUME_UP");
      startAnimation("VOLUME_UP");
      lastRightHold = currentTime;
      rightHoldSent = true;
      rightSinglePending = false;
    }
  }
}

void loop() {
  checkForSerialData();
  
  handleButtons();
  
  if (showingAnimation) {
    if (millis() - animationStartTime > animationDuration) {
      showingAnimation = false;
    }
  } else {
    if (millis() - lastScrollTime > scrollDelay) {
      scrollPosition++;
      int totalWidth = getTotalTextWidth(displayText);
      if (scrollPosition > totalWidth + 8) {
        scrollPosition = 0;
      }
      generateTextPattern(displayText, scrollPosition);
      lastScrollTime = millis();
    }
  }
  
\  for (byte col = 0; col < 8; col++) {
    digitalWrite(CATHODE_PINS[col], LOW);
    for (byte row = 0; row < 8; row++) {
      if (pattern[row][col] == 0) {
        digitalWrite(ANODE_PINS[row], HIGH);
      } else {
        digitalWrite(ANODE_PINS[row], LOW);
      }
    }
    delay(1);
    digitalWrite(CATHODE_PINS[col], HIGH);
  }
}