#include "my_ping.h"

/*
  Ping() implementation for hc-sr04 eccho sensor.

  This requires some more precise timing; IOW we must know delayMS(1) gives 1ms, and readtimer4() gives some ms-translatable val
  Model according to the condition that the echo line lowers after about 30MS anyway other infinite distance / horizon conditions.

  Returns A range, some infinity valu ( 0? ), or an error code ( -1? ) ????

  May build on this to capture whatever experimental conditions arise:
    case: values at < 2cm distance
    case: inifinite distances
    case: warped echoes (soft or angular obstacles)
    case: etc.
    code: poll sensor 10 times, average, filter, markov, or discard bastard vals > stdDev

  Make triggers (and thus readings) per second is probably close to 20.

*/
unsigned int sr04_ping(void)
{
  unsigned int start = 0, end = 0, dt = 0, dist = 0, i = 0, j = 0, k = 0;

  char debugStr[128] = {'\0'};
  unsigned int period = 65000;

  //precon: send short low pulse so high pulse is clean/discrete
  LATEbits.LATE3 = 0;
  for(i = 0; i < 320; i++){ Nop(); }  // Pin should already be low, this just verifies by delaying at least 4us (pin toggle time for device)
  
  //start timer
  TMR4CLR = 0xFFFF;
  configureTimer4(period);  //timer 4 configured in 16 bit mode for 10MHz  (1/8 sys clk divider)

  //read current clock val and trigger a ping
  start = ReadTimer4();
  LATEbits.LATE3 = 1;

  // Delay 10 microsec while BIT_3 high. At 80Mhz, 10 microseconds == 800 clocks
  //for(i = 0; !(PORTRead(IOPORT_E) & 0x00000004); i++){ // 267 = 800 / 3, since loop contains three ops per iteration: <, ++, and nop
  for(i = 0; (i < 350) && !PORTEbits.RE2; i++){ // 267 = 800 / 3, since loop contains three ops per iteration: <, ++, and nop
    Nop();
  }

  //should pingClk get stamped here instead, when rising edge of echo detected??? or even below, after wait-for-echo loop
  LATEbits.LATE3 = 0;  // 10us burst complete: drive output low

  //wait for echo segment to begin
  while( !PORTEbits.RE2 && (k < 60000) ){ k++; }  //30000 is arbitrary testing: this gives about 0.75ms, assuming a single op per iter.

  //scale k and j failsafes to minimum according to experimentation (they will likely fluctuate according to echo/distance


  if(k < 60000){

    //polling: echo is high, wait for it to end: may need nested delay here
    while( PORTEbits.RE2 && (j < 65000) ){
      //currVal = ReadTimer4();
      //dt += currVal - lastVal;
      //lastVal = currVal;
      j++;
    }

    if(j < 65000){  //success
       end = ReadTimer4();
       CloseTimer4();
       // This gives cm's; use / (148 * 10) for inches. The '* 10' term is for scaling the timer val into microsecs, as T4 runs at 10MHz
       dist = (end - start) / (58 * 10);
       //sprintf(debugStr,"dist: %dcm dt: x%8X T4: x%08X i: %d j: %d k: %d\n\r",dist,end - start,ReadTimer4(),i,j,k);
       sprintf(debugStr,"dist: %dcm\n\r",dist);
       putsUART1(debugStr);
    }
    else{
      putsUART1("j >= 65000 in ping()\n");
    }
  }
  else{
     putsUART1("k >= 60000 in ping()\n");
  }

  return dist;
}
