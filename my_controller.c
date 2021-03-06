#include "my_controller.h"

void init(STATE *s)
{
  s->motorDir = MOTOR_FORWARD;  //a 1 (FORWARD) or a zero (REVERSE)
  s->nextDC = DC_50PCT;
  s->lastDC = DC_50PCT;
  s->rightDC = DC_50PCT;
  s->leftDC = DC_50PCT;
  s->cur  = 0;
  s->last = 2;
  s->goalAccX = 0;  //init this at startup, holding the robot upright. all vals will be relative to it.
  s->goalRoll = 0; //should always be zero, unless accelerating forward
  clrImuVector( &(s->sensors[0]) );
  clrImuVector( &(s->sensors[1]) );
  clrImuVector( &(s->sensors[2]) );
}

void updateState(STATE* st)
{
  staticBalance(st);  //updating state may have one or many goals, even though just one for now
}

// abs function for signed short int values
short int shortAbs(short int value)
{
  unsigned short int temp = value >> 15;     // make a mask of the sign bit

  value ^= temp;                   // toggle the bits if value is negative
  value += temp & 1;               // add one if value was negative

  return value;
}

/*
  Actual output of these motors is non-linear, so this is an attempt to
  map the DC output value onto the current accelerometer X value (yes, in that direction).

  120 OC (DC_50PCT) is about the slowest the motors will go with meaningful torque (ie, they turn constantly w/out a push).
  215 OC (DC_80PCT) is about the fastest.



*/
short int linearizeAccX(short int accX)
{
  return accX + OC_LINEAR_CONSTANT;
}


/*
  Implements a pid-based static balance mechanism.

  Goal is to match both current rate of change and current proportional term (absolute difference from goal) to next motor DC.

  accX     : the proportional term, or absolute difference from the goal of zero (static balance) 
  d_accX/dt: the rate of change of the accelerometer, measured wrt last three data points.

  DC       : the proportional term of the motors 
  d_DC/dt  : the rate of change of motor DC

  The goal really is to unify the DC state with the acc state

  It may be possible to eliminate the dt term, since we assume this function is called in strictly regular intervals. Else,
  we need to grab a timer value and perform the division.

  CHECK ALL CODE PATHS ACCORDING TO INTEGER SIGN

*/
short int doPID(STATE* st)
{
  short int newDC, d_accX, d_DC;
  short int cal_accX = shortAbs(st->sensors[st->cur].accX + ACCX_OFFSET);

  //char dbg[64];

  // if d_accX small, angular velocity is slowing
  //very noisy
  d_accX = st->sensors[st->cur].accX - st->sensors[st->last].accX;     //how to distance these? should dist be 1 or 2?

  // provides feedback wrt the rate at which adjustments to DC have been made. while dangerously recursive, this can provide a measure of past success
  // also, consider that d_DC will always be very low while motors are beginning to move, although voltage to them is high
  //d_DC = st->sensors[0].lastDC - st->sensors[2].nextDC;

  //compare d_accX and d_DC here to measure the efficacy of pid's past effectiveness. This way, we could calibrate current approximation to match reality
  //for instance, add some value propoertional to the difference between adjustments and efficacy.


  /*
    It would be best to map current state directly to action, and not use lastDC in this expression since it is a big source of error/overcorrection
    newDX = lastDC + proportional - derivative  [ +/- integral ?? ]
    Be mindful of when this goes to zero: when (accX < Y1) AND (d_accX < Y2)

    I subtract the derivative term for two cases:
       d_Accx << 0 : We're moving away from goal, so d_AccX should contribute more to DC
       d_Accx >> 0 : We're moving (too?) rapidly toward goal, so d_AccX should dampen proportional gain

    Otherwise the derivative term helps to compensate for environmental factors, such as actual motor performance and voltage, etc.
  */
  //newDC = (accX / Y1 - d_accX / Y2) % 256;  
  newDC = (cal_accX * PROPORTIONAL_GAIN - d_accX * DERIVATIVE_GAIN + OC_LINEAR_CONSTANT) % PR_BASE;

  //duty cycle can go negative in above calculation if d_Accx wants to dampen more than DC will allow (it anticipates motor chdir)
  if(newDC <= 0){
    return 0;
  }

  //sprintf(dbg,"nxtDC %5d accX %4d d_accX %2d\n\0", newDC, d_accX, st->sensors[st->cur].accX);
  //putsUART1(dbg);

  return newDC;
}

