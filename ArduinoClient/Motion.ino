volatile boolean next_move_prepared = false;
volatile boolean prepare_shaddow_registers = false;

void startMotion() {
  in_motion = true;
  next_move_prepared=false; //TODO in theory this is not needed  
  prepare_shaddow_registers = false;
  //TODO initialize drivers??
}

void stopMotion() {
  in_motion = false;
}

void checkMotion() {
  if (in_motion && !next_move_prepared) {
    if (moveQueue.count()>0) {
      //analysze the movement (nad take a look at the next
      movement move = moveQueue.pop();
      movement followers[MAX_FOLLOWING_MOTORS];
#ifdef DEBUG_MOTION
      Serial.print(F("Moving motor "));
      Serial.print(move.motor,DEC);
      Serial.print(F(" to "));
      Serial.print(move.data.move.target);
      Serial.print(F(" at "));
      Serial.print(move.data.move.vmax);
#endif
      moveMotor(move.motor, move.data.move.target,move.data.move.vmax, 1, prepare_shaddow_registers);
      //TODO configure the motor so that it - uhm - moves
      //check out which momtors are geared with this
      int following_motors_count = 0;
      do {
        followers[following_motors_count] = moveQueue.peek();
        if (followers[following_motors_count].type==followmotor) {  
          moveQueue.pop();
          following_motors_count++;
        }
        moveMotor(followers[following_motors_count].motor, followers[following_motors_count].data.follow.target,move.data.move.vmax, followers[following_motors_count].data.follow.factor, prepare_shaddow_registers);
      } 
      while (followers[following_motors_count].type == followmotor);

      byte following_motors=0;
      for (char i=0;i<following_motors_count;i++) {
#ifdef DEBUG_MOTION
        Serial.print(F(", following motor "));
        Serial.print(followers[i].motor,DEC);
        Serial.print(F(" by "));
        Serial.print(followers[i].data.follow.factor,DEC);
#endif
        char following_motor = followers[i].motor;
        //all motors mentioned here are configured
        float follow_factor = followers[i].data.follow.factor;
        //TODO compute and write all the values for amax/dmax/bow1-4
        following_motors |= _BV(i);
      }
      for (char i; i<nr_of_motors;i++) {
        if (following_motors && _BV(i) == 0) {
          //TODO configure other to stop
        }
      }
      //finally configure the running motor

      boolean send_start=false;
      prepare_shaddow_registers = true;  
      next_move_prepared = true;

      //register the interrupt handler for this motor
      // attachInterrupt(motors[moved_motor].target_reached_interrupt_nr , target_reached_handler, RISING);

      if (send_start) {
        //and carefully trigger the start pin 
        digitalWrite(start_signal_pin,HIGH);
        pinMode(start_signal_pin,OUTPUT);
        digitalWrite(start_signal_pin,LOW);
        pinMode(start_signal_pin,INPUT);
      }
    } 
    else {
      //we are finished here
    }
  }
}


void target_reached_handler() {
  next_move_prepared=false;
}





































