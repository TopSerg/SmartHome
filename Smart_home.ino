//Подключение библиотек
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

//Настройки сети
#ifndef STASSID
#define STASSID "421"
#define STAPSK  "111BablTop"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80); // включение сервера на порту:80

DHT dht(13, DHT11);  // сообщаем на каком порту будет датчик
LiquidCrystal_I2C lcd(0x3F,16,2);   // Устанавливаем дисплей

//Необходимые пины для светодиодов
const int redPin = 16;
const int greenPin = 0;
const int bluePin = 2;

uint8_t LED1pin = 12;
bool LED1status = HIGH;

uint8_t LED2pin = 14;
bool LED2status = LOW;

//Считывание температуры
String readTemperature() {
  // Считываем температуру в Цельсиях
  float t = dht.readTemperature();
  // Перевод в Фаренгейты для извращенцев
  //t = 1.8 * t + 32;
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT11 sensor!");
    return "";
  }
  else {
    return String(t);
  }
}

//Считывание давления
String readHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT11 sensor!");
    return "";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}

//Some shit
byte mac[6];              

int i = 0, j = 0;
String str1, str2;
String color1;


//Ответ сервера на различные запросы
void handle_OnConnect() 
{ 
  Serial.print("GPIO7 Status: ");
  if(LED1status)
    Serial.print("ON");
  else
    Serial.print("OFF");

  Serial.print(" | GPIO6 Status: ");
  if(LED2status)
    Serial.print("ON");
  else
    Serial.print("OFF");

  Serial.println("");
  server.send(200, "text/html", SendHTML(LED1status,LED2status)); 
}


void handle_led1on() 
{
  LED1status = LOW;
  Serial.println("GPIO7 Status: ON");
  server.send(200, "text/html", SendHTML(true,LED2status)); 
}

void handle_led1off() 
{
  LED1status = HIGH;
  Serial.println("GPIO7 Status: OFF");
  server.send(200, "text/html", SendHTML(false,LED2status)); 
}

void handle_led2on() 
{
  LED2status = HIGH;
  Serial.println("GPIO6 Status: ON");
  server.send(200, "text/html", SendHTML(LED1status,true)); 
}

void handle_led2off() 
{
  LED2status = LOW;
  Serial.println("GPIO6 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status,false)); 
}

void handle_temperature()
{
  server.send(200, "text/html", readTemperature().c_str());
}

void handle_humidity()
{
  server.send(200, "text/html", readHumidity().c_str());
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}

//Вывод отладочной подсветки
void Led3(String color){
  if(color.equals("RED")){
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, HIGH); 
    digitalWrite(bluePin, LOW);
  }
  else if(color.equals("BLUE")){
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, LOW);
    digitalWrite(bluePin, HIGH);
  }
  else if(color.equals("GREEN")){
    digitalWrite(greenPin, HIGH);
    digitalWrite(redPin, LOW);
    digitalWrite(bluePin, LOW);
  }
  else if(color.equals("PURPLE")){
    digitalWrite(greenPin, HIGH);
    digitalWrite(redPin, LOW);
    digitalWrite(bluePin, HIGH);
  }
  else if(color.equals("YELLOW")){
    digitalWrite(greenPin, HIGH);
    digitalWrite(redPin, HIGH);
    digitalWrite(bluePin, LOW);
  }
  else if(color.equals("WBLUE")){
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, HIGH);
    digitalWrite(bluePin, HIGH);
  }
  else if(color.equals("BLACK")){
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, LOW);
    digitalWrite(bluePin, LOW);
  }
  else{
    Serial.println("АХТУНГ САМ ЩИТ АТ ЗЕ ФАНКШН");
  }
}

