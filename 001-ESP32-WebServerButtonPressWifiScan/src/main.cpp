/*
  ESP32 SotfAP WebServer 
  1、显示一个 HelloWorld 页面
  2、数据提交 (wifi设置)
    设计一个 WiFi 配置页面，用于在Web端进行 WIFI 配网
  3、设备LED控制
  4、参数设置
  5、美化 HTML 页面
*/
#include <Arduino.h>

#include <HTTPClient.h>
#include <WiFi.h>
#include <WebServer.h>

#include <Preferences.h>


// HelloWorld  html 页面
const String ROOT_HTML_HELLOWORLD_PAGE PROGMEM = R"rawliteral(
  <!DOCTYPE html><html lang='zh'>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <link href='favicon.ico' rel='shortcut icon'>
    <title>ESP32WebServerHelloWorld</title>
</head>
<body>
<h1>HelloWorld</h1>
</body>
</html>
)rawliteral";


const String WIFICONFIG_HTML_PAGE1 PROGMEM = R"rawliteral(
  <!DOCTYPE html><html lang='zh'>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <link href='favicon.ico' rel='shortcut icon'>
    <title>WIFI配置页面</title>
    <style type='text/css'>
        #titleDiv{
            margin-top: 20px;
            height: 10%;
            width: 100%;
            text-align: center;
            font-size: 2rem;
            font-weight: bold;
        }
        .titleOption{
            text-align: center;
            margin-top: 30px;
            height: 40px;
            background-color: dodgerblue; 
            position: relative;
            color: #ffffff;
            border-radius: 5px;
            line-height: 40px;
        }
        #selectDiv {
            margin-top: 20px;
            height: 40px;
            border-radius: 5px;
            box-shadow: 0 0 5px #ccc;
            position: relative;   
        }
        select {
            border: none;
            outline: none;
            width: 100%;
            height: 40px;
            line-height: 40px;
            appearance: none;
            -webkit-appearance: none;
            -moz-appearance: none;
            text-align: center;
            font-size: 1rem;
        }
        .passAndCity{
            border: none;
            margin-top: 20px;
            height: 40px;
            border-radius: 5px;
            box-shadow: 0 0 5px #ccc;
            font-size: 1rem;
            position: relative;
            text-align: center;
        }
        #rgbDiv{
            margin-top: 25px;
            position: relative;
            text-align: center;
        }
        #rgbDiv input{
            height: 35px;
            border-radius: 5px;
            box-shadow: 0 0 5px #ccc;
            font-size: 1rem;
        }
        .rgb{
            margin-left: 5px;
            width: 26%;
        }
        #sub{
            text-align: center;
            margin-top: 50px;
            height: 40px;
            background-color: dodgerblue; 
            position: relative;
            color: #ffffff;
            border-radius: 5px;
            line-height: 40px;
            cursor: pointer;
        }
        #wifiscanbtn{
            text-align: center;
            margin-top: 50px;
            height: 40px;
            background-color: dodgerblue; 
            position: relative;
            color: #ffffff;
            border-radius: 5px;
            line-height: 40px;
            cursor: pointer;
        }
        #tail{
            font-size: 0.9rem;
            margin-top: 5px;
            width: 100%;
            text-align: center;
            color: #757575;
        }
        #LEDDiv{
            margin-top: 20px;
            height: 10%;
            width: 100%;
            text-align: center;
            font-size: 2rem;
            font-weight: bold;
        }
        .button { 
            background-color: #4CAF50; 
            border: none; 
            color: white; 
            padding: 16px 40px;
            text-decoration: none; 
            font-size: 30px; 
            margin: 2px; 
            cursor: pointer;
        }
        .ledbutton
        {
            background-color: dodgerblue; 
            border: none; 
            color: white; 
            padding: 16px 40px;
            text-decoration: none; 
            font-size: 30px; 
            margin: 2px; 
            cursor: pointer;
        }
    </style>
