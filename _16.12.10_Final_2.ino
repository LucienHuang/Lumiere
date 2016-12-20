#include <Wire.h>
#include <Adafruit_DotStar.h>
#include <SPI.h>

const int MPU_addr = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

int state = 0;
bool power = false;
bool state1Started = false;
float theta = 0;
float breath, heartbeat;

#define NUMPIXELS 60
#define DATAPIN 4
#define CLOCKPIN 5

Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
}

uint32_t color = 0x000000;
uint32_t color1 = 0x000000;
uint32_t color2 = 0x000000;
uint32_t color3 = 0x000000;
uint32_t color4 = 0x000000;
uint32_t color5 = 0x000000;
uint32_t color6 = 0x000000;
uint32_t color7 = 0x000000;

//uint32_t breath = 0x000000;
uint32_t blue = 0x00;
uint32_t green = 0x00;
uint32_t red = 0xFF;
uint32_t spd1 = 0x0F;

int phase = 0;
int brt = 8;
int ran = 0;

int head = 0;
int last = 18;
unsigned long lastChangeTime = 0;

void loop() {
  // put your main code here, to run repeatedly:
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);

  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  if (AcZ > 13000 && abs(AcX) < 5000 && abs(AcY) < 5000) {
    if (state1Started == false) {
      blue = 0x00;
      green = 0x00;
      red = 0xFF;
      state1Started = true;
    }
    state = 1;
  } else if (AcZ < -13000 && abs(AcX) < 5000 && abs(AcY) < 5000) {
    state1Started = false;
    state = 2;
  } else if (AcX > 11000 && abs(AcY) < 7000 && abs(AcZ) < 7000) {
    state1Started = false;
    state = 3;
  } else if (AcX < -11000 && abs(AcY) < 7000 && abs(AcZ) < 7000) {
    state1Started = false;
    state = 4;
  } else if (AcY > 13000 && abs(AcX) < 5000 && abs(AcZ) < 5000) {
    state1Started = false;
    state = 5;
  } else if (AcY < -13000 && abs(AcX) < 5000 && abs(AcZ) < 5000) {
    state1Started = false;
    state = 6;
  } else {
    state = 0;
  }


  if (state == 0) {
    phase = 0;
  } else if (state == 1) {
    //state 1
    if (GyZ > 10000) {
      power = false;
    } else if (GyZ < -10000 && power == false) {
      power = true;
    } else if (GyZ < -10000 && power == true) {
      if (phase < 4) phase++;
      else phase = 0;
      state1Started = false;
      delay(300);
    }

    if (phase == 0) {
      spd1 = 0x0F;
    } else if (phase == 1) {
      spd1 = 0x33;
    } else if (phase == 2) {
      spd1 = 0x55;
    } else if (phase == 3) {
      spd1 = 0x05;
    }

    if (red == 0xFF && green < 0xFF && blue == 0x00) {
      green += spd1;
    } else if (red > 0x00 && green == 0xFF && blue == 0x00) {
      red -= spd1;
    } else if (red == 0x00 && green == 0xFF && blue < 0xFF) {
      blue += spd1;
    } else if (red == 0x00 && green > 0x00 && blue == 0xFF) {
      green -= spd1;
    } else if (red < 0xFF && green == 0x00 && blue == 0xFF) {
      red += spd1;
    } else if (red == 0xFF && green == 0x00 && blue > 0x00) {
      blue -= spd1;
    } else {
      state1Started = false;
    }

    color = blue + green * 0x010000 + red * 0x0100;

    //state 1 end

  } else if (state == 2) {
    //state 2
    if (GyZ > 10000 && power == false) {
      power = true;
    } else if (GyZ < -10000) {
      power = false;
    } else if (GyZ > 10000 && power == true) {
      if (phase < 4) phase++;
      else phase = 0;
      delay(300);
    }

    if (phase == 0) {
      color = 0x4BFF25 ;
      color1 = 0x060004;
      color2 = 0x0100;
    } else if (phase == 1) {
      color = 0x351AFF ;
      color1 = 0x060400;
      color2 = 0x01;
    } else if (phase == 2) {
      color = 0xFEFF30 ;
      color1 = 0x05;
      color2 = 0x00;
    } else if (phase == 3) {
      color = 0xFF2A97 ;
      color1 = 0x0505;
      color2 = 0x00;
    }

    //state 2 end
  } else if (state == 3) {
    //state 3
    if (GyX > 10000) {
      power = false;
    } else if (GyX < -10000 && power == false) {
      power = true;
    } else if (GyX < -10000 && power == true) {
      if (phase < 4) phase++;
      else phase = 0;
      delay(300);
    }

    if (phase == 0) {
      color = 0x110011;
    } else if (phase == 1) {
      color = 0x061100;
    } else if (phase == 2) {
      color = 0x111100;
    } else if (phase == 3) {
      color = 0x080211;
    }

    //state 3 end
  } else if (state == 4) {
    //state 4
    if (GyX > 10000 && power == false) {
      power = true;
    } else if (GyX < -10000) {
      power = false;
    } else if (GyX > 10000 && power == true) {
      if (phase < 4) phase++;
      else phase = 0;
      delay(300);
    }


    if (phase == 0) {
      color = 0x1AFF3E;
      color1 = 0x00;
    } else if (phase == 1) {
      color = 0x83FF11;
      color1 = 0x0D0000;
    } else if (phase == 2) {
      color = 0xD900FF;
      color1 = 0x1100;
    } else if (phase == 3) {
      color = 0xFFE66D;
      color1 = 0x11;
    }

    //state 4 end
  } else if (state == 5) {
    //state 5
    if (GyY > 10000) {
      power = false;
    } else if (GyY < -10000 && power == false) {
      power = true;
    } else if (GyY < -10000 && power == true) {
      if (phase < 4) phase++;
      else phase = 0;
      delay(300);
    }

    if (phase == 0) {
      brt = 9;
    } else if (phase == 1) {
      brt = 7;
    } else if (phase == 2) {
      brt = 5;
    } else if (phase == 3) {
      brt = 3;
    }


    //state 5 end
  } else if (state == 6) {
    //state 6
    if (GyY > 10000 && power == false) {
      power = true;
    } else if (GyY < -10000) {
      power = false;
    } else if (GyY > 10000 && power == true) {
      if (phase < 4) phase++;
      else phase = 0;
      delay(300);
    }


    if (phase == 0) {
      color1 = 0x00FF00;
      color2 = 0x91FF00;
      color3 = 0xFFFF00;
      color4 = 0xFF0000;
      color5 = 0xFF00FF;
      color6 = 0x0000FF;
      color7 = 0x0092FF;
    } else if (phase == 1) {
      color1 = 0xDE6CFF;
      color2 = 0xE866BF;
      color3 = 0xFF7D8C;
      color4 = 0xE8AD66;
      color5 = 0xF5FF68;
      color6 = 0x9FE807;
      color7 = 0x6EFF01;
    } else if (phase == 2) {
      color1 = 0x95FFD4;
      color2 = 0x8BCAE8;
      color3 = 0xA6B2FF;
      color4 = 0xAF8BE8;
      color5 = 0xF591FF;
      color6 = 0xE878CA;
      color7 = 0xFF8094;
    } else if (phase == 3) {
      color1 = 0x41A5FF;
      color2 = 0x49E2E8;
      color3 = 0x5DFF88;
      color4 = 0x6EE857;
      color5 = 0x92FF4C;
      color6 = 0x82E82D;
      color7 = 0xB2FF1E;
    }
    //state 6 end
  }

  //  Serial.print(power && state == 2);
  Serial.println(state);

  if (power && state == 1) {
    //state 1 power
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, color);
      strip.show();
    }
    //state 1 end power
  } else if (power && state == 2) {
    //state 2 power
    ran = random(50);
    for (int i = 0; i < NUMPIXELS; i++) {
      if (i < 10 + ran) {
        strip.setPixelColor(i, color + color1 * i - color2 * i);
      } else {
        strip.setPixelColor(i, 0);
      }
      strip.show();
    }
    //state 2 end power
  } else if (power && state == 3) {
    //state 3 power

    head++;
    if (head > 14) head = 0;

    for (int i = -14; i < NUMPIXELS - 14; i++) {
      color2 = ((i + 15) % 15) * color;
      strip.setPixelColor( i + head, color2);
      strip.show();
    }


    //state 3 end power
  } else if (power && state == 4) {
    //state 4 power
    int bounce;
    heartbeat++;
    if (heartbeat > 7) {
      heartbeat = 0;
    }
    for (int i = 0; i < NUMPIXELS ; i++) {

      if (i % 9 >= heartbeat) {
        color2 = color + (i % 9) * color1;
      } else {
        color2 = 0;
      }

      Serial.println(color2);

      strip.setPixelColor(i, color2);
      strip.show();
    }
    //state 4 end power
  } else if (power && state == 5) {
    //state 5 power

    uint32_t ranB, ranG, ranR;
    ranB = random(0x00, 0xFF);
    ranG = random(0x00, 0xFF);
    ranR = random(0x00, 0xFF);

    color2 = ranB + ranG * 0x010000 + ranR * 0x0100;

    for (int i = 0; i < NUMPIXELS; i++) {
      int ranNum = random(0, brt);
      if (ranNum < 2) {
        strip.setPixelColor(i, color2);
      } else {
        strip.setPixelColor(i, 0);
      }

      strip.show();
    }
    delay(100);
    //state 5 end power
  } else if (power && state == 6) {
    //state 6 power
    head ++;
    if (head > 7) head = 0;

    for (int i = -6; i < NUMPIXELS - 6; i++) {
      if ((i + 7) % 7 == 0) {
        strip.setPixelColor(i + head, color1);
      } else if ((i + 7) % 7 == 1) {
        strip.setPixelColor(i + head, color2);
      } else if ((i + 7) % 7 == 2) {
        strip.setPixelColor(i + head, color3);
      } else if ((i + 7) % 7 == 3) {
        strip.setPixelColor(i + head, color4);
      } else if ((i + 7) % 7 == 4) {
        strip.setPixelColor(i + head, color5);
      } else if ((i + 7) % 7 == 5) {
        strip.setPixelColor(i + head, color6);
      } else if ((i + 7) % 7 == 6) {
        strip.setPixelColor(i + head, color7);
      }
      strip.show();
    }

    //state 6 end power
  } else if (!power) {
    //state off
    for (int i = NUMPIXELS - 1; i >= 0; i--) {
      strip.setPixelColor(i, 0);
      strip.show();
    }
  } else {
    //state between
    for (int i = NUMPIXELS - 1; i >= 0; i--) {
      strip.setPixelColor(i, 0x505050);
      strip.show();
    }
  }
}
