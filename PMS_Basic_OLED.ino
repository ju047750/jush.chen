/*
 * 本範程式為Arduino結合PMS5003 PM.2.5感測模組的範例程式，由傑森創工提供
 * 本程式需配合0.96吋OLED，適用於PMS5003、PMS3003、PMS7003
 * 
 * 1，請先安裝PMS Library函式庫，下載網址：
 *    https://github.com/fu-hsi/pms
 * 
 * 購買套件：
 * https://www.jmaker.com.tw/products/pm25-project
 * 
 * 粉絲團：https://www.facebook.com/jasonshow
 * 傑森創工購物網：https://www.jmaker.com.tw/
 * 傑森創工部落格：https://blog.jmaker.com.tw/
 */
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include "PMS.h"
#include <SoftwareSerial.h>
SoftwareSerial pmsSerial(2, 3);  //RX,TX。接到PMS5003的TX,RX
PMS pms(pmsSerial);
PMS::DATA data;

int pm25=0;

//這行是NodeOLED用的
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ D2, /* data=*/ D1, /* reset=*/ U8X8_PIN_NONE); 
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  //Arduino Uno+0.96吋OLED用這行
//U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); //如果用1.3吋OLED用這行

//OLED上方標題，用圖形表現
static const unsigned char PROGMEM  title[256] = { /* 0X20,0X01,0X80,0X00,0X10,0X00, */
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X03,0XF3,0XCF,0XEF,0XFF,0X1F,0XFF,0XFD,0XFE,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC9,0XF3,0XCF,0X00,0XFE,0XC1,0XFF,0X7C,0XFE,
0X0F,0X78,0X38,0X3E,0XF8,0X3F,0XE0,0X01,0X03,0X4F,0X00,0X1E,0XF0,0XFF,0X02,0XE0,
0X8F,0X71,0X38,0X9E,0XF1,0X3F,0XE0,0X39,0XF9,0X03,0XD6,0XDE,0XF1,0X7F,0X3B,0XFB,
0X8F,0X63,0X18,0XDE,0XE3,0X3F,0XFF,0X39,0XFD,0XCF,0XD3,0XFF,0X9C,0X3F,0X98,0XFB,
0X8F,0X63,0X18,0XDE,0XF3,0X7F,0XFF,0X01,0XFF,0XCF,0X91,0X3F,0XC0,0XFF,0X98,0XF0,
0X8F,0X63,0X10,0XFE,0XF1,0X7F,0XF0,0XD9,0X03,0XCF,0X3C,0XFE,0X31,0XFF,0X02,0XE0,
0X8F,0X71,0X12,0XFE,0XF8,0X3F,0XC7,0X01,0XFE,0X0F,0XFF,0XFF,0X78,0X7E,0XB0,0XFD,
0X0F,0X78,0X22,0X7E,0XFC,0XFF,0XC7,0XFF,0XFF,0X83,0X00,0X1E,0X00,0X3C,0XB0,0XFD,
0X8F,0X7F,0X22,0X3E,0XFE,0XFF,0XC7,0X03,0XC0,0XCF,0XCF,0X3F,0XE6,0XFD,0XBB,0XFD,
0X8F,0X7F,0X22,0X1E,0XF0,0XBC,0XC7,0XB3,0XCD,0XCF,0XEF,0X7F,0X26,0X7F,0XB1,0XFD,
0X8F,0X7F,0X22,0X1E,0XF0,0X38,0XE3,0XB3,0XCD,0XCF,0XEF,0X3F,0X67,0X3E,0X95,0XED,
0X8F,0X7F,0X32,0X1E,0XF0,0X7C,0XF0,0X93,0XC8,0XCF,0X00,0X9E,0XE7,0X3C,0XC5,0XE5,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X01,0X80,0X47,0X00,0XDE,0XF1,0XBD,0XE7,0XE1,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
};

void setup()
{
  Serial.begin(9600);   
  pmsSerial.begin(9600);  
  u8g2.begin();
}

void loop()
{
  u8g2.setFont(u8g2_font_samim_16_t_all); //字型
  u8g2.firstPage();
  do {
    u8g2.drawXBMP(0,0, 128, 16, title);
    u8g2.setCursor(16, 64);
    u8g2.setFont(u8g2_font_osb41_tn);
    if (pms.read(data)){  //讀取PMS的數值
      pm25 = data.PM_AE_UG_2_5; 
      Serial.print("PM 2.5 (ug/m3): ");
      Serial.println(data.PM_AE_UG_2_5);
    }
    u8g2.print(pm25); //將讀到的PM2.5數值顯示在OLED上
    u8g2.setCursor(83, 58);
    u8g2.setFont(u8g2_font_samim_12_t_all);
    u8g2.print("ug/m3"); 
    
  } while ( u8g2.nextPage() );

}
