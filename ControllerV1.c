/*
 * Javier Martinez Garcia 
 * NOV 2015
 * license GPL
 * READ INSTRUCTION AND MOVE STEPPER MOTORS WITH STEP AND DIR (A4988)
 * ADAPTED TO MY MINI CNC (3 AXIS) USING THE ARDUINO MEGA
 * PLEASE, NOTE THAT THIS (MINICNC) IS NOT 
 * THE TARGET MACHINE AND THE CODE HAS UNUSED
 * VARIABLES AND FUNCTIONS TO BE CLEANED
 *
 * USEFUL LINKS:
 * https://www.youtube.com/watch?v=VHL-ju65lEk
 * http://linuxforanengineer.blogspot.com.es/2015/10/arduino-read-instruction-from-serial.html
 * http://linuxforanengineer.blogspot.com.es/2015/10/arduino-move-4-stepper-motors.html
 * 
 * HAVE FUN!
 */

// STEPPER SCALING
const float scaleMA = 2.0; // (steps/mm)
const float scaleMB = 2.0; // (steps/mm)
const float scaleMC = 2.0; // (steps/mm)
const float scaleMD = 2.0; // (steps/mm)

// PINOUT //////////////////////// PINOUT ///////////////
/* POWER SOURCE PINOUT
PC PSU triggered to ON when pin goes LOW
*/
const int PIN_PSU_POWER = 12;

/*
 * WIRE
 */
const int PIN_WIRE = 0;
int wire_temp = 0;

/* STEPPER PINOUT
MA -> Upper stepper side A
MB -> Lower stepper side A
*/
const int PIN_MA_STEP = 30;
const int PIN_MB_STEP = 32;
const int PIN_MC_STEP = 34;
const int PIN_MD_STEP = 36;
const int PIN_MA_DIR = 31;
const int PIN_MB_DIR = 33;
const int PIN_MC_DIR = 35;
const int PIN_MD_DIR = 37;

/* LIMIT SWITCH PINOUT 
m/M -> min/max
A/B -> side A or B*/
const int PIN_LSW_MXA = 0;
const int PIN_LSW_MYA = 0;
const int PIN_LSW_mXA = 0;
const int PIN_LSW_mYA = 0;
const int PIN_LSW_MXB = 0;
const int PIN_LSW_MYB = 0;
const int PIN_LSW_mXB = 0;
const int PIN_LSW_mYB = 0;

// global variables
const unsigned int STEPPER_HIGH_DELAY = 5000;
const unsigned int STEPPER_LOW_DELAY = 5000;
unsigned int variable_high_delay = 8000;
unsigned int variable_low_delay = 8000;

// stepper_instruction { MA, MB, MC, MD }
int stepper_instruction[4] = { 0, 0, 0, 0 };
int machine_position[4] = { 0, 0, 0, 0 };

void mvStepper()
{
  int x = stepper_instruction[0];
  int y = stepper_instruction[1];
  int z = stepper_instruction[2];
  
  if( x != 0 )
  {
   if( x < 0 )
   {
    digitalWrite( PIN_MA_DIR, HIGH );
   }
   digitalWrite( PIN_MA_STEP, HIGH );
  }
  if( y != 0 )
  {
   if( y < 0 )
   {
    digitalWrite( PIN_MB_DIR, HIGH );
   }
   digitalWrite( PIN_MB_STEP, HIGH );
  }
  if( z != 0 )
  {
   if( z < 0 )
   {
    digitalWrite( PIN_MC_DIR, HIGH );
   }
   digitalWrite( PIN_MC_STEP, HIGH );
  }
  delayMicroseconds( variable_high_delay );
  digitalWrite( PIN_MA_STEP, LOW );
  digitalWrite( PIN_MB_STEP, LOW );
  digitalWrite( PIN_MC_STEP, LOW );
  digitalWrite( PIN_MA_DIR, LOW );
  digitalWrite( PIN_MB_DIR, LOW );
  digitalWrite( PIN_MC_DIR, LOW );
  delayMicroseconds( variable_low_delay );
}
//------------------------------------------------------------------------------
void PathABCD( int Adx, int Ady, int Bdx, int Bdy )
{
  // Trajectory logic
  int delta[4] = { Adx, Ady, Bdx, Bdy };
  int aux = 0;
  for( int i = 0; i < 4; i++ )
  {
    if( abs(delta[i]) > aux )
    {
      aux = abs(delta[i]);
    }
  }

  float R[4] = { 0, 0, 0, 0 };
  for( int i = 0; i < 4; i++ )
  {
    R[i] = (float)delta[i] / (float)aux;
  }

  int inc[4] = { 0, 0, 0, 0 };
  int acc[4] = { 0, 0, 0, 0 };

  for( int j = 1; j < aux+1; j++ )
  {
    // create step instruction
    for( int i = 0; i < 4; i++ )
    {
      inc[i] = round( R[i]*j - acc[i] );
      acc[i] = acc[i] + inc[i];
      stepper_instruction[i] = inc[i];
    }
    // execute step
    mvStepper();
    // erase step instruction
    for( int i = 0; i < 4; i++ )
    {
      stepper_instruction[i] = 0;
    }
  }
}

