
#define FS_CONFIG
//兼容1.1.x版本库

#include <wifi_link_tool.h>
#include <Time.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#define PIN_LE    12  // D6  Latch 
#define PIN_CLK   13  // D7  Clock
#define PIN_DATA  14  // D5  Data
#define PIN_BL    15  // D8  Blank
#define PIN_RADAR 16   // 微波人体
#define PIN_PWR   5  //  mos管

bool show24hr = true; //24小时格式
bool leadingZero = true;   //是否显示前导0
char* ntpServerName = "ntp1.aliyun.com"; //ntp服务器域名
unsigned int sjcxsj = 40; //显示时间多少秒
unsigned int fjcxsj = 10; //日期及B数多少秒
String uid = "14010836"; //b站uid设置 0为不开启
unsigned int getsj = 10; //获取数据时间多少分钟



int multiplex_counter = 0 ; //显示索引库
const byte minsec_high[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 11};
const byte minsec_low[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11};
const byte hour_high[2][2][24] = {{{1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1, 1, 1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1, 1}, {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1}}, {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2}}};
const byte hour_low[2][2][24]  = {{{2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1}, {2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1}}, {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3}}};
int HVOUT[] = { 0,  128, 1,  64, 2,  32, 4,  8,  16, 256}; 
//每个段的HVOUT位值
#define segA 0b00000001 //OUT0
#define segF 0b00000010 //OUT1
#define segB 0b00000100 //OUT2
#define segG 0b00001000 //OUT3
#define segE 0b00010000 //OUT4
#define segC 0b00100000 //OUT5
#define segD 0b01000000 //OUT6
#define segH 0b10000000 //OUT7  

char a = 'l'; //1
char b = 'o'; //2
char c = 'a'; //3
char d = 'd'; //4
char e = ' '; //5
char f = ' '; //6
char g = ' '; //7
char h = ' '; //8
char ta, tb, tc, td, te, tf, tg, th;

IPAddress timeServerIP;


const int timeZone = 8;     // 修改北京时区
WiFiUDP Udp;

unsigned int localPort = 8266;  // 修改udp 有些路由器端口冲突时修改
int servoLift = 1500;


//粉丝数量
String  disp = "";


//等待配置网络
unsigned long previousMillis = 0;
const long interval = 500;
unsigned load_t = 0;
unsigned load_one = 0;
//每秒刷新
unsigned long previousMillis1 = 0;
const long interval1 = 1000;
//计时
unsigned int dida = 0;
//模式
unsigned int moshi = 1;
//乱码效果
unsigned long previousMillis2 = 0;
const long interval2 = 20;
unsigned int lmbj = 65; //65-90 asc
//数据更新
unsigned long previousMillis3 = 0;
long interval3 = getsj * 6000;
//检测人
unsigned int rentiyidong = 1;

void setup() {

  pinMode(PIN_LE,  OUTPUT);
  pinMode(PIN_BL,  OUTPUT); // pwm调光
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_RADAR, INPUT);
  pinMode(PIN_PWR, OUTPUT);
  digitalWrite(PIN_PWR, HIGH); // mos管
  analogWrite(PIN_BL, 10); //亮度
  Serial.begin(115200);
  //开启串口
  /////////////////////////////////////////////////////////基础设置//////////////////////////////////////////////////////////
  rstb = D3;
  //重置io
  stateled = D2;
  //指示灯io
  Hostname = "VFD";
  //设备名称 允许中文名称 不建议太长
  wxscan = true;
  //是否被小程序发现设备 开启意味该设备具有后台 true开启 false关闭
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  write_vfd_iv18(9, calculate_segments_7('!')); //显示状态



  Udp.begin(localPort);
  load();


  WiFi.hostByName(ntpServerName, timeServerIP);
  sendNTPpacket(timeServerIP);





}







