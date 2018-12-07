#include <avr/sleep.h> 
#include <avr/wdt.h> 

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define  a3f    208     // 208 Hz
#define  b3f    233     // 233 Hz
#define  b3     247     // 247 Hz
#define  c4     261     // 261 Hz MIDDLE C
#define  c4s    277     // 277 Hz
#define  e4f    311     // 311 Hz    
#define  f4     349     // 349 Hz 
#define  a4f    415     // 415 Hz  
#define  b4f    466     // 466 Hz 
#define  b4     493     // 493 Hz 
#define  c5     523     // 523 Hz 
#define  c5s    554     // 554 Hz
#define  e5f    622     // 622 Hz  
#define  f5     698     // 698 Hz 
#define  f5s    740     // 740 Hz
#define  a5f    831     // 831 Hz 

int giWDCounter = 0;
const int buzzer = 2;

volatile int beatlength = 120; // determines tempo
int beatseparationconstant = 0.3;
//repetitions:
//intervals are a random in between min and max
int giRepetitions = 60/8 * 60 * 24;		//first call is triggered
const int minTime = 60/8 * 60;			//least time interval
const int maxTime = 60/8 * 60 * 2;	//top time interval

int song1_chorus_melody[] =
{ b4f, b4f, a4f, a4f,
  f5, f5, e5f, b4f, b4f, a4f, a4f, e5f, e5f, c5s, c5, b4f,
  c5s, c5s, c5s, c5s,
  c5s, e5f, c5, b4f, a4f, a4f, a4f, e5f, c5s,
  /*
  b4f, b4f, a4f, a4f,
  f5, f5, e5f, b4f, b4f, a4f, a4f, a5f, c5, c5s, c5, b4f,
  c5s, c5s, c5s, c5s,
  c5s, e5f, c5, b4f, a4f, rest, a4f, e5f, c5s, rest
  */
};

int song1_chorus_rhythmn[] =
{ 1, 1, 1, 1,
  3, 3, 6, 1, 1, 1, 1, 3, 3, 3, 1, 2,
  1, 1, 1, 1,
  3, 3, 3, 1, 2, 2, 2, 4, 8,
  /*
  1, 1, 1, 1,
  3, 3, 6, 1, 1, 1, 1, 3, 3, 3, 1, 2,
  1, 1, 1, 1,
  3, 3, 3, 1, 2, 2, 2, 4, 8, 4
  */
};

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(buzzer, OUTPUT);
  //let us know the device is on
  digitalWrite(buzzer,HIGH);
  delay(1000);
  digitalWrite(buzzer,LOW);
  
  cbi(ADCSRA,ADEN);		// disable ADC, no need on this project
  cbi( MCUCR,SE );      // sleep enable, power down mode
  cbi( MCUCR,SM0 );     // power down mode
  sbi( MCUCR,SM1 );     // power down mode
  setup_watchdog(9);
}

// the loop function runs over and over again forever
void loop() {
  //WDIE is cleared after each interrupt so this bit should be re-set
  WDTCR |= _BV(WDIE);
  system_sleep();
  if(giWDCounter >= giRepetitions)
  {
    annoy();
    //change time for next call
    giRepetitions = random(minTime,maxTime);
    giWDCounter = 0;
  }
}

void annoy()
{
  int b = 0;  
  int chorus_length = sizeof(song1_chorus_melody) / sizeof(int);
  
  while(b < chorus_length)
  {
    int notelength = beatlength * song1_chorus_rhythmn[b];
    if (song1_chorus_melody[b] > 0) {
      tone(buzzer, song1_chorus_melody[b], notelength);
    }
    b++;
    delay(notelength);
    noTone(buzzer);
    delay(notelength * beatseparationconstant);
  }
}

// Watchdog Interrupt Service / is executed when  watchdog timed out
ISR(WDT_vect) {
 giWDCounter++;  // set global flag
}

//****************************************************************
// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int ii) {
 byte bb;
 int ww;
 if (ii > 9 ) ii=9;
 bb=ii & 7;
 if (ii > 7) bb|= (1<<5);
 bb|= (1<<WDCE);
 ww=bb;
 MCUSR &= ~(1<<WDRF);
 // start timed sequence
 WDTCR |= (1<<WDCE) | (1<<WDE);
 // set new watchdog timeout value
 WDTCR = bb;
 WDTCR |= _BV(WDIE);
}

//****************************************************************  
// set system into the sleep state 
// system wakes up when wtchdog is timed out
void system_sleep() {
 //we dont need ADC so we will disable on setup
 //cbi(ADCSRA,ADEN);  // switch Analog to Digitalconverter OFF
 set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
 sleep_enable();
 sleep_mode();                        // System sleeps here
 sleep_disable();              // System continues execution here when watchdog timed out
 //we dont need ADC on this projetc
 //sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
}