String SendHTML(uint8_t led1stat,uint8_t led2stat)
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<script src=\"https://code.highcharts.com/highcharts.js\"></script>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP8266 Web Server</h1>\n";
  ptr +="<h3>Using Access Point(AP) Mode</h3>\n";
  
  if(led1stat)
    ptr +="<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";
  else
    ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";

  if(led2stat)
    ptr +="<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";
  else
    ptr +="<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";

  ptr +="<div id=\"chart-temperature\" class=\"container\"></div>\n";
  ptr +="<div id=\"chart-humidity\" class=\"container\"></div>\n";
  ptr +="</body>\n\n";
  ptr +="<script>\n";
  ptr +="var chartT = new Highcharts.Chart({\n";
  ptr +="  chart:{ renderTo : 'chart-temperature' },\n";
  ptr +="  title: { text: 'BME280 Temperature' },\n";
  ptr +="  series: [{\n";
  ptr +="    showInLegend: false,\n";
  ptr +="    data: []\n";
  ptr +="  }],\n";
  ptr +="  plotOptions: {\n";
  ptr +="    line: { animation: false,\n";
  ptr +="      dataLabels: { enabled: true }\n";
  ptr +="    },\n";
  ptr +="    series: { color: '#059e8a' }\n";
  ptr +="  },\n";
  ptr +="  xAxis: { type: 'datetime',\n";
  ptr +="    dateTimeLabelFormats: { second: '%H:%M:%S' }\n";
  ptr +="  },\n";
  ptr +="  yAxis: {\n";
  ptr +="    title: { text: 'Temperature (Celsius)' }\n";
  ptr +="    //title: { text: 'Temperature (Fahrenheit)' }\n";
  ptr +="  },\n";
  ptr +="  credits: { enabled: false }\n";
  ptr +="});\n";
  ptr +="setInterval(function ( ) {\n";
  ptr +="  var xhttp = new XMLHttpRequest();\n";
  ptr +="  xhttp.onreadystatechange = function() {\n";
  ptr +="    if (this.readyState == 4 && this.status == 200) {\n";
  ptr +="      var x = (new Date()).getTime(),\n";
  ptr +="          y = parseFloat(this.responseText);\n";
  ptr +="      //console.log(this.responseText);\n";
  ptr +="      if(chartT.series[0].data.length > 40) {\n";
  ptr +="        chartT.series[0].addPoint([x, y], true, true, true);\n";
  ptr +="      } else {\n";
  ptr +="        chartT.series[0].addPoint([x, y], true, false, true);\n";
  ptr +="      }\n";
  ptr +="    }\n";
  ptr +="  };\n";
  ptr +="  xhttp.open(\"GET\", \"/temperature\", true);\n";
  ptr +="  xhttp.send();\n";
  ptr +="}, 30000 ) ;\n";
  ptr +="\n";
  ptr +="var chartH = new Highcharts.Chart({\n";
  ptr +="  chart:{ renderTo:'chart-humidity' },\n";
  ptr +="  title: { text: 'BME280 Humidity' },\n";
  ptr +="  series: [{\n";
  ptr +="    showInLegend: false,\n";
  ptr +="    data: []\n";
  ptr +="  }],\n";
  ptr +="  plotOptions: {\n";
  ptr +="    line: { animation: false,\n";
  ptr +="      dataLabels: { enabled: true }\n";
  ptr +="    }\n";
  ptr +="  },\n";
  ptr +="  xAxis: {\n";
  ptr +="    type: 'datetime',\n";
  ptr +="    dateTimeLabelFormats: { second: '%H:%M:%S' }\n";
  ptr +="  },\n";
  ptr +="  yAxis: {\n";
  ptr +="    title: { text: 'Humidity (%)' }\n";
  ptr +="  },\n";
  ptr +="  credits: { enabled: false }\n";
  ptr +="});\n";
  ptr +="setInterval(function ( ) {\n";
  ptr +="  var xhttp = new XMLHttpRequest();\n";
  ptr +="  xhttp.onreadystatechange = function() {\n";
  ptr +="    if (this.readyState == 4 && this.status == 200) {\n";
  ptr +="      var x = (new Date()).getTime(),\n";
  ptr +="          y = parseFloat(this.responseText);\n";
  ptr +="      //console.log(this.responseText);\n";
  ptr +="      if(chartH.series[0].data.length > 40) {\n";
  ptr +="        chartH.series[0].addPoint([x, y], true, true, true);\n";
  ptr +="      } else {\n";
  ptr +="        chartH.series[0].addPoint([x, y], true, false, true);\n";
  ptr +="      }\n";
  ptr +="    }\n";
  ptr +="  };\n";
  ptr +="  xhttp.open(\"GET\", \"/humidity\", true);\n";
  ptr +="  xhttp.send();\n";
  ptr +="}, 30000 ) ;\n";
  ptr +="</script>\n";
  ptr +="</html>\n";
  return ptr;
}


void setup(void) {
  Serial.begin(115200);
  dht.begin();

  lcd.init();                     
  lcd.backlight();// Включаем подсветку дисплея

  pinMode(LED1pin, OUTPUT);
  pinMode(LED2pin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  Led3("BLACK");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
    Serial.println(WiFi.status());
    digitalWrite(greenPin, HIGH);
    digitalWrite(redPin, HIGH);
  }
  Led3("GREEN");
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(" ");
  Serial.println("Hi, Serge!");
  Serial.println(" ");

  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
  Serial.println("Hi, Serge!");

  server.on("/",handle_OnConnect); // Ответ сервера на запросы
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.on("/led2on", handle_led2on);
  server.on("/led2off", handle_led2off);
  server.on("/temperature", handle_temperature);
  server.on("/humidity", handle_humidity);
  server.onNotFound(handle_NotFound);

  server.begin(); //Запуск сервера
}

void loop () {
  i++;
  server.handleClient(); //обработка текущих входящих HTTP-запросов 
  if(LED1status)
    digitalWrite(LED1pin, HIGH);
  else
    digitalWrite(LED1pin, LOW);
  if(LED2status)
    digitalWrite(LED2pin, HIGH);
  else
    digitalWrite(LED2pin, LOW);
  if(i == 1){
    str1 = readTemperature();
    
    lcd.print("              ");
    lcd.setCursor(0,1);
    lcd.print("              ");
    lcd.setCursor(0,0);
    lcd.print("Temperature:");
    lcd.setCursor(0,1);
    lcd.print(str1);
    lcd.setCursor(0,0);
  }
  else if(i == 50){
    str2 = readHumidity();
    lcd.print("              ");
    lcd.setCursor(0,1);
    lcd.print("              ");
    lcd.setCursor(0,0);
    lcd.print("Humidity:");
    lcd.setCursor(0,1);
    lcd.print(str2);
    lcd.setCursor(0,0);
    bool work = !(str1.equals("") || str2.equals(""));
    if (work){
      color1 = "GREEN";
    }
    else{
      color1 = "RED";
    }
  }
  else if( i == 100){
    i = 0;
  }
  Led3(color1);
  delay(100);
}