void loop() {
  pant();

  if (digitalRead(PIN_RADAR) == HIGH)
  {
    digitalWrite(PIN_PWR, HIGH); //检测有无人
    rentiyidong = 1;

  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis2 >= interval2) {
    if (load_one == 1 && moshi == 1) sj(0);
    previousMillis2 = currentMillis;
  }


  if (currentMillis - previousMillis3 >= interval3) {
    getdate();
    previousMillis3 = currentMillis;

  }


  if (rentiyidong == 1) dp(); //检测到人才进行刷新


  if (link()) {

    if (load_one == 0)
    {
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        if (load_t < 4)load_t = 4;
        if (load_t > 8) {
          load_one = 1;
          getdate();
          return;
        }
        load_t++;
        switch (load_t)
        {
          case 5:
            e = '-';
            break;
          case 6:
            f = '-';
            break;
          case 7:
            g = '-';
            break;
          case 8:
            h = '-';
            break;
        }
        previousMillis = currentMillis;
        return;
      }
    } else {



      if (rentiyidong == 1) {

        //检测到人才进行模式切换
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis1 >= interval1) {
          dida++;

          switch (moshi) {
            case 1:
              lmbj = 65;
              sj(1);

              if (dida > sjcxsj) {
                dida = 0;
                moshi = 2;
              }
              break;

            case 2:
              rq();
              if (dida > fjcxsj) {
                dida = 0;
                if (uid != "") {
                  moshi = 3;
                }
                else {
                  moshi = 1;
                  if (digitalRead(PIN_RADAR) == 0)
                  {
                    digitalWrite(PIN_PWR, LOW); //关闭供电
                    rentiyidong = 0;
                    Serial.println("关闭供电");

                  }
                }
              }
              break;

            case 3:
              bf();
              if (dida > fjcxsj) {
                dida = 0;
                moshi = 1;

                if (digitalRead(PIN_RADAR) == 0)
                {
                  digitalWrite(PIN_PWR, LOW); //关闭供电
                  rentiyidong = 0;
                  Serial.println("关闭供电");
                }

              }
              break;

          }




          previousMillis1 = currentMillis;
        }


      }


    }



  } else {

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      if (load_t == 8)
      {
        a = ' '; //1
        b = ' '; //2
        c = ' '; //3
        d = ' '; //4
        e = ' '; //5
        f = ' '; //6
        g = ' '; //7
        h = ' '; //8

        load_t = 0;


      }
      load_t++;

      switch (load_t)
      { case 1:
          a = '-';
          break;
        case 2:
          b = '-';
          break;
        case 3:
          c = '-';
          break;
        case 4:
          d = '-';
          break;
        case 5:
          e = '-';
          break;
        case 6:
          f = '-';
          break;
        case 7:
          g = '-';
          break;
        case 8:
          h = '-';
          break;
      }

      previousMillis = currentMillis;
    }


  }

}























////////////////////////////////数据获取部分//////////////////////////////////////////////////
void getdate() {
  write_vfd_iv18(9, calculate_segments_7('!')); //显示状态
  setSyncProvider(getNtpTime);

  write_vfd_iv18(9, calculate_segments_7('8')); //显示状态
  if (uid != "") {
    DynamicJsonBuffer jsonBuffer;
    WiFiClient client;
    HTTPClient http;
    if (http.begin(client, "http://api.bilibili.com/x/relation/stat?vmid=" + uid)) {
      int httpCode = http.GET();
      String payload = http.getString();
      http.end();
      JsonObject& res_json = jsonBuffer.parseObject(payload);
      String follower = res_json["data"]["follower"];
      if (follower != "") disp = follower;
      Serial.println("目前粉丝：" + String(disp));
    }



  }



}



