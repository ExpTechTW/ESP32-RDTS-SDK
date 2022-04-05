#include <WiFi.h> // Wi-Fi 庫
#include <WiFiMulti.h> // Wi-Fi STA 管理庫
#include <RDTS.h> // RDTS 庫
#include <ArduinoJson.h> // Json 庫

WiFiMulti WiFiMulti;
using namespace websockets;
WebsocketsClient RDTS; // RDTS 對象
StaticJsonDocument<4096> JSON; // Json 對象 (接收)
DynamicJsonDocument json(4096); // Json 對象 (傳送)
char json_string[4096];

String ssid = ""; // Wi-Fi SSID
String pass = ""; // Wi-Fi PassWord

void Connect() {
  while (true) {
    if (!RDTS.RDTSconnect()) { // 連線到 RDTS 服務器
      Serial.println("Connect fail"); // 連線失敗
      delay(100);
    } else {
      Serial.println("Connect success"); //連線成功

      json["EID"] = 1; // 設備編號
      json["APIkey"] = "<放你的 API Key>"; // APIkey
      json["Function"] = "RDTS"; // 固定值
      json["Type"] = "save"; // 固定值
      json["FormatVersion"] = 1; // 固定值
      json["Value"] = "test"; // 資料

      serializeJson(json, json_string);

      RDTS.send(json_string); // RDTS 服務器 身分驗證

      break;
    }
  }
}

void setup() {
  Serial.begin(115200);

  Serial.println(WiFi.macAddress()); // 顯示 Mac 地址
  WiFi.mode(WIFI_STA); // 指定為 STA 模式

  WiFiMulti.addAP(ssid.c_str(), pass.c_str()); // 新增 Wi-Fi

  if (WiFiMulti.run() != WL_CONNECTED) { // 連接 Wi-Fi
    ESP.restart(); // 重啟
  }

  Connect();

  // RDTS 回傳處理
  RDTS.onMessage([&](WebsocketsMessage message) {
    deserializeJson(JSON, message.data()); // 解析 回傳>>Json
    Serial.println(message.data()); // 輸出 接收到的資料
    /*
       示例回傳 Json
       {
        "Function":"restart"
       }
      if (JSON["Function"].as<String>() == "restart") {
      ESP.restart(); // 重啟
      }
    */

  });
}

void loop() {

  if (RDTS.available()) { // RDTS 已連接
    RDTS.poll();// RDTS 輪詢
  } else {
    Connect();
  }

}
