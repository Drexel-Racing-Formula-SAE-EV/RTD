int TimeDelay = 2000;     //Time Buzzer is on in ms after RTD Initial  1000<= TimeDelay <= 3000
float Thres = 800;        //Minimum voltage from board to initiate RTD mode 0 < Thers < 5.0

int key = A0; // Key to turn
int brakeSens = A1; // Sense the brake
int startButton = A2; // Push button
int tractiveSystem = A3; // Don't know what tractive means but it is a switch
int buzzer = 11; // Buzzer Pin
int RTD_Enable = 12; // RTD Enable pin
int startButtonLED = 10; // Push button LED

void setup() {
  pinMode(key, INPUT); //A0
  pinMode(brakeSens, INPUT); //A1
  pinMode(startButton, INPUT); // A2
  pinMode(tractiveSystem, INPUT); // A3
  pinMode(buzzer, OUTPUT); //D2,05
  pinMode(RTD_Enable, OUTPUT); //D3
  pinMode(startButtonLED, OUTPUT); //D10
  Serial.begin(9600);
}

int state = 0;
unsigned long startupTime = 0;

void loop() 
{
  // If the key or tractive system swtich are ever turned off, deactivate Ready to Drive make sure the buzzer is off and turn back on the buttonled
  if(!keyTurned() || !tractiveSystemOn()) {
    digitalWrite(RTD_Enable, LOW);
    digitalWrite(buzzer, LOW);
    digitalWrite(startButtonLED, HIGH);
    state = 0;
  }

  if (state == 0 && keyTurned() && tractiveSystemOn())
    state = 1;

  if (state == 1 && brakeOn()) { // If the brake is on
    if ((millis() / 650) % 2 == 1)
      digitalWrite(startButtonLED, HIGH);
    else
      digitalWrite(startButtonLED, LOW);

    if(startButtonPressed()) {
      digitalWrite(startButton, LOW); // Make sure button LED is off
      state = 2;
    }
  } else if(state == 1 && !brakeOn()) {
    digitalWrite(startButtonLED, LOW);
  }

  if (state == 2) {
    startupTime = millis();
    digitalWrite(buzzer, HIGH);
    state = 3;
  }

  if(state == 3) {
    if ((millis() / 100 ) % 2 == 1) {
      digitalWrite(startButtonLED, HIGH);
    } else {
      digitalWrite(startButtonLED, LOW);
    }
     
    if (millis() - startupTime > TimeDelay) {
      digitalWrite(buzzer, LOW);
      state = 4;
    }
  }

  if (state == 4) {
    digitalWrite(RTD_Enable, HIGH);
    digitalWrite(startButtonLED, HIGH);

    delay(0);
  }
  //digitalWrite(13, HIGH);
  Serial.println(state);
}

boolean keyTurned() {
  return analogRead(key) > 500;
}

boolean tractiveSystemOn() {
  return analogRead(tractiveSystem) > 20;
}

boolean brakeOn() {
  return true;
  //return analogRead(brakeSens) > 500;
}

boolean startButtonPressed() {
  return analogRead(startButton) > 500;
}