/*
  Motor direction update state machine.  This may require a bit more complexity.
*/
void updateMotorDir(STATE* st, short int cal_X)
{
  short int motorDir = 0;

  /*
    Map current accX directly to ideal motorDir. Adding readings n, n-1, and n-2 is a small method of smoothing 
    any noisy vals that throw the sign bit. If majority are some sign, then maintain current dir.
  */
  motorDir = ( 3 * ACCX_OFFSET + st->sensors[st->cur].accX + st->sensors[((st->cur+1)%3)].accX + st->sensors[st->last].accX)  & 0x8000;  //get the sign bit: lean forward -> go forward   lean backward -> go backward

  if(motorDir ^ st->motorDir){  //detect state change
    
    if(cal_X > ACCX_SOFT_THRESHOLD){  //only change direction state if accX is some threshold above/below horizon, so we don't switch direciton rapidly
      st->motorDir = motorDir & 0x8000;  //set motor direction to opposite of current accelerometer sign val
      //putsUART1("switch mtrs:  ");
      LATBINV = 0x1800;
      //(motorDir == FORWARD) ? putsUART1("FORWARD\n") : putsUART1("REVERSE\n");
      switchMotors(st->motorDir);
    }
  }
}



/*
  IMU structs contain short int vals.

  Also, it is very likely that a balanced robot will not mean X == 0 !!
  We need some way to calibrate this factor later.

  DC ranges from 0 - 255 (see my_motors.h)

  !!accelerometer and gyro sensors must be oriented in exactly the same direction as one another
  
  Simple equation for duty cycle:
    DC_next = (1 + accX / y1 - gyroX / y2 ) * DC_cur

  This eqn applies regardless of the sign of accX or gyroX, so mod them to get positive vals.
  Some other function will handle the motor direction state machine

  Also need to verify that in 

*/
void staticBalance(STATE* st)
{
  short int cal_x, dx;
  //float x = 0.0, dx = 0;
  char dbg[128];

  //abs the vals so DC update occurs w/out wrt motor direction   
  cal_x = shortAbs(st->sensors[st->cur].accX + ACCX_OFFSET);
  //dx = shortAbs(st->sensors[0].gyroRoll);  //don't abs dx, since it helps 
 
  updateMotorDir(st,cal_x);
 
  //DC ranges from 0 to 255. x ranges -330-255, but only the -30-30 range is operational 
  //this is recursive; not yet clear if recursive solution will operate too fast/slow, over-correct, etc.
  //for instance, if we pass through the threshold, switch dir, then base next vals on the last DC val from before motor switch
  
  //the greedy hill climbing strategy; concept is still good, but this version breaks easily (especially if x ever goes to zero)
  // ALSO, remember switching motors sends DC to ZERO as well
  //st->lastDC = st->nextDC;
  //st->nextDC = st->lastDC * (1 + x / Y1 );// - dx / Y2 );  //should changing DC use multiplicaton or addition?? -->
  if(cal_x > FALL_THRESHOLD){  // the "oh crap!" range--no calculation required
    st->lastDC = st->nextDC;
    st->nextDC = DC_80PCT;
  }
  else{  // new state = updateState(state)

    //optimization: update DC at lower resolution within goal region. This is risky if we are passing through the region very fast (high d_accX)
    if((cal_x < ACCX_SOFT_THRESHOLD) && ((cal_x % 3) == 0)){
      st->nextDC = st->lastDC;
    }
    else{
      st->lastDC = st->nextDC;
      st->nextDC = doPID(st);
      //st->nextDC = (st->lastDC + x / 10 ) % DC_80PCT;// - dx / Y2 );  //should changing DC use multiplicaton or addition?? -->
    }
  }

  actions(st);
/*
  sprintf(dbg,"DC %3d accX %4d d_accX %2d dir: %c\n\0", st->nextDC, st->sensors[st->cur].accX, st->sensors[st->cur].accX - st->sensors[st->last].accX, (st->motorDir == 0 ? 'R' : 'F'));
  //sprintf(dbg,"nxtDC %5d accX %4d d_accX %d gRoll %8d\n\0", st->nextDC, st->sensors[st->cur].accX, st->sensors[st->cur].accX - st->sensors[st->last].accX, st->sensors[0].gyroRoll);
  taskENTER_CRITICAL();
  putsUART1(dbg);
  taskEXIT_CRITICAL();
*/
}

//after updating internal state, update external state (actions)
void actions(STATE* st)
{
  writeOC2(st->nextDC,st->nextDC);
  writeOC3(st->nextDC,st->nextDC);
}


