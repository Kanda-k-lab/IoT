
#include "tsl2561.h"
#include "Firebase.h"
#include "screen.h"
#include "ds18b20.h"
#include "gasu.h"

#define NO_CONNECT 0
#define CONNECTED 1

#define SEC2 2
#define SEC5 5
#define SEC10 10
#define MIN5 300
#define MIN30 1800

static int temp_state = NO_CONNECT;
static int lux_state = NO_CONNECT;
static int gasu_state = NO_CONNECT;
static int ptemp_state = NO_CONNECT;

static int lux = 0;
static float temp = 0;
static float o2 = 0;
static float co2 = 0;
static float ptemp = 0;

//区間平均計算用
static int lux_ave = 0;
static float temp_ave = 0;
static float o2_ave = 0;
static float co2_ave = 0;
static float ptemp_ave = 0;

static int before_state = SEC2;

void sensor_search() {

  // ポートAで照度センサが接続されているか判定
  byte error, address; //variable for error and I2C address
  int nDevices;
  int judge = 0;
  int judge1 = 0;
  gasu_Init(); // 2022-04-26 追加 ここに置かなければ接続判定されない．
  //Serial.println("Scanning...");
  pinMode(16, INPUT);
  if(digitalRead(16) == 1){ // ポートC(1)に温度センサが接続されているか判定
    vInitDs18b20_0();
    temp_state = CONNECTED;
  }
  if(digitalRead(17) == 1){ // ポートC(2)に温度センサが接続されているか判定
    vInitDs18b20_1();
    ptemp_state = CONNECTED;
  }
  
  Lux_init();
  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      if (address < 16)
        Serial.print("0");
      if(String(address, HEX) == "29"){ // 照度センサのアドレスは0x29
        judge = 1;
      }
      nDevices++;
    }
    else if (error == 4)
    {
      if (address < 16)
        Serial.print("0");
    }
  }
  
  if(judge == 1){ // ポートAに照度センサが接続されていたとき
    lux_state = CONNECTED;
  }
  
  if(digitalRead(36) == 1){ // ポートBにガステックが接続されているか判定
    gasu_Init();
    gasu_state = CONNECTED;
  }
  get_state(temp_state, lux_state, gasu_state, ptemp_state);
  hyouji(temp_state, lux_state, gasu_state, ptemp_state); // 何が接続されているか表示
}

void get_data(int judge, int state) { // 各センサから値を取得する
  if(lux_state == CONNECTED){ // 照度センサが接続されているとき
    lux = get_lux();
  }
  if(temp_state == CONNECTED) { // 温度センサが接続されているとき
    temp = fGetTemp0();
  }
  if(gasu_state == CONNECTED) { // ガステックが接続されているとき
    get_gasu();
    o2 = get_o2() / 10;
    co2 = get_co2() / 100;
  }
  if(ptemp_state == CONNECTED) { // p温度センサが接続されているとき
    ptemp = fGetTemp1();
  }

  // 区間平均計算
  if(before_state != state){ // 計測間隔が変更された時
    lux_ave = 0;
    temp_ave = 0;
    o2_ave = 0;
    co2_ave = 0;
    ptemp_ave = 0;
  }

  if(state == MIN5) { //　5分計測のとき
    if(judge == 2) { // 1分毎に足す
      lux_ave += lux;
      temp_ave += temp;
      o2_ave += o2;
      co2_ave += co2;
      ptemp_ave += ptemp;
    }else if(judge == 1){ // 5分になったとき，平均を出してFirebaseに送信
      lux_ave += lux;
      temp_ave += temp;
      o2_ave += o2;
      co2_ave += co2;
      ptemp_ave += ptemp;
      push_to_firebase(lux_ave / 5, temp_ave / 5, o2_ave / 5, co2_ave / 5, ptemp_ave /5);
      lux_ave = 0;
      temp_ave = 0;
      o2_ave = 0;
      co2_ave = 0;
      ptemp_ave = 0;
    }
  }

  if(state == MIN30) { // 30分計測の時
    if(judge == 2) {
      lux_ave += lux;
      temp_ave += temp;
      o2_ave += o2;
      co2_ave += co2;
      ptemp_ave += ptemp;
    }else if(judge == 1){
      lux_ave += lux;
      temp_ave += temp;
      o2_ave += o2;
      co2_ave += co2;
      ptemp_ave += ptemp;
      push_to_firebase(lux_ave / 6, temp_ave / 6, o2_ave / 6, co2_ave / 6, ptemp_ave / 6);
      lux_ave = 0;
      temp_ave = 0;
      o2_ave = 0;
      co2_ave = 0;
      ptemp_ave = 0;
    }
  }
  
  if(state == SEC10 or state == SEC5 or state == SEC2){
    if(judge == 1){
      push_to_firebase(lux, temp, o2, co2, ptemp);
    }
  }

  before_state = state;
  show_data(lux, temp, o2, co2, ptemp); // データ出力
}
