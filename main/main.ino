#include <M5Stack.h>

#include "Time.h"
#include "Firebase.h"
#include "screen.h"
#include "sensor.h"
// 今の状態
#define STATE_START 1 //計測開始画面
#define STATE_KEISOKU 2 //計測中
#define STATE_MODE_CHOICE 3 //モード変更画面
#define STATE_KANKAKU_CHOICE 4 //計測間隔変更画面
// 選択しているモード
#define MODE_REAL 1 // リアルタイムモード
#define MODE_KEISOKU 2 // 計測間隔指定モード

// 選択している計測間隔
#define SEC2 2     // 2秒
#define SEC5 5     // 5秒
#define SEC10 10   // 10秒
#define MIN5 300   // 5分
#define MIN30 1800 // 30分

static int Mode;      // 今のモード
static int Mode_button; //ボタンでモードを変更するための
static int Kankaku_butn; //ボタンで間隔を変更するための
static int CurState1; // 今の状態を示す
static int CurState2; // 今の状態を示す
static int Kankaku;   // 今の計測間隔
static int num = 0;   //ループの中で1回だけ実行用



void setup() {
  M5.begin();
  pinMode(21, INPUT_PULLDOWN);
  screen_init(); // 電源接続時の画面
  dacWrite(25, 0); // 雑音消去
  Firebase_init(); // firebase接続
  sensor_search(); // 接続されているセンサを確認する
  Mode = MODE_REAL; //　始めはリアルタイムモードで計測開始
  Mode_button = MODE_REAL;
  CurState1 = STATE_START;
  Kankaku = SEC5;
  Kankaku_butn = SEC5;
}