void bf() {


  switch (disp.length()) {
    case 1:
      a = ' ';
      b = ' ';
      c = ' ';
      d = disp.toInt();
      e = ' ';
      f = ' ';
      g = ' ';
      h = ' ';
      break;
    case 2:
      a = '-';
      b = '-';
      c = '-';
      d = disp.charAt(0) - 48;
      e = disp.charAt(1) - 48;
      f = '-';
      g = '-';
      h = '-';
      break;
    case 3:
      a = ' ';
      b = ' ';
      c = ' ';
      d = disp.charAt(0) - 48;
      e = disp.charAt(1) - 48;
      f = disp.charAt(2) - 48;
      g = ' ';
      h = ' ';
      break;
    case 4:
      a = '-';
      b = '-';
      c = disp.charAt(0) - 48;
      d = disp.charAt(1) - 48;
      e = disp.charAt(2) - 48;
      f = disp.charAt(3) - 48;
      g = '-';
      h = '-';
      break;
    case 5:
      a = ' ';
      b = disp.charAt(0) - 48;
      c = disp.charAt(1) - 48;
      d = disp.charAt(2) - 48;
      e = disp.charAt(3) - 48;
      f = disp.charAt(4) - 48;
      g = ' ';
      h = ' ';
      break;
    case 6:
      a = ' ';
      b = disp.charAt(0) - 48;
      c = disp.charAt(1) - 48;
      d = disp.charAt(2) - 48;
      e = disp.charAt(3) - 48;
      f = disp.charAt(4) - 48;
      g = disp.charAt(5) - 48;
      h = ' ';
      break;
    case 7:
      a = ' ';
      b = disp.charAt(0) - 48;
      c = disp.charAt(1) - 48;
      d = disp.charAt(2) - 48;
      e = disp.charAt(3) - 48;
      f = disp.charAt(4) - 48;
      g = disp.charAt(5) - 48;
      h = disp.charAt(6) - 48;
      break;
    case 8:
      a = disp.charAt(0) - 48;
      b = disp.charAt(1) - 48;
      c = disp.charAt(2) - 48;
      d = disp.charAt(3) - 48;
      e = disp.charAt(4) - 48;
      f = disp.charAt(5) - 48;
      g = disp.charAt(6) - 48;
      h = disp.charAt(7) - 48;
      break;

  }




}








void sj(int pd) {
  c = '-';
  f = '-';

  if (pd == 1) {

    ta = hour_high[show24hr][leadingZero][hour()];
    tb = hour_low[show24hr][leadingZero][hour()];

    td = minsec_high[minute()];
    te = minsec_low[minute()];

    tg = minsec_high[second()];
    th = minsec_low[second()];
    lmbj = 65;



  } else {


    if (lmbj < 91) {


      if (ta != a) {
        a = lmbj;
      }
      if (tb != b) {
        b = lmbj;
      }
      if (td != d) {
        d = lmbj;
      }
      if (te != e) {
        e = lmbj;
      }
      if (tg != g) {
        g = lmbj;
      }
      if (th != h) {
        h = lmbj;
      }
      lmbj++;

      return;
    } else if (lmbj == 91) {
      lmbj++;
      a = hour_high[show24hr][leadingZero][hour()];
      b = hour_low[show24hr][leadingZero][hour()];

      d = minsec_high[minute()];
      e = minsec_low[minute()];

      g = minsec_high[second()];
      h = minsec_low[second()];



    }


  }









}

void rq() {
  a = year() / 10 % 10;
  b = year() % 10;
  c = '*';
  if (month() < 10) {
    d = '0';
    e = month();
  } else {
    d = month() / 10 % 10;
    e = month() % 10;
  }
  f = '*';
  if (day() < 10) {
    g = '0';
    h = day();
  } else {
    g = day() / 10 % 10;
    h = day() % 10;
  }
}







////////////////////////////////驱动部分//////////////////////////////////////////////////


void dp() {                           //多路复用主程序

  write_vfd_iv18(8, calculate_segments_7(a));
  delayMicroseconds(3);
  write_vfd_iv18(7, calculate_segments_7(b));
  delayMicroseconds(3);
  write_vfd_iv18(6, calculate_segments_7(c));
  delayMicroseconds(3);
  write_vfd_iv18(5, calculate_segments_7(d));
  delayMicroseconds(3);
  write_vfd_iv18(4, calculate_segments_7(e));
  delayMicroseconds(3);
  write_vfd_iv18(3, calculate_segments_7(f));
  delayMicroseconds(3);
  write_vfd_iv18(2, calculate_segments_7(g));
  delayMicroseconds(3);
  write_vfd_iv18(1, calculate_segments_7(h));
  delayMicroseconds(5);
}

