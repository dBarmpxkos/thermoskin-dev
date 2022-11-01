#include "server_functions.h"
#include "lcd_handling.h"

/* WiFi */
char ssid[] = "thermoSkin";
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

String sliderValue = "0";
const char* PARAM_INPUT = "value";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ThermoSkin</title>
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.3rem;}
    p {font-size: 1.9rem;}
    body {max-width: 400px; margin:0px auto; padding-bottom: 25px;}
    .slider { -webkit-appearance: none; margin: 14px; width: 360px; height: 25px; background: #FFD65C;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #003249; cursor: pointer;}
    .slider::-moz-range-thumb { width: 35px; height: 35px; background: #003249; cursor: pointer; }
  </style>
</head>
<body>
  <h2>ThermoSkin Web Interface</h2>
  <p><span id="textSliderValue">%SLIDERVALUE%</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="pwmSlider" min="0" max="255" value="%SLIDERVALUE%" step="1" class="slider"></p>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i>
    <span class="dht-labels">Temperature</span>
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i>
    <span class="dht-labels">Target</span>
    <span id="temperature">%TARGET%</span>
    <sup class="units">&deg;C</sup>
  </p>
<script>
function updateSliderPWM(element) {
  var sliderValue = document.getElementById("pwmSlider").value;
  document.getElementById("textSliderValue").innerHTML = sliderValue;
  console.log(sliderValue);
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider?value="+sliderValue, true);
  xhr.send();
}

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 1000 ) ;


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
    if (var == "SLIDERVALUE")      return sliderValue;
    else if (var == "TEMPERATURE") return String(runningTemp);
    else if (var == "TARGET")      return String(targetTemp);
    return String();
}

void
increase_pwm(AsyncWebServerRequest *request){
    pwmValue += 5;
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->addHeader("Server","ThermoSkin");
    response->printf("Increased PWM to: %d", pwmValue);
    request->send(response);
    lcd_go_and_print_value(display, "PWM Inc: ", pwmValue);
}

void
decrease_pwm(AsyncWebServerRequest *request){
    pwmValue -= 5;
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->addHeader("Server","ThermoSkin");
    response->printf("Decreased PWM to: %d", pwmValue);
    request->send(response);
    lcd_go_and_print_value(display, "PWM Dec: ", pwmValue);
}

void
activate_heater(AsyncWebServerRequest *request){
    ledcWrite(ledChannel, pwmValue);
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->addHeader("Server","ThermoSkin");
    response->printf("Activated FET");
    request->send(response);
    lcd_go_and_print(display, "Heater: ON");
}

void
deactivate_heater(AsyncWebServerRequest *request){
    ledcWrite(ledChannel, 0);
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->addHeader("Server","ThermoSkin");
    response->printf("Deactivated FET");
    request->send(response);
    lcd_go_and_print(display, "Heater: OFF");
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
    RESTServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html, processor);
    });
    RESTServer.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String inputMessage;
        if (request->hasParam(PARAM_INPUT)) {
            inputMessage = request->getParam(PARAM_INPUT)->value();
            sliderValue = inputMessage;
            ledcWrite(ledChannel, sliderValue.toInt());
        }
        else {
            inputMessage = "No message sent";
        }
        Serial.println(inputMessage);
        request->send(200, "text/plain", "OK");
    });
    RESTServer.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", String(runningTemp).c_str());
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
