#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// WiFi設定
const char* ssid = "Buffalo-G-2530";
const char* password = "h6sdkbhuevc46";

// MQTTブローカー
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "emqx/esp32";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // ソフトウェアシリアルボー速度を115200に設定
  Serial.begin(115200);
  // WiFi接続開始を追加
  WiFi.begin(ssid, password);
  // WiFiネットワークへの接続
  int wifi_retry = 0;
 while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print("WiFi接続失敗: ");
  Serial.println(WiFi.status());  // ステータスコード表示
  wifi_retry++;
  if (wifi_retry > 10) {
    Serial.println("WiFi接続を10回試行しましたが失敗。再起動します。");
    ESP.restart();
  }
  }
  Serial.println("WiFiネットワークに接続しました");
  // MQTTブローカーへの接続
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("クライアント %s がパブリックMQTTブローカーに接続します\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("EMQXのパブリックMQTTブローカーに接続しました");
    } else {
      Serial.print("状態で失敗しました ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  
  // パブリッシュとサブスクライブ
  client.publish(topic, "こんにちは、ESP32です ^^");
  client.subscribe(topic);  
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("トピックにメッセージが届きました:");
  Serial.println(topic);

  // JSONパース
  payload[length] = '\0'; // 文字列終端を追加
  String msg = String((char *)payload);

  if (msg.startsWith("{")) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, msg);
    if (!error) {
      float do_value = doc["do_value"];
      const char* do_unit = doc["do_unit"];
      bool relay = doc["relay_state"];

      Serial.printf("🌊 溶存酸素: %.2f %s, リレー: %s\n", do_value, do_unit, relay ? "ON" : "OFF");
    } else {
      Serial.println("⚠️ JSON解析エラー");
    }
  } else {
    Serial.print("メッセージ:");
    Serial.println(msg);
  }

  Serial.println("-----------------------");
}

void loop() {
  client.loop();
}