</head>
<body>
    <div id='titleDiv'>WIFI配置</div>
    <form action='configwifi' method='post' id='form' accept-charset="UTF-8">
        <div class='titleOption commonWidth'>WiFi名称</div>
        <div id='selectDiv' class='commonWidth'>
            <select name='ssid' id='ssid'>
                <option value=''></option>
)rawliteral";


const String WIFICONFIG_HTML_PAGE2 PROGMEM = R"rawliteral(
  </select>
        </div>
        <div class='titleOption commonWidth'>WiFi密码</div>
)rawliteral";

const String WIFICONFIG_HTML_PAGE3 PROGMEM = R"rawliteral(
        <div class='commonWidth' style="background-color: '0 20 23';"></div>
        <div id='sub' onclick='doSubmit()'>提交</div>
    </form>
    <form action='wifiscan' method='post' id='scanform' accept-charset="UTF-8">
        <div id='wifiscanbtn' onclick='wifiscanbtn()'>WIFI扫描</div>
    </form>
    <div id="LEDDiv">LED控制</div>
)rawliteral";

const String WIFICONFIG_HTML_PAGE4 PROGMEM = R"rawliteral(
    <script type='text/javascript'>
        function wifiscanbtn(){
            console.log("wifi scan");
            document.getElementById('scanform').submit();
        }
        function doSubmit(){
            var select = document.getElementById('ssid');
            var selectValue = select.options[select.selectedIndex].value;
            if(selectValue == ''){
                alert('请选择要连接的WiFi');
                return;
            }
            if(document.getElementById('pass').value == ''){
                alert('请输入该WiFi的密码');
                return;
            }
            document.getElementById('form').submit();
        }
        var nodes = document.getElementsByClassName('commonWidth');
        var rgbNodes = document.getElementsByClassName('rgb');
        var node = document.getElementById('sub');
        var screenWidth = window.screen.width;
        function setWidth(width){
            nodes[0].setAttribute('style',width);
            nodes[1].setAttribute('style',width);
            nodes[2].setAttribute('style',width);
            nodes[3].setAttribute('style',width);
            nodes[4].setAttribute('style',width);
            nodes[5].setAttribute('style',width);
        }
        if(screenWidth > 1000){
            setWidth('width: 40%;left: 30%;');
            node.setAttribute('style','width: 14%;left: 43%;');
        }else if(screenWidth > 800 && screenWidth <= 1000){
            setWidth('width: 50%;left: 25%;');
            node.setAttribute('style','width: 16%;left: 42%;');
        }else if(screenWidth > 600 && screenWidth <= 800){
            setWidth('width: 60%;left: 20%;');
            node.setAttribute('style','width: 20%;left: 40%;');
        }else if(screenWidth > 400 && screenWidth <= 600){
            setWidth('width: 74%;left: 13%;');
            node.setAttribute('style','width: 26%;left: 37%;');
        }else{
            setWidth('width: 90%;left: 5%;');
            node.setAttribute('style','width: 40%;left: 30%;');
        }
    </script>
</body>
</html>
)rawliteral";



// SoftAP相关
const char *APssid = "ESP32WebServer";
IPAddress staticIP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);


// 要连接的wifi 信息
String ssid;  //WIFI名称
String pass;  //WIFI密码
// 是否顺利连接上wifi
bool wifiConnected = false;

String WifiNames; // 根据搜索到的wifi生成的option字符串
String PassHTML; // 根据密码生成的HTML字符串
String LEDHTML;  // LED 控制 html 显示字符串
bool apConfigflag; // 系统启动时是否需要配网




// NVS 读写 EEPROM
Preferences prefs; // 声明Preferences对象


// led 初始状态
bool ledstatus=false;
String ledstatus_str="OFF";

// 上电时读取Wifi账号、密码等信息
void getInfos(){
  prefs.begin("configwifi");
  ssid = prefs.getString("ssid", "");
  pass = prefs.getString("pass", "");
  apConfigflag = prefs.getBool("apConfig", true);
  prefs.end();
}

