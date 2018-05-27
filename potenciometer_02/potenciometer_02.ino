
String incomingByte;   // for incoming serial data
const int SETTINGS_UD         = D5; // Settings Up/Down pulses
const int SETTINGS_CS_A1      = D3; // CS - actuator 1

const int ACTUATOR_MAX_STEPS  = 64; // MCP4011 ofers 64 steps
const int ACTUATOR_STEP_WIDTH = 50; // 50 ms pulse width

static int settings_pos_a1     = -1;

void setup() {
 Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
  pinMode(SETTINGS_UD, OUTPUT);
  pinMode(SETTINGS_CS_A1, OUTPUT);

  digitalWrite(SETTINGS_UD, LOW);
  digitalWrite(SETTINGS_CS_A1, HIGH);
}

void loop() {
        // send data only when you receive data:
        if (Serial.available() > 0) {
                // read the incoming byte:
                
                incomingByte = Serial.readString();
                int actuator = (incomingByte.substring(0,1)).toInt();
                int pos =  incomingByte.substring(2).toInt();
               
                   Serial.print("act: ");
                   Serial.println(actuator);
                   Serial.print("pos: ");
                   Serial.println(pos);


//                   /////////////////////////////////////////////////////////////////////////////////

  int settings_cs;
  int settings_pos = -1;

  settings_cs = SETTINGS_CS_A1;
  settings_pos = settings_pos_a1;


// Deactivate all actuators:
  digitalWrite(SETTINGS_CS_A1, HIGH);
//  digitalWrite(SETTINGS_CS_A2, HIGH);

// Uninitialized! Move wiper position to zero:
  if(settings_pos == -1)
  {
    digitalWrite(SETTINGS_UD, LOW); // Decrementing
    delay(200);

    digitalWrite(settings_cs, LOW); // Select actiator
    delay(200);

    for(int i = 0; i < ACTUATOR_MAX_STEPS; i++)
    {
      digitalWrite(settings_cs, HIGH);
      delay(ACTUATOR_STEP_WIDTH);
      digitalWrite(settings_cs, LOW);
      delay(ACTUATOR_STEP_WIDTH);
    }

    delay(200);
    digitalWrite(settings_cs, HIGH); // Deselect actiator
    delay(200);
  }

// Put wiper positon on place:
  delay(50);

// Decrementing:
  int steps = settings_pos - pos;

  if(steps > 0)
  {
    digitalWrite(SETTINGS_UD, LOW); // Decrementing
    delay(200);
  }else{
    digitalWrite(SETTINGS_UD, HIGH); // Incrementing
    delay(200);
  }

// Move position:
  digitalWrite(settings_cs, LOW); // Select actiator
  delay(200);
  digitalWrite(SETTINGS_UD, LOW); // Set ready
  delay(100);

  for(int i = 0; i < abs(steps); i++)
  {
    digitalWrite(SETTINGS_UD, HIGH);
    delay(ACTUATOR_STEP_WIDTH);
    digitalWrite(SETTINGS_UD, LOW);
    delay(ACTUATOR_STEP_WIDTH);
     Serial.println(i);
  }

  delay(100);
  digitalWrite(settings_cs, HIGH); // Deselect actiator
  delay(50);

// Set current actuator position:

    settings_pos_a1 = pos;
    Serial.println(pos);
        }
}
