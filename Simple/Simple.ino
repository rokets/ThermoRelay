#include <OneWire.h>


OneWire ds(10); // на пине 10 (нужен резистор 4.7 КОм)
byte DS1[8] = {0x28, 0xFF, 0x72, 0x70, 0x86, 0x16, 0x5, 0xC4}; // Адрес 1го датчика
byte DS2[8] = {0x28, 0xFF, 0x88, 0x81, 0x85, 0x16, 0x4, 0x9B}; // Адрес 2го датчика

// Реле модуль подключен к цифровому выводу 4
byte Relay = 4;
byte i;

void setup(void)
{
  Serial.begin(9600);
  pinMode(Relay, OUTPUT);  // Инициализируем порт для работы с реле
}

void loop(void)
{

  byte data1[12]; // Данные 1го датчика. Всего доступно 12 байт, но используется для чтения 9
  byte data2[12]; // Данные 2го датчика


  // Передаём команду получить температуру для 1го датчика
  ds.reset();
  ds.select(DS1);
  ds.write(0x44); // начинаем преобразование, используя ds.write(0x44,1) с "паразитным" питанием

  // Передаём команду получить температуру для 2го датчика
  ds.reset();
  ds.select(DS2);
  ds.write(0x44); // начинаем преобразование, используя ds.write(0x44,1) с "паразитным" питанием

  // Задержка 750-1000 мс, время необходимое для считывания температуры
  delay(750);

  //
  ds.reset();
  ds.select(DS1);
  ds.write(0xBE);
  for ( i = 0; i < 9; i++)
  {
    data1[i] = ds.read();
  }

  ds.reset();
  ds.select(DS2);
  ds.write(0xBE);
  for ( i = 0; i < 9; i++)
  {
    data2[i] = ds.read();
  }

  int16_t tmp1 = (data1[1] << 8) | data1[0];
  int16_t tmp2 = (data2[1] << 8) | data2[0];

  // при маленьких значениях, малые биты не определены, их надо обнулять
  // Обнуление 1го датчика
  switch (data1[4] & 0x60) {
    case 0x00: // разрешение 9 бит, 93.75 мс
      tmp1 = ((data1[1] << 8) | data1[0]) & ~7;
      break;
    case 0x20: // разрешение 10 бит, 187.5 мс
      tmp1 = ((data1[1] << 8) | data1[0]) & ~3;
      break;
    case 0x40: // разрешение 11 бит, 375 мс
      tmp1 = ((data1[1] << 8) | data1[0]) & ~1;
      break;
    default:
      tmp1 = (data1[1] << 8) | data1[0];
      // разрешение по умолчанию равно 12 бит, время преобразования - 750 мс
  }
  // Обнуление 2го датчика
  switch (data2[4] & 0x60) {
    case 0x00:
      tmp2 = ((data2[1] << 8) | data2[0]) & ~7;
      break;
    case 0x20:
      tmp2 = ((data2[1] << 8) | data2[0]) & ~3;
      break;
    case 0x40:
      tmp2 = ((data2[1] << 8) | data2[0]) & ~1;
      break;
    default:
      tmp2 = (data2[1] << 8) | data2[0];
  }

  Serial.print("Temperature 1 = ");
  Serial.print((float)tmp1 / 16.0);
  Serial.print(" C,  Temperature 2 = ");
  Serial.print((float)tmp2 / 16.0);
  Serial.print(" C, Delta = ");
  Serial.print(((float)tmp1 - (float)tmp2) / 16.0);
  Serial.println(" C");

  if ((tmp1 - tmp2) > 10)
  {
    digitalWrite(Relay, LOW);   // реле включено
  }
  else
  {
    digitalWrite(Relay, HIGH);  // реле выключено
  }
}
