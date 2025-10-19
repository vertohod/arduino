#include <PS2KeyRaw.h>
#define AX0 4
#define AX1 5
#define AX2 6
#define AX3 7
#define AY0 8
#define AY1 9
#define AY2 10
#define RSTMT 14
#define CSMT  15
#define DATMT 16
#define STBMT 17
#define DATAPIN 2
#define IRQPIN  3

uint8_t data = HIGH;

uint8_t table[128] = {
    0x7F, 0x7F, 0x7F, 0x7F, // 0x00
    0x7F, 0x7F, 0x7F, 0x7F, // 0x04
    0x7F, 0x7F, 0x7F, 0x7F, // 0x08
    0x7F, 0x7F, 0x7F, 0x7F, // 0x0C
    0x7F, 0x7F, 0x00, 0x7F, // 0x10
    0x7F, 0x20, 0x30, 0x7F, // 0x14
    0x7F, 0x7F, 0x01, 0x11, // 0x18
    0x10, 0x21, 0x31, 0x7F, // 0x1C
    0x7F, 0x03, 0x02, 0x12, // 0x20
    0x22, 0x33, 0x32, 0x7F, // 0x24
    0x7F, 0x70, 0x04, 0x13, // 0x28
    0x24, 0x23, 0x34, 0x7F, // 0x2C
    0x7F, 0x73, 0x74, 0x64, // 0x30
    0x14, 0x54, 0x44, 0x7F, // 0x34
    0x7F, 0x7F, 0x72, 0x63, // 0x38
    0x53, 0x43, 0x42, 0x7F, // 0x3C
    0x7F, 0x7F, 0x62, 0x52, // 0x40
    0x51, 0x40, 0x41, 0x7F, // 0x44
    0x7F, 0x7F, 0x7F, 0x61, // 0x48
    0x7F, 0x59, 0x7F, 0x7F, // 0x4C
    0x7F, 0x7F, 0x7F, 0x7F, // 0x50
    0x7F, 0x7F, 0x7F, 0x7F, // 0x54
    0x7F, 0x71, 0x60, 0x7F, // 0x58
    0x7F, 0x7F, 0x7F, 0x7F, // 0x5C
    0x7F, 0x7F, 0x7F, 0x7F, // 0x60
    0x7F, 0x7F, 0x40, 0x7F, // 0x64
    0x7F, 0x7F, 0x7F, 0x34, // 0x68
    0x7F, 0x7F, 0x7F, 0x7F, // 0x6C
    0x7F, 0x7F, 0x7F, 0x7F, // 0x70
    0x7F, 0x7F, 0x7F, 0x7F, // 0x74
    0x7F, 0x7F, 0x7F, 0x7F, // 0x78
    0x7F, 0x7F, 0x7F, 0x7F  // 0x7C
};

