#include <stdbool.h>

//#define V_THRESHOLD 800   //Minimum voltage from board to initiate RTD mode 0 < Thers < 5.0

#define TIME_DELAY 2000     //Time that buzzer is on in ms after RTD Initial  1000<= TIME_DELAY <= 3000


#define KEY 12
#define HARD_BRAKE 11 //Hard Braking
#define START_BUTTON 10
#define TS_HIGH 9     // Tractive System > 60V
#define BUZZER 17     
#define RTD_ENABLE 16 // RTD Control pin
#define PB_LED 5      // Push button LED


/* * * * * * * * * * 
ARDUINO CONSTANTS
COMMENT OUT FOR COMPILATION AND UPLOAD IN ARDUINO IDE
* * * * * * * * * */

#define LOW 0
#define HIGH 1
#define RISING 1
#define FALLING 1
#define INPUT 1
#define OUTPUT 1

void pinMode(int i, int j);
void attachInterrupt(int i, void* j, int k);
void detachInterrupt(int i);
int digitalPinToInterrupt(int i);
void digitalWrite(int i, int j);
int digitalRead(int i);
void interrupts(void);
int millis(void);

/* * * * * * * * * * 
--------------------
* * * * * * * * * */

void ISR_KEY (void);
void ISR_RESET (void);
void ISR_BUZZER (void);

void setup() {
  pinMode(KEY, INPUT); //A0
  pinMode(HARD_BRAKE, INPUT); //A1
  pinMode(START_BUTTON, INPUT); // A2
  pinMode(TS_HIGH, INPUT); // A3
  pinMode(BUZZER, OUTPUT); //D2,05
  pinMode(RTD_ENABLE, OUTPUT); //D3
  pinMode(START_BUTTON, OUTPUT); //D10
  
  attachInterrupt(digitalPinToInterrupt(KEY), ISR_KEY, RISING);
  attachInterrupt(digitalPinToInterrupt(KEY), ISR_RESET, LOW);
  attachInterrupt(digitalPinToInterrupt(TS_HIGH), ISR_RESET, LOW);
  
  interrupts();

  t0 = millis();
}

volatile int state = 0;
unsigned long startupTime = 0;
volatile int RESET = 1;
volatile int READY = 0;
volatile int PRIMING = 0;
volatile int RTD = 0;

int t0;

void loop() 
{
  if (!digitalRead(KEY) || !digitalRead(TS_HIGH))
    ISR_RESET();
  if (RESET) { //RESET is broken in ISR_KEY (triggerd on rising key) 
    digitalWrite(RTD_ENABLE, LOW);
    digitalWrite(BUZZER, LOW);
    digitalWrite(PB_LED, LOW);
    return;
  }

  if (PRIMING) {
    int startTime = millis();
    while (millis() < startTime + TIME_DELAY) {
      digitalWrite(BUZZER, HIGH); 
      if ((millis() - t0) > 500) {
        t0 = millis();
        digitalWrite(PB_LED, HIGH);
      }  
      else digitalWrite(PB_LED, LOW);
    }
    digitalWrite(BUZZER, LOW);
    PRIMING = 0;
    return;
  }

  if (READY) {
    if ((millis() - t0) > 500) {
      t0 = millis();
      digitalWrite(PB_LED, HIGH);
    } 
    
    else digitalWrite(PB_LED, LOW);

    return;
  }

  if (RTD) {
    digitalWrite(RTD_ENABLE, HIGH);
    digitalWrite(PB_LED, HIGH);
  }
 
}

void ISR_KEY(void) {
  if (READY || RTD || BUZZER)
    return; 
  READY = digitalRead(TS_HIGH);
  if (READY) 
    attachInterrupt(digitalPinToInterrupt(START_BUTTON), ISR_BUZZER, FALLING);
  RESET = !READY;
  return;
}

void ISR_BUZZER (void) {
  PRIMING = digitalRead(HARD_BRAKE);
  if (PRIMING) 
    detachInterrupt(digitalPinToInterrupt(START_BUTTON));
  READY = !PRIMING;
}

void ISR_RESET (void) {
  detachInterrupt(digitalPinToInterrupt(START_BUTTON));
  RESET = 1;
} 