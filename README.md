# MQTT-Test
- このコードは、ESP32をWi-Fiに接続し、EMQXのパブリックMQTTブローカー（broker.emqx.io）を使って、MQTTトピック emqx/esp32 を購読（Subscribe）して、受け取ったメッセージを処理・表示するプログラムである。
## 開発環境構築
1. ArduinoIDEをインストール。
2. 「ArduinoIDE」メニューの中の「基本設定」に遷移し、「追加のボードマネージャーのURLの項目」に「https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json」　を加える。
3. 「ツール」メニューの「ライブラリを管理」から「ライブラリマネージャー」に遷移し、「esp32 by Espressif Systems」をダウンロードする。
4. 「ツール」メニューの「ボード」から、使用するボードを「ESP32 Dev Module」に指定。
5. 「ツール」メニューの「ポート」から、ESP32が接続されているCOMポート（/dev/cu.SLAB_USBtoUARTなど）を選択。
## MQTT環境構築
- [MQTTブローカー](https://www.emqx.com/ja/cloud)を準備する
  - 暫定的にEMQX Cloudの無料サービスのEMQX Cloud Serverlessを使用。
- MQTTクライアントライブラリの「PubSubClient」をダウンロードする。
  - 「ライブラリマネージャ」に遷移し、「PubSubClient by Nick O'Leary」ライブラリを見つけ、インストール。 
## 使用しているライブラリ 
-  (ライブラリ、用途名)
-  (WiFi.h、ESP32をWi-Fiに接続)
-  (PubSubClient.h、MQTTプロトコルで通信)
-  (ArduinoJson.h、JSON形式のメッセージをパース処理）
## コードの主要な流れ
### Wi-Fi接続
```
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
  ...
}
```
- ssid と password を使ってWi-Fiに接続。
- 10回までリトライし、失敗したら自動で再起動。（ESP.restart()）
### MQTTブローカーに接続
```
client.setServer(mqtt_broker, mqtt_port);
client.setCallback(callback);
client.connect(...)
```
- EMQXのMQTTブローカー（broker.emqx.io:1883）に接続
- 接続成功後
  - client.publish(...) で「こんにちは、ESP32です ^^」と1回だけ送信（パブリッシュ）
  - client.subscribe(...) でトピック emqx/esp32 を購読
### メッセージ受信処理（コールバック関数）
```
void callback(char *topic, byte *payload, unsigned int length)
```
- MQTTメッセージを受信すると呼び出される関数
- メッセージがJSON形式の場合
  - ArduinoJson を使って解析
  - do_value（溶存酸素値）, do_unit（単位）, relay_state（リレー状態）を取り出し表示
  - 例：🌊 溶存酸素: 1.54 mg/L, リレー: OFF
- JSONでない場合はそのまま文字列表示
### loop() 関数
```
void loop() {
  client.loop();
}
```
- MQTTのメッセージの受信処理を継続させるために必要
## 実際に表示される出力例
```
WiFiネットワークに接続しました
クライアント esp32-client-xx がパブリックMQTTブローカーに接続します
EMQXのパブリックMQTTブローカーに接続しました
トピックにメッセージが届きました: emqx/esp32
🌊 溶存酸素: 1.54 mg/L, リレー: OFF
```

