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
    body { font-family: Consolas; text-align: center; margin:0px auto; padding-top: 30px;}
    .button-1 {
      padding: 10px 20px;
      font-size: 24px;
      text-align: center;
      outline: none;
      color: #fff;
      background-color: #2f4468;
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
  </style>
</head>
<body>

  <h1>ThermoSkin Web Interface</h1>
  <button onclick="load_it()" class="button-1"  onmousedown="toggleCheckbox('on');"
  ontouchstart="toggleCheckbox('on');"
  onmouseup="toggleCheckbox('off');"
  ontouchend="toggleCheckbox('off');">
Activate!&#x1F525</button>

<script>
  function load_it(){
    const barInterval = setInterval(function(){
    var z = document.getElementById('burn');
    z.value = z.value+1;
    if (z.value == 100) {
      z.value = 0;
      clearInterval(barInterval);
    }
  }, 25);
  }
</script>

<p>
  <meter id="burn"
  min="0" max="100"
  low="33" high="66" optimum="80"
  value="0">
</meter>

<script>
 function toggleCheckbox(x) {
   var xhr = new XMLHttpRequest();
   xhr.open("GET", "/" + x, true);
   xhr.send();
 }
</script>

</body>
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
    else if (var == "TEMPERATURE")  return String(roomTemp);
    else if (var == "DEVICETEMP")   return String(deviceTemp);
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
    ledcWrite(ledChannel, pwmValue);
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->addHeader("Server","ThermoSkin");
    response->printf("Activated FET");
    request->send(response);
}

void
deactivate_heater(AsyncWebServerRequest *request){
    ledcWrite(ledChannel, 0);
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
    /* room temperature endpoint */
    RESTServer.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
        char resBuf[20] = {'\0'};
        dtostrf(roomTemp, 2, 3, resBuf);
        request->send(200, "text/plain", resBuf);
    });
    /* device temperature endpoint */
    RESTServer.on("/deviceTemp", HTTP_GET, [](AsyncWebServerRequest *request){
        char resBuf[20] = {'\0'};
        dtostrf(deviceTemp, 2, 3, resBuf);
        request->send(200, "text/plain", resBuf);
    });
    /* resistance endpoint */
    RESTServer.on("/resistance", HTTP_GET, [](AsyncWebServerRequest *request){
        char resBuf[20] = {'\0'};
        dtostrf(resistance, 2, 4, resBuf);
        request->send(200, "text/plain", resBuf);
    });

    /* demo on/off */
    RESTServer.on("/on", HTTP_GET, [] (AsyncWebServerRequest *request) {
        wasPressed = true;
        request->send(200, "text/plain", "ok");
    });

    /* demo on/off */
    RESTServer.on("/off", HTTP_GET, [] (AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "ok");
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