// 用户配网后写入Wifi账号、密码
void recordInfos(String ssid,String pass,bool apConfig){
  prefs.begin("configwifi");
  prefs.putString("ssid", ssid);
  prefs.putString("pass", pass);
  prefs.putBool("apConfig", apConfig);
  prefs.end();
}


// 扫描WiFi,并将扫描到的Wifi组成option选项字符串
void scanWiFi(){
  Serial.println("开始扫描WiFi");
  int n = WiFi.scanNetworks();
  if (n){
    Serial.print("扫描到");
    Serial.print(n);
    Serial.println("个WIFI");
    WifiNames = "";
    bool hasSavedSSID = false; // 记录是否扫描到已经存储在NVS中的wifi
    for (size_t i = 0; i < n; i++){
      int32_t rssi = WiFi.RSSI(i);
      String signalStrength;
      if(rssi >= -35){
        signalStrength = " (信号极强)";
      }else if(rssi >= -50){
        signalStrength = " (信号强)";
      }else if(rssi >= -70){
        signalStrength = " (信号中)";
      }else{
        signalStrength = " (信号弱)";
      }
      WifiNames += "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + signalStrength + "</option>";
      if(WiFi.SSID(i).equals(ssid) && !hasSavedSSID){
        hasSavedSSID = true;
        WifiNames += "<option selected value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + signalStrength + "</option>";
      }else{
        WifiNames += "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + signalStrength + "</option>";
      }
    }
    if(!hasSavedSSID && !ssid.equals("")){ // 如果扫描到的wifi中，没有已经存储的wifi，则手动添加进去
      WifiNames += "<option selected value='" + ssid + "'>" + ssid + "</option>";
    }
    Serial.println(WifiNames.c_str());
  }else{
    Serial.println("没扫描到WIFI");
  }
}

// 开启AP模式，如果开启失败，重启系统
void startAP(){
  Serial.println("开启AP模式...");
  WiFi.enableAP(true); // 使能AP模式
  //传入参数静态IP地址,网关,掩码
  WiFi.softAPConfig(staticIP, gateway, subnet);
  if (!WiFi.softAP(APssid)) {
    Serial.println("AP模式启动失败");
    ESP.restart(); // Ap模式启动失败，重启系统
  }  
  Serial.println("AP模式启动成功");
  Serial.print("IP地址: ");
  Serial.println(WiFi.softAPIP());
}


void getLEDHTML(bool ledstatus)
{
  if(ledstatus ==true)
  {
    LEDHTML = "<p><a href=\"/ledoff\"><button class=\"ledbutton\">OFF</button></a></p>";
  }
  else
  {
    LEDHTML = "<p><a href=\"/ledon\"><button class=\"ledbutton\">ON</button></a></p>";
  }

}

// 将从NVS中获取到的信息组合成字符串,将wifi密码组合成字符串
void getHTML(){
  PassHTML = "<input type='text' placeholder='请输入WiFi密码' name='pass' id='pass' class='passAndCity commonWidth' value='" + pass + "'>";
}

// 处理服务器请求
void doClient(){
  server.handleClient();
}

// 处理网站根目录的访问请求
void handleRoot(){
  server.send(200,"text/html", ROOT_HTML_HELLOWORLD_PAGE);
}


void handleConfig(){
  server.send(200,"text/html", WIFICONFIG_HTML_PAGE1 + WifiNames + WIFICONFIG_HTML_PAGE2 + PassHTML + WIFICONFIG_HTML_PAGE3 + LEDHTML + WIFICONFIG_HTML_PAGE4);
}

void handleWifiScanConfig(){
  scanWiFi();
  handleConfig();
}

void handleLEDON()
{
  ledstatus = true;
  ledstatus_str = "ON";
  Serial.print("LED    ");
  Serial.println(ledstatus_str);
  getLEDHTML(ledstatus);
  handleConfig();
}

