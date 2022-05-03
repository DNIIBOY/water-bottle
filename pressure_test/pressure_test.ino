#define analogPin A4
int pressureVal = 0;

String input;  // Input for calibration

int mini = 505;
int maxi = 535;

int avgCount = 20;

int values[20];
int i = 0;
int avg;  // average value over 20 meassurements
int state = 0;  // How many quarters full the container is
int val;

int quarters[4];

void updateQuarters(){
  int halfdiff = (maxi-mini)/2;
  quarters[0] = mini;
  quarters[1] = mini + halfdiff;
  quarters[2] = maxi - halfdiff;
  quarters[3] = maxi;
}

int updateAvgVal(){
  i = i % avgCount;
  values[i] = val;
  i++;
  
  int sum = 0;
  for (int j = 0; j<avgCount; j++){
    sum += values[j];
  }
  return sum/avgCount; 
}

int updatePressureState(){
  int state;
  if (avg > quarters[3]){
    state = 3;
  }
  else if (avg > quarters[2]){
    state = 2;
  }
  else if (avg > quarters[1]){
    state = 1;
  }
  else if (avg > quarters[0]){
    state = 0;
  }
  else{
    state = -1;
  }
  return state;
}

void setup()
{
  Serial.begin(115200);          //  setup serial
  updateQuarters();
}

void loop()
{
  val = analogRead(analogPin);    // read the input pin  
  avg = updateAvgVal();
  state = updatePressureState();

  Serial.print("AVG: ");
  Serial.println(avg);
  Serial.print("State: ");
  Serial.println(state);
  delay(50);
}
