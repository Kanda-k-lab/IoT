#include "gasu.h"

#define NO_CONNECT 0
#define CONNECTED 1

#define SEC5 5     // 5秒
#define SEC10 10   // 10秒
#define MIN5 300   // 5分
#define MIN30 1800 // 30分

static String device_id = "s996";

static int temp_states = NO_CONNECT;
static int lux_states = NO_CONNECT;
static int gasu_states = NO_CONNECT;
static int ptemp_states = NO_CONNECT;

static int lux_copy = 0;
static float temp_copy = 0;
static float o2_copy = 0;
static float co2_copy = 0;
static float ptemp_copy = 0;


//static int mode_select[2] = {0, 0}; //[0]が1ならリアル，[1]が1なら計測間隔

void screen_init(){
  dengen();
}

void reset_disp() {
  M5.Lcd.fillScreen(BLACK);
}

void devi_show() { // デバイス番号を表示
  M5.Lcd.setCursor(270, 210); // 横，縦
  M5.Lcd.setTextSize(2);
  M5.Lcd.print(device_id);
}

void start_disp() {
  
  M5.Lcd.setCursor(130, 160); // 横，縦
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("press");

  M5.Lcd.fillRect(150, 190, 20, 20, BLUE);

  M5.Lcd.fillTriangle(160, 230, 140, 210, 180, 210, BLUE);
}

void real_disp() { // リアルタイムモードの計測中
  
  M5.Lcd.setCursor(60, 210); // 横，縦
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("realtime mode");

  devi_show();
}

void keisoku_disp(int Kankaku) { // 計測間隔指定モードの計測中

  M5.Lcd.setCursor(10, 210); // 横，縦
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("interval mode");
  if(Kankaku == SEC5){
    M5.Lcd.print("  5s");
  }else if(Kankaku == SEC10){
    M5.Lcd.print("  10s");
  }else if(Kankaku == MIN5){
    M5.Lcd.print("  5m");
  }else if(Kankaku == MIN30){
    M5.Lcd.print("  30m");
  }

  devi_show();
}

void kankaku_disp() { // 計測間隔を表示
  M5.Lcd.setCursor(0, 120); // 横，縦
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("select");

  devi_show();
}

void hyouji(int temp, int lux, int gasu, int ptemp){ // 接続されているセンサを表示
  M5.Lcd.setCursor(60, 60); // 横，縦
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("connected sensor");

  M5.Lcd.setCursor(70, 90); // 横，縦
  M5.Lcd.setTextSize(2);

  if(temp == CONNECTED) {
    temp_states = CONNECTED;
    M5.Lcd.print("temp ");
  }
  if(lux == CONNECTED) {
    lux_states = CONNECTED;
    M5.Lcd.print("lux ");
  }
  if(gasu == CONNECTED) {
    gasu_states = CONNECTED;
    M5.Lcd.print("o2 co2");
  }
  if(ptemp == CONNECTED) {
    ptemp_states = CONNECTED;
    M5.Lcd.print("ptemp ");
  }
  M5.Lcd.fillRect(50, 200, 300, 300, BLACK); // wifi接続中表示削除
  delay(1000);
}

void show_data(int lux, float temp, float o2, float co2, float ptemp) { // データを出力
  int num = 10;
  //　桁が変わったときはみ出た値を消す
  if(lux_copy >= 10000 and lux < 10000) {
    M5.Lcd.fillRect(180, 10, 200, 25, BLACK);
  }else if(lux_copy >= 1000 and lux < 1000){
    M5.Lcd.fillRect(180, 10, 200, 25, BLACK);
  }else if(lux_copy >= 100 and lux < 100){
    M5.Lcd.fillRect(180, 10, 200, 25, BLACK);
  }else if(lux_copy >= 10 and lux < 10){
    M5.Lcd.fillRect(180, 10, 200, 25, BLACK);
  }
  
  if(lux_states == CONNECTED) { // 照度センサが接続されているとき表示する
    lux_copy = lux;
    M5.Lcd.setCursor(20, num); // 横，縦
    M5.Lcd.setTextSize(3);
    M5.Lcd.print("lux[lux] : ");
    M5.Lcd.println(lux);
    num += 30;
  }
  if(temp_states == CONNECTED) { // 温度センサが接続されているとき表示する
    temp_copy = temp;
    M5.Lcd.setCursor(20, num); // 横，縦
    M5.Lcd.setTextSize(3);
    M5.Lcd.print("temp[C] : ");
    if(temp >= 100) {
      M5.Lcd.println(temp, 1);
    }else {
      M5.Lcd.println(temp);
    }
    num += 30;
  }
  if(gasu_states == CONNECTED) { // ガステックが接続されているとき表示する
    if(o2 == 0 and o2_copy != 0){
      M5.Lcd.fillRect(180, num, 150, 25, BLACK);
    }
    o2_copy = o2;
    co2_copy = co2;
    M5.Lcd.setCursor(50, num); // 横，縦
    M5.Lcd.setTextSize(3);
    M5.Lcd.print("o2[%] : ");
    M5.Lcd.println(o2_copy, 1);
    num += 30;
    M5.Lcd.setCursor(30, num); // 横，縦
    M5.Lcd.setTextSize(3);
    M5.Lcd.print("co2[%] : ");
    M5.Lcd.println(co2_copy);
  }
  if(ptemp_states == CONNECTED) { // 温度センサが接続されているとき表示する
    ptemp_copy = ptemp;
    M5.Lcd.setCursor(20, num); // 横，縦
    M5.Lcd.setTextSize(3);
    M5.Lcd.print("ptemp[C] : ");
    if(ptemp >= 100) {
      M5.Lcd.println(ptemp, 1);
    }else {
      M5.Lcd.println(ptemp);
    }
    num += 30;
  }
}

void dengen() { //　電源接続時の画面
  M5.Lcd.setCursor(20, 10); // 横，縦
  M5.Lcd.setTextSize(3);
  M5.Lcd.print("devide_id : ");
  M5.Lcd.println(device_id);

  M5.Lcd.setCursor(90, 200); // 横，縦
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Connecting to Wi-Fi");
}

void fireok() {
  M5.Lcd.setCursor(20, 10); // 横，縦
  M5.Lcd.setTextSize(3);
  M5.Lcd.print("devide_id : ");
  M5.Lcd.println(device_id);

  M5.Lcd.setCursor(50, 200); // 横，縦
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Connecting to Firebase");
}