void handleLEDOFF()
{
  ledstatus = false;
  ledstatus_str = "OFF";
  Serial.print("LED    ");
  Serial.println(ledstatus_str);
  getLEDHTML(ledstatus);
  handleConfig();
}

// 处理404情况的函数'handleNotFound'
void handleNotFound(){
  //handleRoot(); // 根页面
  handleConfig(); //访问不存在目录则返回配置页面
}



// 提交数据后的提示页面
void handleConfigWifi(){
  //判断是否有WiFi名称
  if (server.hasArg("ssid")){
    Serial.print("获得WiFi名称:");
    ssid = server.arg("ssid");
    Serial.println(ssid);
  }else{
    Serial.println("错误, 没有发现WiFi名称");
    server.send(200, "text/html", "<meta charset='UTF-8'>错误, 没有发现WiFi名称");
    return;
  }
  //判断是否有WiFi密码
  if (server.hasArg("pass")){
    Serial.print("获得WiFi密码:");
    pass = server.arg("pass");
    Serial.println(pass);
  }else{
    Serial.println("错误, 没有发现WiFi密码");
    server.send(200, "text/html", "<meta charset='UTF-8'>错误, 没有发现WiFi密码");
    return;
  }

  // 将信息存入nvs中
  recordInfos(ssid, pass, false);
  // 获得了所需要的一切信息，给客户端回复
  server.send(200, "text/html", "<meta charset='UTF-8'><style type='text/css'>body {font-size: 2rem;}</style><br/><br/>WiFi: " + ssid + "<br/>密码: " + pass + "<br/>已取得相关信息,正在尝试连接,请手动关闭此页面。");
  Serial.println("获取到 WIFI 配置信息，系统即将重启");
  delay(2000);
  ESP.restart();
}




// 启动服务器
void startServer(){
  // 当浏览器请求服务器根目录(网站首页)时调用自定义函数handleRoot处理，设置主页回调函数，必须添加第二个参数HTTP_GET，否则无法强制门户
  server.on("/", HTTP_GET, handleRoot);
  server.on("/config", HTTP_GET, handleConfig);

  server.on("/wifiscan", HTTP_POST, handleWifiScanConfig);

  server.on("/ledon", HTTP_GET, handleLEDON);

  server.on("/ledoff", HTTP_GET, handleLEDOFF);
  
  // HTTP POST 
  // 当浏览器请求服务器/configwifi(表单字段)目录时调用自定义函数handleConfigWifi处理
  server.on("/configwifi", HTTP_POST, handleConfigWifi);
  
  // 当浏览器请求的网络资源无法在服务器找到时调用自定义函数handleNotFound处理   
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("服务器启动成功！");
}


// 开启SoftAP进行配网
void wifiConfigBySoftAP(){
  // 开启AP模式，如果开启失败，重启系统
  startAP();
  // 扫描WiFi,并将扫描到的WiFi组成option选项字符串
  scanWiFi();
  // 将从NVS中获取到的颜色信息组合成字符串,将wifi密码组合成字符串
  getHTML();
  getLEDHTML(ledstatus);
  // 启动服务器
  startServer();
}



// 连接WiFi
void connectWiFi(int timeOut_s){
  int connectTime = 0; //用于连接计时，如果长时间连接不成功，则提示失败
  Serial.print("正在连接网络");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    connectTime++;
    if (connectTime > 2 * timeOut_s){ //长时间连接不上，提示连接失败
      Serial.println("网络连接失败...");
      return;
    }
  }
  wifiConnected = true;
  Serial.println("网络连接成功");
  Serial.print("本地IP： ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 WebServer WiFI Config Example");
  
  // 从NVS中获取信息
  getInfos();

  if(apConfigflag)
  {
    wifiConfigBySoftAP();
  }
  else
  {
    connectWiFi(30);
  }

}

void loop() {
  if(apConfigflag)
  {
    doClient(); // 监听客户端配网请求
    delay(50);
  }
  else
  {
    Serial.println("WiFi Client running...");
    delay(5000);
  }

}