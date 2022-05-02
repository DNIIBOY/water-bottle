const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<style>
.card{
    max-width: 400px;
     min-height: 250px;
     background: #0278b8;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}
</style>
<body>

<div class="card">
  <h4>Den smarteste drikkedunk i byen</h4><br>
  <h1>Water Temp:<br> <span id="TempValue">0</span> &deg;C</h1><br>
</div>
<div class="card">
  <h1>Water Level:<br> <span id="PressureValue">0</span>%</h1><br>
</div>
<div class="card">
  <h1>Time remaining:<br> <span id="TimeValue">0</span></h1><br>
</div>
<script>

setInterval(function() {
  // Call a function repetatively with 2 Second interval
  getTempData();
  getPressureData();
  getTimeData();
}, 1000); //1000mSeconds update rate

function getTimeData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("TimeValue").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "readTime", true);
  xhttp.send();
}

function getTempData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("TempValue").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "readTemp", true);
  xhttp.send();
}

function getPressureData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("PressureValue").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "readPressure", true);
  xhttp.send();
}
</script>
</body>
</html>
)=====";
