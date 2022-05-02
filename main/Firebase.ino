
#include <ArduinoJson.h>
#include <FirebaseESP32.h>
#include <WiFi.h>// WiFi

#include "screen.h"
// firebase
//#define FIREBASE_HOST "iotproject-692ca.firebaseio.com"
//#define FIREBASE_AUTH "EhJnxJKynKGiJWC35UffWNMMU7XcqwdUzbcrp3r6"
#define FIREBASE_HOST "role-test-df819-test-database.firebaseio.com"
#define FIREBASE_AUTH "BoekWlHf4TpMaLMM1i5X7KVzOkHz6Q3jaWajjbNM"

FirebaseData firebaseData;
//FirebaseJson json;

static String push_address = "/Device/s996";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

static int lux_state2 = 0;
static int temp_state2 = 0;
static int gasu_state2 = 0;
static int ptemp_state2 = 0;

const char* fname = "/wifi.csv";
File fp;
char ssid[32];
char pass[32];

void Firebase_init() {

  // <<<  SDカードからWi-FiのSSIDとPASSWORDを取得する
  unsigned int cnt = 0;
  char data[64];
  char *str;

  fp = SD.open(fname, FILE_READ);
  while(fp.available()){
    data[cnt++] = fp.read();
  }
  strtok(data,",");
  str = strtok(NULL,"\r");    // CR
  strncpy(&ssid[0], str, strlen(str));

  strtok(NULL,",");
  str = strtok(NULL,"\r");    // CR
  strncpy(&pass[0], str, strlen(str));
  // >>>
  // <<< Wi-Fi, Firebase接続
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
  }
  configTime(9 * 3600L, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");
  delay(1000);
  M5.Lcd.fillScreen(BLACK);
  fireok();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  delay(1000);
}

void get_state(int temp_state, int lux_state, int gasu_state, int ptemp_state){
  lux_state2 = lux_state;
  temp_state2 = temp_state;
  gasu_state2 = gasu_state;
  ptemp_state2 = ptemp_state;
}

void push_to_firebase(int lux, float temp, float o2, float co2, float ptemp) { // Firebaseに値を送る
  FirebaseJson json;

  // WiFiに接続されているならば
  if(WiFi.status() == WL_CONNECTED){
  
    // <<< タイムスタンプ作成
    struct tm timeinfo;
  
    getLocalTime(&timeinfo);
    
    int yy = timeinfo.tm_year + 1900; // 年
    int mm = timeinfo.tm_mon + 1; // 月
    int dd = timeinfo.tm_mday; // 日
    int hh = timeinfo.tm_hour; //　時
    int nn = timeinfo.tm_min; // 分
    int ss = timeinfo.tm_sec; // 秒
  
    char timestamp0[100];
    sprintf(timestamp0,"%d-%02d-%02d %02d:%02d:%02d",yy,mm,dd,hh,nn,ss); //　日付など0埋め2桁にする必要あり
  
    String timestamp = String(timestamp0);
    // >>>
    if(lux_state2 == 1){
      json.add("Lux", lux);
    }
    if(temp_state2 == 1){
      json.add("temp", temp);
    }
    if(o2 != 0 or co2 != 0){
      json.add("O2", o2);
      json.add("CO2", co2);
    }
    if(ptemp_state2 == 1){
      json.add("PTemp", ptemp);
    }
    json.add("Timestamp", timestamp);
    
    Firebase.pushJSON(firebaseData, push_address, json); // Json型でFirebaseに送る
  }
}
