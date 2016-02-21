//Radio Events
void onIncomingMessageEvent(const MyMessage &message) {
  
  Serial.println(message.sender);
  Serial.println(message.sensor);
  Serial.println(message.type);
  
  if (message.sensor == BOULDER_ID) {
    if (message.type == V_VAR1) {
      if (message.getInt() == 0) {
        mLeds[0] = CRGB::Yellow;
      } 
      if(message.getInt() == 1) {
        mLeds[0] = CRGB::Blue;
      }
      if(message.getInt() == 2) {
        mLeds[0] = CRGB::Red;
      }
    }
  }
}

int ledPin = 13;



void setup() {

  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);


}

void loop()
{

}







