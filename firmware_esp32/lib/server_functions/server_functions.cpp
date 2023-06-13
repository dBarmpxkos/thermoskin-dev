#include "server_functions.h"

/* WiFi */
char password[] = "therm0sk1n";

IPAddress localIP(192,168,1,99);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
AsyncWebServer RESTServer(80);

/* API */
const char increasePWM[]      = "/v1/increase";
const char decreasePWM[]      = "/v1/decrease";
const char heatOn[]           = "/v1/heaton";
const char heatOff[]          = "/v1/heatoff";

const char* PARAM_INPUT = "value";
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ThermoSkin</title>

  <meta name="viewport" content="width=device-width, initial-scale=1">

  <style>
    body {
      font-family: Consolas; text-align: center; margin:0px auto; padding-top: 30px;
    }

    .button-1 {
      padding: 10px 20px;
      font-size: 24px;
      text-align: center;
      outline: none;
      color: #fff;
      background-color: #628dd9;
      border: none;
      border-radius: 5px;
      box-shadow: 6px 6px #999;
      cursor: pointer;
      -webkit-touch-callout: none;
      -webkit-user-select: none;
      -khtml-user-select: none;
      -moz-user-select: none;
      -ms-user-select: none;
      user-select: none;
      -webkit-tap-highlight-color: rgba(0,0,0,0);
    }
    .button-1:hover {background-color: #1f2e45}
    .button-1:active {
      background-color: #1f2e45;
      box-shadow: 0 4px #666;
      transform: translateY(2px);
    }

    .button-2 {
      padding: 10px 20px;
      font-size: 24px;
      text-align: center;
      outline: none;
      color: #fff;
      background-color: #30518a;
      border: none;
      border-radius: 5px;
      box-shadow: 6px 6px #999;
      cursor: pointer;
      -webkit-touch-callout: none;
      -webkit-user-select: none;
      -khtml-user-select: none;
      -moz-user-select: none;
      -ms-user-select: none;
      user-select: none;
      -webkit-tap-highlight-color: rgba(0,0,0,0);
    }
    .button-2:hover {background-color: #1f2e45}
    .button-2:active {
      background-color: #1f2e45;
      box-shadow: 0 4px #666;
      transform: translateY(2px);
    }

    .card {
      background: #02b875;
      padding: 1px;
      box-sizing: border-box;
      height: auto;
      width: 305px;
      color: #FFF;
      font-size: 22px;
      margin: 0 auto;
      border-radius: 5px;
      box-shadow: 6px 6px #999;
      float: none;
      margin-bottom: 5px;
    }

    meter {
      width: 350px;
      height: 5px;
    }

    footer {
      text-align: right;
      padding: 15px;
      color: black;
    }


  </style>

</head>
<body>

  <h1>ThermoSkin Web Interface</h1>

  <p style="font-size:24px;">&#x1F50B 97%</p>
  <div class="card">
    <h3>&#127777;&#65039;Temperature: <span id="tempValue">0</span> degC</h3>
  </div>
  <script>


    setInterval(function() {
        getRes();
        getTemp();
        getProgress();
    }, 250);

    function getRes() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("resValue").innerHTML =
          this.responseText;
        }
      };
      xhttp.open("GET", "readResistance", true);
      xhttp.send();
    }

    function getTemp() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("tempValue").innerHTML =
          this.responseText;
        }
      };
      xhttp.open("GET", "readTemp", true);
      xhttp.send();
    }

    function getProgress() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("burn").value =
          this.responseText;
        }
      };
      xhttp.open("GET", "isactive", true);
      xhttp.send();
    }
  </script>

<p>
  <button onclick="toggleHot('onLow')" class="button-1"
    onmousedown="toggleHot('onLow');"
    ontouchstart="toggleHot('onLow');"
    onmouseup="toggleHot('off');"
    ontouchend="toggleHot('off');">
    &#x1F525Medium (3 sec, 55 degC)<br>
  </button>
<p>
  <button onclick="toggleHot('onMid')" class="button-2"
  onmousedown="toggleHot('onMid');"
  ontouchstart="toggleHot('onMid');"
  onmouseup="toggleHot('off');"
  ontouchend="toggleHot('off');">
  &#x1F525&#x1F525High (3 sec, 70 degC)<br></button>

  <script>
   function toggleHot(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/" + x, true);
     xhr.send();
   }
 </script>

