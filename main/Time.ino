
#include "sensor.h"

#define SEC2 2
#define SEC5 5
#define SEC10 10
#define MIN1 60
#define MIN5 300
#define MIN30 1800

hw_timer_t * timer = NULL;

static int cnt = 0;
static int sensor_cnt = 0;
static int before_cnt = 0; // 1秒間に2回計測対策
static int num_before = 0;

void cntTimer(){
  if(cnt > 1800){ // 30分超えたら初期化
    cnt = 0;
  }
  cnt++;
  sensor_cnt = 0;
}

void Timer_init() { // タイマースタート
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &cntTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
}

void stopTimer() { // タイマーストップ
  if(timer) {
    timerEnd(timer);
    timer = NULL;
  }
}

void get_sensor(int num) { //　センサーから値を取得する
  int judge = 0; // 同じ時間に2回値を取得しないように判定
  if(num_before != num){ // 計測間隔が切り替わったら検知用
    cnt = 1;
  }
  
  if((cnt - 1) == before_cnt){ // 同じ時間で2回計測することの対策
    if(num == SEC2) { // 2秒指定の時(リアルタイムモード)
      if(cnt % SEC2 == 0){
        get_data(1, SEC2);
      }
    }
    if(num == SEC5) { // 5秒指定
      if(cnt % SEC5 == 0){
        get_data(1, SEC5);
        judge += 1;
      }
      if(cnt % SEC2 == 0 and judge == 0){
        get_data(0, SEC5);
      }
    }
    else if(num == SEC10) { // 10秒指定
      if(cnt % SEC10 == 0){
        get_data(1, SEC10);
        judge += 1;
      }
      if(cnt % SEC2 == 0 and judge == 0){
        get_data(0, SEC10);
      }
    }
    else if(num == MIN5) { // 5分指定
      if(cnt % MIN5 == 0){
        get_data(1, MIN5);
        judge += 1;
      }else if(cnt % MIN1 == 0){
        get_data(2, MIN5);
        judge += 1;
      }
      if(cnt % SEC2 == 0 and judge == 0){
        get_data(0, MIN5);
      }
    }
    else if(num == MIN30) { // 30分指定
      if(cnt % MIN30 == 0){
        get_data(1, MIN30);
        judge += 1;
      }else if(cnt % MIN5 == 0){
        get_data(2, MIN30);
        judge += 1;
      }
      if(cnt % SEC2 == 0 and judge == 0){
        get_data(0, MIN30);
      }
    }
  }
  num_before = num;
  before_cnt = cnt;
}