void setup()
{
  Serial.begin(115200);
  // output
  pinMode( PIN_MA_STEP, OUTPUT); 
  pinMode( PIN_MA_DIR, OUTPUT );
  pinMode( PIN_MB_STEP, OUTPUT );
  pinMode( PIN_MB_DIR, OUTPUT );
  pinMode( PIN_MC_STEP, OUTPUT );
  pinMode( PIN_MC_DIR, OUTPUT );
  pinMode( PIN_PSU_POWER, OUTPUT );
  digitalWrite( PIN_PSU_POWER, HIGH );
  pinMode( 10, OUTPUT );
  analogWrite( 10, 0 );
}
String complete_instruction[6];  // will contain the decoded instruction
bool INIT = false;
int machine_global_position[4] = { 0, 0, 0, 0 };
void loop()
{ 
  while(!Serial.available()) {}  // if there is nothing on serial, do nothing
  int  i = 0;
  char raw_instruction[25];
  while (Serial.available())
  {  // if something comes from serial, read it and store it in raw_instruction char array
    delay(10); // delay to allow buffer to fill
    if (Serial.available() > 0)
    {
      raw_instruction[i] = Serial.read();
      i++;
    }
  }
  if( strlen( raw_instruction ) > 0 )  // if a new raw_instruction has been read
  {
    // clean raw_instruction before decoding (overwrite non filled array positions with empty spaces)
    for( int n = i; n < 25; n++ ) { raw_instruction[n] = ' '; }
    // decode the instruction (4 fields) (iterator n = field, iterator j = character)
    int j = 0;
    for( int n = 0; n < 5; n++ )
    { 
      while( j < 25 )
      {
        if( raw_instruction[j] == ' ' )
        {
          j++;
          break;
        }
        else
        {
          complete_instruction[n] += raw_instruction[j];
        }
        j++;
      }
    }
    // print decoded instruction by serial
    if( complete_instruction[0] == "INIT" )
    {
      INIT = true; // start reading program
      Serial.println( 0 );
    }
    if( INIT == true )
    {
     // for debug Serial.println( complete_instruction[0] );
      if( complete_instruction[0] == "POWER" )
      {
        if( complete_instruction[1] == "ON" )
        {
         digitalWrite( PIN_PSU_POWER, LOW ); 
         Serial.println( 0 );
        }
        else
        { 
          digitalWrite( PIN_PSU_POWER, HIGH );
          Serial.println( 0 );
        }
      }
      if( complete_instruction[0] == "FEED" )
      {
        variable_high_delay = variable_high_delay + complete_instruction[1].toInt();
        variable_low_delay = variable_low_delay + complete_instruction[2].toInt();
        Serial.println( 0 );
      }
      if( complete_instruction[0] == "SPIN" )
      {
        analogWrite( 10, complete_instruction[1].toInt() );
        Serial.println( 0 );
      }
      if( complete_instruction[0] == "WIRE" )
      {
        wire_temp = complete_instruction[1].toInt();
        Serial.print( "Wire temperature set to: " );
        Serial.println( wire_temp );
        Serial.println( 0 );
      }
      if( complete_instruction[0] == "MOVE" )
      {
        int stepsMA = round( complete_instruction[1].toFloat()*scaleMA );
        int stepsMB = round( complete_instruction[2].toFloat()*scaleMB );
        int stepsMC = round( complete_instruction[3].toFloat()*scaleMC );
        int stepsMD = round( complete_instruction[4].toFloat()*scaleMD );
        PathABCD( stepsMA, stepsMB, stepsMC, stepsMD );
        Serial.println( 0 );
      }
      if( complete_instruction[0] == "END" )
      {
        INIT = false;
        Serial.println( 0 );
      }
    }
    for( int i = 0; i < 5; i++ )
    {
      complete_instruction[i] = "";
    }
  }
  delay(10);
}





