#include "server_functions.h"

/* WiFi */
char password[] = "therm0sk1n";

IPAddress localIP(192,168,1,99);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
AsyncWebServer RESTServer(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ThermoSkin</title>

  <meta name="viewport" content="width=device-width, initial-scale=1">

  <style>
    body {
      font-family: Consolas; text-align: center; margin:0px auto; padding-top: 30px;
    }

    .card_measure {
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
    .card_batt {
      background: #628dd9;
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
    .card_temp {
      background: #BDB76B;
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


  </style>

</head>
<body>

  <h2>ThermoSkin Web Interface</h2>

  <div class="card_measure">
    <h3>&#x23F1; Counter: <span id="countValue">0</span> times</h3>
  </div>

  <div class="card_batt">
    <h3>&#x1F50B; Battery: <span id="batValue">0</span> V</h3>
  </div>

  <div class="card_temp">
    <h3>&#x1F525; Temperature: <span id="tempValue">0</span> C</h3>
  </div>

  <script>

    setInterval(function() {
        getCount();
        getBatt();
        getTemp();
    }, 250);

    function getCount() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("countValue").innerHTML =
          this.responseText;
        }
      };
      xhttp.open("GET", "counts", true);
      xhttp.send();
    }

    function getBatt() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("batValue").innerHTML =
          this.responseText;
        }
      };
      xhttp.open("GET", "batlevel", true);
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
      xhttp.open("GET", "temperature", true);
      xhttp.send();
    }

  </script>

</body>

</html>
)rawliteral";

void
setup_AP(char *SSID, char *PWD,
         const IPAddress &softlocalIP,
         const IPAddress &softGateway,
         const IPAddress &softSubnet){
    do { /* setup AP */ } while (!WiFi.softAP(SSID, PWD));
    do { /* setup AP */ } while (!WiFi.softAPConfig(softlocalIP, softGateway, softSubnet));
}

void
not_found(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void
setup_endpoints(){
    /* root */
    RESTServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html);
    });

    RESTServer.on("/counts", HTTP_GET, [] (AsyncWebServerRequest *request) {
        char htmlBuff[20] = {'\0'};
        sprintf(htmlBuff, "%d", touchCounter);
        request->send(200, "text/plain", htmlBuff);
    });

    RESTServer.on("/batlevel", HTTP_GET, [] (AsyncWebServerRequest *request) {
        char htmlBuff[20] = {'\0'};
        sprintf(htmlBuff, "%.2f", batLevel);
        request->send(200, "text/plain", htmlBuff);
    });

    RESTServer.on("/temperature", HTTP_GET, [] (AsyncWebServerRequest *request) {
        char htmlBuff[20] = {'\0'};
        sprintf(htmlBuff, "%.2f", temperature);
        request->send(200, "text/plain", htmlBuff);
    });

    RESTServer.onNotFound(not_found);
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    RESTServer.begin();

}
