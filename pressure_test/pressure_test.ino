int analogPressure = A0;
int pressureVal = 0;

String input;  // Input for calibration

int mini = 500;
int maxi = 540;


int values[20];
int i = 0;
int avg;  // average value over 20 meassurements
int state = 0;  // How many quarters full the container is

int quarters[4];

void updateQuarters(){
  int halfdiff = (maxi-mini)/2;
  quarters[0] = mini;
  quarters[1] = mini + halfdiff;
  quarters[2] = maxi - halfdiff;
  quarters[3] = maxi;
}

void setup()
{
  Serial.begin(115200);          //  setup serial
  updateQuarters();
}

void loop()
{
  val = analogRead(analogPin);    // read the input pin  
  input = Serial.read();

  if (input == "49"){  // Number 1
    mini = avg;
    updateQuarters();
  }
  if (input == "50"){  // Number 2
    maxi = avg;
    updateQuarters();
  }

  if (avg > quarters[3]){
    state = 3;
  }
  else if (avg > quarters[2]){
    state = 3;
  }
  else if (avg > quarters[1]){
    state = 3;
  }
  else if (avg > quarters[0]){
    state = 0;
  }
  else{
    state = -1;
  }
  
  i = i % 20;
  delay(50);
  if (i<=20){
    values[i] = val;
  }
  i++;
  int sum = 0;
  for (int j = 0; j<20; j++){
    sum += values[j];
  }
  avg = sum/20;
  Serial.print("AVG: ");
  Serial.println(avg);
  Serial.print("State: ");
  Serial.println(state);
}
