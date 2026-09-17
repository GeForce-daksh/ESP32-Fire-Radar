#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <math.h>

Servo radarServo;
WebServer server(80);

const int flamePin = 2;
const int buzzerPin = 0;
const int servoPin = 4;
const int ledPin = 5;

int angle = 0;
int direction = 2;

bool fire = false;
int fireAngle = 0;

const char* ssid = "Fire-Radar";
const char* password = "12345678";

const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Fire Radar</title>

<style>
* {
  box-sizing: border-box;
}

body {
  margin: 0;
  background: #020402;
  color: #39ff66;
  font-family: Arial, sans-serif;
  text-align: center;
}

h2 {
  margin: 15px 0 2px;
  font-size: 24px;
}

#status {
  font-size: 18px;
  margin: 5px;
}

.radar {
  width: 94vw;
  max-width: 520px;
  height: 47vw;
  max-height: 260px;
  margin: 30px auto 8px;
  position: relative;
  overflow: hidden;
  border-radius: 520px 520px 0 0;
  border: 2px solid #208c3a;
  border-bottom: 2px solid #208c3a;

  background:
    radial-gradient(
      circle at 50% 100%,
      transparent 0 24%,
      #155522 24.5% 25%,
      transparent 25.5% 49%,
      #155522 49.5% 50%,
      transparent 50.5% 74%,
      #155522 74.5% 75%,
      transparent 75.5%
    ),
    #061006;
}

.radar::after {
  content: "";
  position: absolute;
  left: 0;
  right: 0;
  bottom: 0;
  height: 2px;
  background: #155522;
}

#scanLine {
  position: absolute;
  width: 50%;
  height: 2px;
  left: 50%;
  bottom: 0;
  background: #39ff66;
  transform-origin: left center;
  box-shadow: 0 0 6px #39ff66, 0 0 15px #39ff66;
}

.center {
  position: absolute;
  width: 10px;
  height: 10px;
  left: calc(50% - 5px);
  bottom: -5px;
  background: #39ff66;
  border-radius: 50%;
  box-shadow: 0 0 12px #39ff66;
}

#fireDot {
  display: none;
  position: absolute;
  width: 20px;
  height: 20px;
  background: red;
  border-radius: 50%;
  transform: translate(-50%, 50%);
  box-shadow: 0 0 8px red, 0 0 18px red, 0 0 30px red;
  animation: fireBlink .5s infinite alternate;
}

#fireText {
  display: none;
  position: absolute;
  color: red;
  font-size: 15px;
  font-weight: bold;
  transform: translate(-50%, 0);
  text-shadow: 0 0 5px red, 0 0 10px red;
}

.locked {
  color: red !important;
  font-weight: bold;
  animation: blink .5s infinite alternate;
}

@keyframes fireBlink {
  from {
    opacity: .45;
  }

  to {
    opacity: 1;
  }
}

@keyframes blink {
  from {
    opacity: .4;
  }

  to {
    opacity: 1;
  }
}

.info {
  color: white;
  font-size: 17px;
}
</style>
</head>

<body>

<h2>FIRE RADAR</h2>

<div id="status">SCANNING...</div>

<div class="radar">

  <div id="scanLine"></div>

  <div class="center"></div>

  <div id="fireDot"></div>

  <div id="fireText">FIRE</div>

</div>

<div class="info">
  ANGLE: <span id="angle">0</span>&deg;
</div>

<script>

function updateRadar(data) {

  let angle = data.angle;
  let fire = data.fire;
  let fireAngle = data.fireAngle;

  document.getElementById("angle").innerText = angle;

  let scanLine = document.getElementById("scanLine");

  scanLine.style.transform =
    "rotate(" + (-180 + angle) + "deg)";

  let dot = document.getElementById("fireDot");
  let fireText = document.getElementById("fireText");
  let status = document.getElementById("status");

  if (fire) {

    let rad = fireAngle * Math.PI / 180;

    let radiusX = 42;
    let radiusY = 78;

    let x =
      50 + radiusX * Math.cos(Math.PI - rad);

    let y =
      100 - radiusY * Math.sin(rad);

    dot.style.left = x + "%";
    dot.style.bottom = (100 - y) + "%";

    fireText.style.left = x + "%";
    fireText.style.bottom = (100 - y + 10) + "%";

    dot.style.display = "block";
    fireText.style.display = "block";

    status.innerHTML = "FIRE DETECTED";
    status.className = "locked";

  } else {

    dot.style.display = "none";
    fireText.style.display = "none";

    status.innerHTML = "SCANNING...";
    status.className = "";
  }
}

setInterval(function() {

  fetch("/data")
    .then(function(response) {
      return response.json();
    })
    .then(function(data) {
      updateRadar(data);
    });

}, 60);

</script>

</body>
</html>
)rawliteral";


void setup()
{
  pinMode(flamePin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  digitalWrite(buzzerPin, HIGH);
  digitalWrite(ledPin, LOW);

  radarServo.attach(servoPin);
  radarServo.write(angle);

  Serial.begin(115200);

  WiFi.softAP(ssid, password);

  Serial.println();
  Serial.println("FIRE RADAR STARTED");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", []()
  {
    server.send(200, "text/html", webpage);
  });

  server.on("/data", []()
  {
    String data = "{";

    data += "\"angle\":";
    data += angle;

    data += ",";

    data += "\"fire\":";
    data += fire ? "true" : "false";

    data += ",";

    data += "\"fireAngle\":";
    data += fireAngle;

    data += "}";

    server.send(200, "application/json", data);
  });

  server.begin();
}


void loop()
{
  server.handleClient();

  int flame = digitalRead(flamePin);

  if (flame == HIGH)
  {
    fire = true;
    fireAngle = angle;

    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, HIGH);

    radarServo.write(fireAngle);

    while (digitalRead(flamePin) == HIGH)
    {
      server.handleClient();
      delay(20);
    }

    fire = false;

    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, LOW);
  }
  else
  {
    fire = false;

    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, LOW);

    radarServo.write(angle);

    angle += direction;

    if (angle >= 180)
    {
      angle = 180;
      direction = -2;
    }

    if (angle <= 0)
    {
      angle = 0;
      direction = 2;
    }

    delay(20);
  }
}