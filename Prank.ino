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

// the setup function runs once when you press reset or power the board
void setup() {
  DDRB = 0b0010;  
  //let us know the device is on
  PORTB = 0b0010;
  delay(100);
  PORTB = 0b0000;
  
  cbi(ADCSRA,ADEN);//disable ADC, no need on this project
  cbi( MCUCR,SE );      // sleep enable, power down mode
  cbi( MCUCR,SM0 );     // power down mode
  sbi( MCUCR,SM1 );     // power down mode
  setup_watchdog(9);
}

// the loop function runs over and over again forever
void loop() {
  system_sleep();
  if(giWDCounter == 400)
  {
    giWDCounter = 0;
    annoy();
  }
}

void annoy()
{
  int b = 0;  
    int song1_chorus_melody[] =
  { b4f, b4f, a4f, a4f,
    f5, f5, e5f, b4f, b4f, a4f, a4f, e5f, e5f, c5s, c5, b4f,
    c5s, c5s, c5s, c5s,
    c5s, e5f, c5, b4f, a4f, a4f, a4f, e5f, c5s,
  };
  int song1_chorus_rhythmn[] =
  { 1, 1, 1, 1,
    3, 3, 6, 1, 1, 1, 1, 3, 3, 3, 1, 2,
    1, 1, 1, 1,
    3, 3, 3, 1, 2, 2, 2, 4, 8,
  };
  
  while(b < sizeof(song1_chorus_melody) / sizeof(int))
  {
    if (song1_chorus_melody[b] > 0) {
      tone(2, song1_chorus_melody[b], 120 * song1_chorus_rhythmn[b]);
    }
    b++;
    delay(120 * song1_chorus_rhythmn[b]);
    noTone(2);
    delay(120 * song1_chorus_rhythmn[b] * 0.3);
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
 WDTCR |= _BV(WDTIE);
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