void loop() {
  
  switch( Mode ) {
    case MODE_REAL: // リアルタイム計測モード

      // 計測中の状態
      switch( CurState1 ) {
        case STATE_START: // 計測開始画面
          start_disp(); //　スタート画面を表示
          if(M5.BtnB.wasPressed()) { //　ボタンBが押されたら計測中に移動
            reset_disp();
            Timer_init();
            CurState1 = STATE_KEISOKU;
          }
          break;
    
        case STATE_KEISOKU: // 計測中
          get_sensor(SEC2); // 2秒指定
          real_disp();
          if(M5.BtnA.pressedFor(2000)) { // ボタンAが押されたらモード選択画面に移動
            reset_disp();
            CurState1 = STATE_MODE_CHOICE;
          }
          break;
    
        case STATE_MODE_CHOICE: // モード選択画面

          stopTimer(); //　タイマーを止める
          if(num == 0){ //最初の選択用の枠を表示
            M5.Lcd.drawRect(30, 60, 120, 90, RED); // 横, 縦, witdh, height
            M5.Lcd.drawRect(180, 60, 120, 90, BLACK);
            num = 1;
          }
          M5.Lcd.setCursor(60, 90); // 横，縦
          M5.Lcd.setTextSize(3);
          M5.Lcd.print("REAL");
          M5.Lcd.setCursor(200, 90); // 横，縦
          M5.Lcd.setTextSize(3);
          M5.Lcd.print("INTER");

          //　モード選択画面のボタンによるモード選択
          switch(Mode_button) {
            case MODE_REAL: //　リアルタイムモードが選択されている時
              if(M5.BtnA.wasPressed()) { //　ボタンAが押されたら左に移動
                M5.Lcd.drawRect(30, 60, 120, 90, BLACK); // 横, 縦, witdh, height
                M5.Lcd.drawRect(180, 60, 120, 90, RED);
                Mode_button = MODE_KEISOKU;
              }
              if(M5.BtnC.wasPressed()) { // ボタンCが押されたら右に移動
                M5.Lcd.drawRect(30, 60, 120, 90, BLACK); // 横, 縦, witdh, height
                M5.Lcd.drawRect(180, 60, 120, 90, RED);
                Mode_button = MODE_KEISOKU;
              }
              if(M5.BtnB.wasPressed()) { // ボタンBが押されたら選択確定 リアルタイムモードに移動
                num = 0;
                reset_disp();
                Timer_init();
                CurState1 = STATE_KEISOKU;
                CurState2 = STATE_KEISOKU;
                Mode = MODE_REAL;
              }
              break;

            case MODE_KEISOKU: // 計測間隔指定モードが選択されている時
              if(M5.BtnA.wasPressed()) { // ボタンAが押されたら左に移動
                M5.Lcd.drawRect(30, 60, 120, 90, RED); // 横, 縦, witdh, height
                M5.Lcd.drawRect(180, 60, 120, 90, BLACK);
                Mode_button = MODE_REAL;
              }
              if(M5.BtnC.wasPressed()) { // ボタンCが押されたら右に移動
                M5.Lcd.drawRect(30, 60, 120, 90, RED); // 横, 縦, witdh, height
                M5.Lcd.drawRect(180, 60, 120, 90, BLACK);
                Mode_button = MODE_REAL;
              }
              if(M5.BtnB.wasPressed()) { // ボタンBが押されたら選択確定　計測間隔指定モードに移動
                num = 0;
                reset_disp();
                Timer_init();
                CurState1 = STATE_KEISOKU;
                CurState2 = STATE_KEISOKU;
                Mode = MODE_KEISOKU;
              }
              break;

            default:
              break;
          }
          break;
        
        default:
          break;
      }
      break;

    case MODE_KEISOKU: //計測間隔指定モード
      
      switch( CurState2 ) {
        case STATE_START: // 計測開始画面
          
          start_disp(); // 計測開始画面を表示
          if(M5.BtnB.wasPressed()) { // ボタンBが押されたら計測中に移動
            reset_disp();
            Timer_init();
            CurState2 = STATE_KEISOKU;
          }
          break;
    
        case STATE_KEISOKU: // 計測中

          // 設定されている計測間隔で値を送信，表示をする
          if(Kankaku == SEC5) {
            get_sensor(SEC5); // 5秒指定
          }
          else if(Kankaku == SEC10) {
            get_sensor(SEC10); // 10秒指定
          }
          else if(Kankaku == MIN5) {
            get_sensor(MIN5); // 5分指定
          }
          else if(Kankaku == MIN30) {
            get_sensor(MIN30); // 30分指定
          }
          // 指定している計測間隔を表示する
          keisoku_disp(Kankaku);
          
          if(M5.BtnA.pressedFor(2000)) { // ボタンAが押されたらモード設定画面に移動
            reset_disp();
            CurState2 = STATE_MODE_CHOICE; // モード設定画面へ
          }
          if(M5.BtnB.pressedFor(2000)) { // ボタンBが押されたら計測間隔設定画面に移動
            reset_disp();
            CurState2 = STATE_KANKAKU_CHOICE; // 計測間隔設定画面へ
          }
          break;
    
        case STATE_MODE_CHOICE: // モード設定画面
          stopTimer(); // タイマーを止める
          if(num == 0){ // 最初の枠を表示
            M5.Lcd.drawRect(30, 60, 120, 90, BLACK); // 横, 縦, witdh, height
            M5.Lcd.drawRect(180, 60, 120, 90, RED);
            Mode_button = MODE_KEISOKU;
            num = 1;
          }
          M5.Lcd.setCursor(60, 90); // 横，縦
          M5.Lcd.setTextSize(3);
          M5.Lcd.print("REAL");
          M5.Lcd.setCursor(200, 90); // 横，縦
          M5.Lcd.setTextSize(3);
          M5.Lcd.print("INTER");

          switch(Mode_button) {
            case MODE_REAL: // リアルタイムモードが選択されているとき
              if(M5.BtnA.wasPressed()) { // ボタンAが押された時，左に移動
                M5.Lcd.drawRect(30, 60, 120, 90, BLACK); // 横, 縦, witdh, height
                M5.Lcd.drawRect(180, 60, 120, 90, RED);
                Mode_button = MODE_KEISOKU;
              }
              if(M5.BtnC.wasPressed()) { // ボタンCが押された時，右に移動
                M5.Lcd.drawRect(30, 60, 120, 90, BLACK); // 横, 縦, witdh, height
                M5.Lcd.drawRect(180, 60, 120, 90, RED);
                Mode_button = MODE_KEISOKU;
              }
              if(M5.BtnB.wasPressed()) { // ボタンBが押された時，選択確定　リアルタイムモードに移動
                num = 0;
                reset_disp();
                Timer_init();
                CurState1 = STATE_KEISOKU;
                CurState2 = STATE_KEISOKU;
                Mode = MODE_REAL;
              }
              break;

            case MODE_KEISOKU: // 計測間隔指定モードが設定されているとき
              if(M5.BtnA.wasPressed()) { // ボタンAが押された時，左に移動
                M5.Lcd.drawRect(30, 60, 120, 90, RED); // 横, 縦, witdh, height
                M5.Lcd.drawRect(180, 60, 120, 90, BLACK);
                Mode_button = MODE_REAL;
              }
              if(M5.BtnC.wasPressed()) { // ボタンCが押された時，右に移動
                M5.Lcd.drawRect(30, 60, 120, 90, RED); // 横, 縦, witdh, height
                M5.Lcd.drawRect(180, 60, 120, 90, BLACK);
                Mode_button = MODE_REAL;
              }
              if(M5.BtnB.wasPressed()) { // ボタンBが押された時，選択確定　計測間隔指定モードに移動
                num = 0;
                reset_disp();
                Timer_init();
                CurState1 = STATE_KEISOKU;
                CurState2 = STATE_KEISOKU;
                Mode = MODE_KEISOKU;
              }
              break;

            default:
              break;
          }
          break;

        case STATE_KANKAKU_CHOICE: // 計測間隔設定画面
          stopTimer(); // タイマーを止める
          if(num == 0){ // 最初の枠を表示
            M5.Lcd.drawRect(20, 60, 70, 80, RED); // 横, 縦, witdh, height
            M5.Lcd.drawRect(90, 60, 70, 80, BLACK);
            M5.Lcd.drawRect(160, 60, 70, 80, BLACK);
            M5.Lcd.drawRect(230, 60, 70, 80, BLACK);
            Kankaku_butn = SEC5;
            num = 1;
          }
          M5.Lcd.setCursor(40, 90); // 横，縦
          M5.Lcd.setTextSize(3);
          M5.Lcd.print("5s");
          M5.Lcd.setCursor(100, 90); // 横，縦
          M5.Lcd.setTextSize(3);
          M5.Lcd.print("10s");
          M5.Lcd.setCursor(180, 90); // 横，縦
          M5.Lcd.setTextSize(3);
          M5.Lcd.print("5m");
          M5.Lcd.setCursor(240, 90); // 横，縦
          M5.Lcd.setTextSize(3);
          M5.Lcd.print("30m");

          switch(Kankaku_butn) {
            case SEC5: //　5秒が選択されているとき
              if(M5.BtnA.wasPressed()) {
                M5.Lcd.drawRect(20, 60, 70, 80, BLACK); // 横, 縦, witdh, height
                M5.Lcd.drawRect(90, 60, 70, 80, BLACK);
                M5.Lcd.drawRect(160, 60, 70, 80, BLACK);
                M5.Lcd.drawRect(230, 60, 70, 80, RED);
                Kankaku_butn = MIN30;
              }
              if(M5.BtnC.wasPressed()) {
                M5.Lcd.drawRect(20, 60, 70, 80, BLACK); // 横, 縦, witdh, height
                M5.Lcd.drawRect(90, 60, 70, 80, RED);
                M5.Lcd.drawRect(160, 60, 70, 80, BLACK);
                M5.Lcd.drawRect(230, 60, 70, 80, BLACK);
                Kankaku_butn = SEC10;
              }
              if(M5.BtnB.wasPressed()) {
                num = 0;
                reset_disp();
                Timer_init();
                CurState1 = STATE_KEISOKU;
                CurState2 = STATE_KEISOKU;
                Kankaku = SEC5;
                Mode = MODE_KEISOKU;
              }
              break;

            case SEC10: // 10秒が選択されているとき
              if(M5.BtnA.wasPressed()) {
                M5.Lcd.drawRect(20, 60, 70, 80, RED); // 横, 縦, witdh, height
                M5.Lcd.drawRect(90, 60, 70, 80, BLACK);
                M5.Lcd.drawRect(160, 60, 70, 80, BLACK);
                M5.Lcd.drawRect(230, 60, 70, 80, BLACK);
                Kankaku_butn = SEC5;
              }
              if(M5.BtnC.wasPressed()) {
                M5.Lcd.drawRect(20, 60, 70, 80, BLACK); // 横, 縦, witdh, height
                M5.Lcd.drawRect(90, 60, 70, 80, BLACK);
                M5.Lcd.drawRect(160, 60, 70, 80, RED);
                M5.Lcd.drawRect(230, 60, 70, 80, BLACK);
                Kankaku_butn = MIN5;
              }
              if(M5.BtnB.wasPressed()) {
                num = 0;
                reset_disp();
                Timer_init();
                CurState1 = STATE_KEISOKU;
                CurState2 = STATE_KEISOKU;
                Kankaku = SEC10;
                Mode = MODE_KEISOKU;
              }
              break;

            case MIN5: // 5分が選択されているとき
              if(M5.BtnA.wasPressed()) {
                M5.Lcd.drawRect(20, 60, 70, 80, BLACK); // 横, 縦, witdh, height
                M5.Lcd.drawRect(90, 60, 70, 80, RED);
                M5.Lcd.drawRect(160, 60, 70, 80, BLACK);
                M5.Lcd.drawRect(230, 60, 70, 80, BLACK);
                Kankaku_butn = SEC10;
              }
              if(M5.BtnC.wasPressed()) {
                M5.Lcd.drawRect(20, 60, 70, 80, BLACK); // 横, 縦, witdh, height
                M5.Lcd.drawRect(90, 60, 70, 80, BLACK);
                M5.Lcd.drawRect(160, 60, 70, 80, BLACK);
                M5.Lcd.drawRect(230, 60, 70, 80, RED);
                Kankaku_butn = MIN30;
              }
              if(M5.BtnB.wasPressed()) {
                num = 0;
                reset_disp();
                Timer_init();
                CurState1 = STATE_KEISOKU;
                CurState2 = STATE_KEISOKU;
                Kankaku = MIN5;
                Mode = MODE_KEISOKU;
              }
              break;

            case MIN30: // 30分が選択されているとき
              if(M5.BtnA.wasPressed()) {
                M5.Lcd.drawRect(20, 60, 70, 80, BLACK); // 横, 縦, witdh, height
                M5.Lcd.drawRect(90, 60, 70, 80, BLACK);
                M5.Lcd.drawRect(160, 60, 70, 80, RED);
                M5.Lcd.drawRect(230, 60, 70, 80, BLACK);
                Kankaku_butn = MIN5;
              }
              if(M5.BtnC.wasPressed()) {
                M5.Lcd.drawRect(20, 60, 70, 80, RED); // 横, 縦, witdh, height
                M5.Lcd.drawRect(90, 60, 70, 80, BLACK);
                M5.Lcd.drawRect(160, 60, 70, 80, BLACK);
                M5.Lcd.drawRect(230, 60, 70, 80, BLACK);
                Kankaku_butn = SEC5;
              }
              if(M5.BtnB.wasPressed()) {
                num = 0;
                reset_disp();
                Timer_init();
                CurState1 = STATE_KEISOKU;
                CurState2 = STATE_KEISOKU;
                Kankaku = MIN30;
                Mode = MODE_KEISOKU;
              }
              break;

            default:
              break;
          }
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }

  delay(200); // ボタンのチャタリング対策
  M5.update();
}
