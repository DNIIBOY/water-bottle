int analogPin = A0;     // potentiometer wiper (middle terminal) connected to analog pin 3
                       // outside leads to ground and +5V
int val = 0;           // variable to store the value read

void setup()
{
  Serial.begin(115200);          //  setup serial
}

int values[20];
int i = 0;

void loop()
{
  val = analogRead(analogPin);    // read the input pin
  Serial.print("Val: ");
  Serial.println(val);             // debug value
  delay(400);
  if (i<=20){
    values[i] = val;
  }
  i++;
  int sum = 0;
  for (int j = 0; j<20; j++){
    sum += values[j];
  }
  Serial.print("AVG: ");
  Serial.println(sum/20);
}