//0-9
char segments_7n[] = {
  segA + segB + segC + segD + segE + segF ,  //0
  segB + segC, //1
  segA + segB + segD + segE + segG, //2
  segA + segB + segC + segD + segG, //3
  segB + segC + segF + segG, //4
  segA + segC + segD + segF + segG, //5 S
  segA + segC + segD + segE + segF + segG, //6
  segA + segB + segC, //7
  segA + segB + segC + segD + segE + segF + segG, //8
  segA + segB + segC + segD + segF + segG, //9
};

// A-Z
char segments_7a[] = {
  segA + segB + segC + segE + segF + segG, //10 A
  segC + segD + segE + segF + segG, //' ' B
  segA + segD + segE + segF, //12 C
  segB + segC + segD + segE + segG, //13 D
  segA + segD + segE + segF + segG, //14 E
  segA + segE + segF + segG, //15 F
  segA + segC + segD + segE + segF, //G
  segB + segC + segE + segF + segG, //H
  segB + segC, // I
  segB + segC + segD + segE, //J
  segB + segE + segF + segG, //K
  segD + segE + segF, //L
  segA + segC + segE + segG, //M
  segC + segE + segG, //N
  segA + segB + segC + segD + segE + segF,  //0
  segA + segB + segE + segF + segG, //P
  segA + segB + segC + segF + segG, //Q
  segE + segG, //R
  segA + segC + segD + segF + segG, //5 S
  segD + segE + segF + segG, //T
  segB + segC + segD + segE + segF, //U
  segC + segD + segE, //V
  segA + segD + segG, //W
  segA + segD + segG, //X
  segB + segC + segD + segF + segG, //Y
  segD + segG//Z
};


// 分段计算
uint8_t calculate_segments_7(uint8_t ch)
{
  uint8_t segs = 0;
  if ((ch >= 0) && (ch <= 9))           
    segs = segments_7n[ch];
  else if ((ch >= '0') && (ch <= '9'))  
    segs = segments_7n[ch - 48];
  else if ((ch >= 'A') && (ch <= 'Z'))  // A-Z
    segs = segments_7a[ch - 'A'];
  else if ((ch >= 'a') && (ch <= 'z'))  // a-z
    segs = segments_7a[ch - 'a'];
  else if (ch == '-')
    segs = segG;
  else if (ch == '"')
    segs = segB + segF;
  else if (ch == 0x27)  // "'"
    segs = segB;
  else if (ch == '_')
    segs = segD;
  else if (ch == '*')
    segs = segH;
  else if (ch == '!')
    segs = segH + segB;
  else
    segs = 0;
  return segs;
}

// HV5812 驱动
void write_vfd_8bit(uint8_t data)
{
  // MSB 移位
  for (uint8_t i = 0; i < 8; i++)  {
    if (!!(data & (1 << (7 - i))))
      digitalWrite(PIN_DATA, HIGH);
    else
      digitalWrite(PIN_DATA, LOW);

    digitalWrite(PIN_CLK, HIGH);
    delayMicroseconds(1);
    digitalWrite(PIN_CLK, LOW);
  }
}

void write_vfd_iv18(uint8_t digit, uint8_t segments)
{

  uint32_t val = ((HVOUT[digit]) << 11) | ((uint32_t)segments );
  write_vfd_8bit(0);
  write_vfd_8bit(val >> 16);
  write_vfd_8bit(val >> 8);
  write_vfd_8bit(val);

  digitalWrite(PIN_LE, HIGH);
  delayMicroseconds(1);
  digitalWrite(PIN_LE, LOW);
}



//////////////////////////////////////////////////////////////////解包设置//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void digitalClockDisplay() {

  Serial.print(hour());
  printDigits(minute());
  Serial.println();

}
void printDigits(int digits) {
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


const int NTP_PACKET_SIZE = 48; 
byte packetBuffer[NTP_PACKET_SIZE];

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; 
  Serial.println("连接时间服务器");
  sendNTPpacket(timeServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("时间服务器应答");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  
      unsigned long secsSince1900;
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("获取失败:-(");
  return 0; 
}


void sendNTPpacket(IPAddress & address)
{
 
  memset(packetBuffer, 0, NTP_PACKET_SIZE);

  packetBuffer[0] = 0b11100011;  
  packetBuffer[1] = 0;
  packetBuffer[3] = 0xEC;

  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  Udp.beginPacket(address, 123);
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
