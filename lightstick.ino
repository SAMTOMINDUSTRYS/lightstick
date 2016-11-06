#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <SD.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

// D6 == GPIO 12
// D2 == GPIO 4
#define LIGHT_PIN 4

// Number of NeoPixels
#define LEDN 144
#define CARD_PIN 15

int num_cols;
int *colour;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDN, LIGHT_PIN, NEO_GRB + NEO_KHZ800);
File colourFile;

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show();

  Serial.print("Initializing SD card...");
  if (!SD.begin(CARD_PIN)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  colourFile = SD.open("col.txt");

  char lines[1];
  lines[0] = colourFile.read();
  num_cols = atoi(lines);
  Serial.println(num_cols);
  colour = (int*) new int [num_cols*3];
  colourFile.read();
  colourFile.read();
  
  char buff[4];
  for(int i=0; i < num_cols; i++){
    for(int j=0; j < 3; j++){
      for(int k=0; k < 3; k++){
        buff[k] = colourFile.read();
      }
      colour[(i*3)+j] = atoi(buff);
    }
    colourFile.read();
    colourFile.read();
  }
  for(int i=0; i < num_cols; i++){
    Serial.print(colour[(i*3)]);
        Serial.print(",");
    Serial.print(colour[(i*3)+1]);
        Serial.print(",");
    Serial.print(colour[(i*3)+2]);
    Serial.print("\n");
  }
}

void loop() {
  for(int i=0; i < num_cols; i++){
    colorWipe(strip.Color(colour[(i*3)], colour[(i*3)+1], colour[(i*3)+2]), 50);
  }

  // Some example procedures showing how to display to the pixels:
  //colorWipe(strip.Color(255, 0, 0), 50); // Red
  //colorWipe(strip.Color(0, 255, 0), 50); // Green
  //colorWipe(strip.Color(0, 0, 255), 50); // Blue

  // Send a theater pixel chase in...
  //theaterChase(strip.Color(127, 127, 127), 50); // White
  //theaterChase(strip.Color(127, 0, 0), 50); // Red
  //theaterChase(strip.Color(0, 0, 127), 50); // Blue

  //rainbow(20);
  //rainbowCycle(20);
  //theaterChaseRainbow(50);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
