const int led = 13; 
const int gate = 32; 
const int cosmic_ray = 0; 

const int delay_time = 10; 

const byte channel = 1; // MIDI channel
const byte note = 60; // MIDI note
const byte velocity = 127; // MIDI velocity

void setup() {
  Serial.begin(57600); 

  pinMode(cosmic_ray, INPUT); 
  pinMode(led, OUTPUT); 
  pinMode(gate, OUTPUT); 
}

void loop() {
  if(digitalReadFast(cosmic_ray) == HIGH) {
    digitalWriteFast(gate, HIGH); 
    digitalWriteFast(led, HIGH); 
    usbMIDI.sendNoteOn(60, 127, 1); 
    Serial.println("ping"); 
    delay(delay_time); 

    digitalWriteFast(gate, LOW); 
    digitalWriteFast(led, LOW); 
    usbMIDI.sendNoteOn(60, 0, 1); 
    delay(delay_time); 
  }
}
