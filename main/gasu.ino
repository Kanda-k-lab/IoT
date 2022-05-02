static String gasu_data[3] = {};
static String moji_data;

void gasu_Init() {
  //Serial2.begin(9600, SERIAL_8N1, 16, 17);
  Serial2.begin(9600, SERIAL_8N1, 36, 26);
}

void split(String data, String bun){ // ガステックのシリアル2から取得したデータを酸素と二酸化炭素で分ける
  int count = 0;
  int len = data.length();
  String words;
  if(len == 10) {
    for(int i = 0; i < len; i++){
      if(data.charAt(i) == ','){
        gasu_data[count] = words;
        count += 1;
        words = "";
      }else{
        words.concat(data.charAt(i));
      }
    }
    gasu_data[count] = words;
  }
  
}

void get_gasu() {
  
  if (Serial2.available()) {
    moji_data = Serial2.readStringUntil('\n');
    split(moji_data, ",");
    /*
    Serial.print(gasu_data[0]);
    Serial.print(" ");
    Serial.print(gasu_data[1]);
    Serial.print(" ");
    Serial.println(gasu_data[2]);*/
  }
}

float get_o2() {
  return gasu_data[1].toFloat();
}

float get_co2() {
  return gasu_data[2].toFloat();
}