PS2KeyRaw keyboard;
void setup() {
  //Инициализация портов
  pinMode(AX0, OUTPUT);   //AX0
  pinMode(AX1, OUTPUT);   //AX1
  pinMode(AX2, OUTPUT);   //AX2
  pinMode(AX3, OUTPUT);   //AX3
  pinMode(AY0, OUTPUT);   //AY0
  pinMode(AY1, OUTPUT);   //AY1
  pinMode(AY2, OUTPUT);   //AY2
  pinMode(RSTMT, OUTPUT); //RES
  pinMode(CSMT, OUTPUT);  //CS
  pinMode(DATMT, OUTPUT); //DAT
  pinMode(STBMT, OUTPUT); //STB
  //Инициализация клавиатуры
  keyboard.begin( DATAPIN, IRQPIN );
  //Инициализация MT8816
  SetAddr(0);
  digitalWrite(RSTMT, LOW);
  digitalWrite(CSMT, LOW);
  digitalWrite(DATMT, LOW);
  digitalWrite(STBMT, LOW);
  InitMt();
}
//Сброс MT
void InitMt() {
  digitalWrite(STBMT, HIGH); //инициализация
  digitalWrite(CSMT, HIGH); //выбор чипа
  digitalWrite(RSTMT, HIGH);
  digitalWrite(RSTMT, LOW);  //сброс
  digitalWrite(CSMT, LOW);
  digitalWrite(STBMT, LOW);
}
void SetAddr(uint8_t addr) {
  digitalWrite(AX0,bitRead(addr,0));
  digitalWrite(AX1,bitRead(addr,1));
  digitalWrite(AX2,bitRead(addr,2));
  digitalWrite(AX3,bitRead(addr,3));
  digitalWrite(AY0,bitRead(addr,4));
  digitalWrite(AY1,bitRead(addr,5));
  digitalWrite(AY2,bitRead(addr,6));
}
void SetKey(uint8_t d){
   digitalWrite(CSMT, HIGH); //выбор чипа
   digitalWrite(STBMT, HIGH); //строб on
   digitalWrite(DATMT, d); //данные
   digitalWrite(STBMT, LOW); //строб off    
   digitalWrite(CSMT, LOW);
}
void loop()
{
  if (keyboard.available() > 0) {
    int c = keyboard.read(); //чтение кода
    if (c == 0xF0) {
      data = LOW;
      return;
    }
    switch (c) {
      case 0xE1:              //Если считался префикс 0xE1 сброс MK
        InitMt();
        break;
      case 0xE0:              //если считался префикс 0xE0
        break;
      case 0x66:              //если считался код 0x66 [BS]
        SetAddr(table[0x12]);
        SetKey(data);
        SetAddr(table[0x45]);
        SetKey(data);
        break;
      case 0x6B:              //если считался код 0x6B [Left]
        SetAddr(table[0x12]);
        SetKey(data);
        SetAddr(table[0x2E]);
        SetKey(data);
        break;
      case 0x72:              //если считался код 0x72 [Down]
        SetAddr(table[0x12]);
        SetKey(data);
        SetAddr(table[0x36]);
        SetKey(data);
        break;
      case 0x75:              //если считался код 0x75 [Up]
        SetAddr(table[0x12]);
        SetKey(data);
        SetAddr(table[0x3D]);
        SetKey(data);
        break;
      case 0x74:              //если считался код 0x74 [Right]
        SetAddr(table[0x12]);
        SetKey(data);
        SetAddr(table[0x3E]);
        SetKey(data);
        break;
      case 0x76:              //если считался код 0x76 [Edit]
        SetAddr(table[0x12]);
        SetKey(data);
        SetAddr(table[0x16]);
        SetKey(data);
        break;
      case 0x58:              //если считался код 0x58 [Caps lock]
        SetAddr(table[0x12]);
        SetKey(data);
        SetAddr(table[0x1E]);
        SetKey(data);
        break;
      case 0x0D:              //если считался код 0x0D [Ext mode]
        SetAddr(table[0x12]);
        SetKey(data);
        SetAddr(table[0x59]);
        SetKey(data);
        break;
      case 0x41:              //если считался код 0x41 [,]
        SetAddr(table[0x59]);
        SetKey(data);
        SetAddr(table[0x31]);
        SetKey(data);
        break;
      case 0x49:              //если считался код 0x49 [.]
        SetAddr(table[0x59]);
        SetKey(data);
        SetAddr(table[0x3A]);
        SetKey(data);
        break;
      case 0x4C:              //если считался код 0x4C [;]
        SetAddr(table[0x59]);
        SetKey(data);
        SetAddr(table[0x44]);
        SetKey(data);
        break;
      case 0x52:              //если считался код 0x52 ["]
        SetAddr(table[0x59]);
        SetKey(data);
        SetAddr(table[0x4D]);
        SetKey(data);
        break;
      default:  
        SetAddr(table[c]);
        SetKey(data);
    }
    data = HIGH;
  }
}