<div>
  <meter id="burn" min="0" max="100" low="33" high="66" optimum="80" value="0"></meter>
</div>

</body>

<footer>
  <h6>Resistance: <span id="resValue">0</span> ohm</h6><br>
</footer>

</html>
)rawliteral";

void
setup_AP(char *SSID, char *PWD,
              const IPAddress &softlocalIP, const IPAddress &softGateway, const IPAddress &softSubnet){
    do { /* setup AP */ } while (!WiFi.softAP(SSID, PWD));
    do { /* setup AP */ } while (!WiFi.softAPConfig(softlocalIP, softGateway, softSubnet));
}

void
not_found(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

String
processor(const String& var){
    if (var == "SLIDERVALUE")       return pwmValStr;
    else if (var == "DEVICETEMP")   return String(temperature);
    else if (var == "RESISTANCE")   return String(resistance);
    return String();
}

void
increase_pwm(AsyncWebServerRequest *request){
    pwmValue += 5;
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->addHeader("Server","ThermoSkin");
    response->printf("Increased PWM to: %d", pwmValue);
    request->send(response);
}

void
decrease_pwm(AsyncWebServerRequest *request){
    pwmValue -= 5;
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->addHeader("Server","ThermoSkin");
    response->printf("Decreased PWM to: %d", pwmValue);
    request->send(response);
}

void
activate_heater(AsyncWebServerRequest *request){
    ledcWrite(senseChannel, pwmValue);
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->addHeader("Server","ThermoSkin");
    response->printf("Activated FET");
    request->send(response);
}

void
deactivate_heater(AsyncWebServerRequest *request){
    ledcWrite(senseChannel, 0);
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->addHeader("Server","ThermoSkin");
    response->printf("Deactivated FET");
    request->send(response);
}

void
param_parser(AsyncWebServerRequest *request){
    int paramsNr = request->params();

    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
        Serial.printf("\r\n[HTTP]\tParam: %s, Value: %s",
                      p->name().c_str(), p->value().c_str());

//        if (strcmp(p->name().c_str(), PARAM_INTV) == 0) measureDelay = p->value().toInt();

    }
    request->send(200, "text/plain", "message received");
}

void
setup_endpoints(){
    /* root */
    RESTServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html, processor);
    });
    /* PWM */
    RESTServer.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String inputMessage;
        if (request->hasParam(PARAM_INPUT)) {
            inputMessage = request->getParam(PARAM_INPUT)->value();
            pwmValStr = inputMessage;
        }
        request->send(200, "text/plain", "OK");
    });

    RESTServer.on("/onLow", HTTP_GET, [] (AsyncWebServerRequest *request) {
        hotTicker = millis();
        hotPicker = true;
        lowHeat = true;
        request->send(200, "text/plain", "ok");
    });

    RESTServer.on("/onMid", HTTP_GET, [] (AsyncWebServerRequest *request) {
        hotTicker = millis();
        hotPicker = true;
        medHeat = true;
        request->send(200, "text/plain", "ok");
    });

    RESTServer.on("/off", HTTP_GET, [] (AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "ok");
    });

    RESTServer.on("/readResistance", HTTP_GET, [] (AsyncWebServerRequest *request) {
        char htmlBuff[20] = {'\0'};
        sprintf(htmlBuff, "%.4f", resistance);
        request->send(200, "text/plain", htmlBuff);
    });

    RESTServer.on("/readTemp", HTTP_GET, [] (AsyncWebServerRequest *request) {
        char htmlBuff[20] = {'\0'};
        sprintf(htmlBuff, "%d", (int)temperature);
        request->send(200, "text/plain", htmlBuff);
    });

    RESTServer.on("/isactive", HTTP_GET, [] (AsyncWebServerRequest *request) {
        char htmlBuff[20] = {'\0'};
//        sprintf(htmlBuff, "%d", 1 + ( std::rand() % ( 100 - 1 + 1 ) ));
        sprintf(htmlBuff, "%d", progressBarStatus);
        request->send(200, "text/plain", htmlBuff);
    });

    RESTServer.onNotFound(not_found);
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    RESTServer.begin();

    /*
    RESTServer.on(increasePWM, HTTP_GET, increase_pwm);
    RESTServer.on(decreasePWM, HTTP_GET, decrease_pwm);
    RESTServer.on(heatOn, HTTP_GET, activate_heater);
    RESTServer.on(heatOff, HTTP_GET, deactivate_heater);
    */
}
