#include <SPI.h>
#include <Adafruit_GFX.h>
#include "ST7789_AVR.h"
#include <SoftwareSerial.h>
#include <TinyGPS++.h> 
#include <TimeLib.h>
#include "uUnixDate.h"

#define TFT_DC   5
#define TFT_CS  -1
#define TFT_RST  6
#define SCR_WD 240
#define SCR_HT 240
#define GPS_RX  3
#define GPS_TX  2
#define OFFSET 3

int text_color = BLUE;
int c_code = 0;
int lhour = 0;
int lmin = 0 ;
int lsec = 0;
int lsech = 0;
int lsecl = 0;
int last_sec = 0;
int lday_num = 2;
int lday = 1;
int lmon_num = 0;
int lyear = 2025;
char ltime[6] = "01 01";
char days[7][26] = {"воскресенье", "понедельник", "  вторник", "   среда", "  четверг", "  пятница", "  суббота"};
char mons[12][8] = {"ЯНВ", "ФЕВ", "МАР", "АПР", "МАЙ", "ИЮН", "ИЮЛ", "АВГ", "СЕН", "ОКТ", "НОЯ", "ДЕК"};
char date[20] = "01ЯНВ2025";
bool update_all = false;
bool update_gps = true;
char serial_buffer;
char numsats[26] = "спутников 0";
TinyGPSPlus gps;

ST7789_AVR lcd = ST7789_AVR(TFT_DC, TFT_RST, TFT_CS);
SoftwareSerial SoftSerial(GPS_TX, GPS_RX);

void setup() {
  setTime(1, 1, 1, 1, 1, 2025);

  lsec = second();
  last_sec = lsec;
  lmin = minute();
  lhour = hour();
  lday = day();
  lday_num = weekday() - 1; 
  lmon_num = month() - 1;
  lyear = year();

  sprintf(ltime, "%02i %02i", lhour, lmin);
  sprintf(date, "%02i%s%4i", lday, mons[lmon_num], lyear);

  Serial.begin(9600);
  Serial.println("Start clock");

  pinMode(7, INPUT_PULLUP);

  lcd.init();
  lcd.fillScreen(BLACK);
  
  lcd.setCursor(0, 30);
  lcd.setTextColor(BLUE, BLACK);
  lcd.setTextSize(8);
  lcd.println(ltime);

  lcd.setCursor(112, 40);
  lcd.setTextColor(BLUE, BLACK);
  lcd.setTextSize(2);
  lcd.println(lsech);
  lcd.setCursor(112, 70);
  lcd.setTextColor(BLUE, BLACK);
  lcd.setTextSize(2);
  lcd.println(lsecl);

  lcd.setCursor(15, 110);
  lcd.setTextColor(BLUE, BLACK);
  lcd.setTextSize(3);
  lcd.println(utf8rus(days[lday_num]));

  lcd.setCursor(10, 170);
  lcd.setTextColor(BLUE, BLACK);
  lcd.setTextSize(4);
  lcd.println(utf8rus(date));

  /*lcd.fillRect(150, 220, 240, 240, BLACK);
  lcd.setCursor(160, 230);
  lcd.setTextColor(text_color, BLACK);
  lcd.setTextSize(1);
  lcd.println(utf8rus(numsats));*/
  
  SoftSerial.begin(9600);
}

void loop() {
  //delay(100);

  while (SoftSerial.available() > 0)  {
    serial_buffer = SoftSerial.read();
    Serial.write(serial_buffer);
    if (gps.encode(serial_buffer)) {
      if (update_gps && gps.time.isValid() && gps.date.day() != 0) {
        uUnixDate dateGPS = uUnixDate(gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());
        uUnixDate dateTZ = uUnixDate(dateGPS.timestamp() + 3600 * OFFSET); 
        setTime(dateTZ.hour(), dateTZ.minute(), dateTZ.second(), dateTZ.day(), dateTZ.month(), dateTZ.year());
        //setTime(gps.time.hour() + OFFSET, gps.time.minute(), gps.time.second(), gps.date.day(), gps.date.month(), gps.date.year());
        //adjustTime(offset * SECS_PER_HOUR);
        //sprintf(numsats, "спутников %i", gps.satellites.value());
        update_gps = false;
      }
    }
  }

  lsec = second();
  lmin = minute();
  lhour = hour();
  lday = day();
  lday_num = weekday() - 1; 
  lmon_num = month() - 1;
  lyear = year();
  
  if (lsec == 0) {
    update_all = true;
    update_gps = true;
  }

  if (digitalRead(7) == 0) {
    c_code++;
    if (c_code > 4) {
      c_code = 0;
    }
    switch (c_code) {
      case 0:
        text_color = BLUE;
        break;
      case 1:
        text_color = GREEN;
        break;
      case 2:
        text_color = RED;
        break;
      case 3:
        text_color = YELLOW;
        break;
      case 4:
        text_color = WHITE;
        break;
    }
    update_all = true;
  }

  lsech = (int) lsec / 10;
  lsecl = lsec % 10;

  if (update_all && timeStatus()== timeSet) {
    sprintf(ltime, "%02i %02i", lhour, lmin);
    sprintf(date, "%02i%s%4i", lday, mons[lmon_num], lyear);

    lcd.fillRect(0, 0, 240, 100, BLACK);
    lcd.setCursor(0, 30);
    lcd.setTextColor(text_color, BLACK);
    lcd.setTextSize(8);
    lcd.println(ltime); 

    lcd.fillRect(0, 110, 240, 50, BLACK);
    lcd.setCursor(15, 110);
    lcd.setTextColor(text_color, BLACK);
    lcd.setTextSize(3);
    lcd.println(utf8rus(days[lday_num])); 

    lcd.fillRect(10, 170, 240, 70, BLACK);
    lcd.setCursor(10, 170);
    lcd.setTextColor(text_color, BLACK);
    lcd.setTextSize(4);
    lcd.println(utf8rus(date));

    /*lcd.fillRect(150, 220, 240, 240, BLACK);
    lcd.setCursor(160, 230);
    lcd.setTextColor(text_color, BLACK);
    lcd.setTextSize(1);
    lcd.println(utf8rus(numsats));*/

    update_all = false;
  }

  if (last_sec != lsec) {
    lcd.fillRect(105, 30, 25, 80, BLACK);
    lcd.setCursor(112, 40);
    lcd.setTextColor(text_color, BLACK);
    lcd.setTextSize(2);
    lcd.println(lsech);
    lcd.setCursor(112, 70);
    lcd.setTextColor(text_color, BLACK);
    lcd.setTextSize(2);
    lcd.println(lsecl);

    last_sec = lsec;
  }
}

String utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) {
    n = source[i]; i++;
    if (n >= 0xBF){
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x2F;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB7; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x6F;
            break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
  return target;
}