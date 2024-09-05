/* 
* PIA code - stolen - for vampire
  Cycle through images on arduino nano
*/

// libraries:
#include <Adafruit_GFX.h>     // graphics library
#include <Adafruit_ST7735.h>  // library for driver st7735 for BIG
#include <SPI.h>              // SPI library: comms for BIG
#include <SD.h>               // SD library: using images stored on SD for BIG
#include <Wire.h>             // for LITTLE screen - comms
#include <Adafruit_SSD1306.h>


// defining BIG screen connector pins
#define TFT_RST 5  // reset line (optional, pass -1 if not used)
#define TFT_CS 6   // chip select line
#define TFT_DC 7   // data/command line

// stuff for BIG images SD
#define SD_CS 4
#define BUFFPIXEL 20


// LITTLE screen stuff
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // for 32 height screen

#include <Fonts\Picopixel.h>

int size = 2; // font size
int maxCharLine = SCREEN_WIDTH / size; 
int index = 0; // cycle through lines.
String vampLines[10] = {
 "blood!", "blood!", "blood!", "blood!", "blood!" "blood!", "blood!",
"blood!", "blood!", "blood!"
};


// LED stuff
int pins[2] = {2, 3};
int ledCount = 0;
int ledInvertValue = 3;


// init. screens:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, 7, 5);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);



void setup() {
  Serial.begin(9600);

  // init LEDS
  for (int i = 0; i < sizeof(pins); i++) {
    pinMode(pins[i], OUTPUT);
  };

  // init LITTLE SCREEN
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  // below is font and format etc.
  display.setTextSize(size);
  display.setTextColor(SSD1306_WHITE);
  display.setFont(&Picopixel);


  // init BIG SCREEN
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);  // enable display output!!!!
  tft.initR(INITR_BLACKTAB);

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("fail..........");
    while (1)
      ;  // freeze code infinite loop
  }
  Serial.println("good!");

  File root = SD.open("/");  // open SD card main root
  printDirectory(root, 0);   // print all files names and sizes
  root.close();
}



void loop() {
  File root = SD.open("/");  // open SD card main root

  while (true) { // idk why this is here man
    File entry = root.openNextFile(); 
    index = (index + 1) % ((sizeof(vampLines)) / sizeof(vampLines[0]));

    // if no next file, close and reset
    if (!entry) { // 
      root.close();
      return;
    }

    getNewImg(entry);
    entry.close();
    int randomDelay = (int)random(3000, 5000); // SET delay

    
    if (ledCount % ledInvertValue == 0) {
      for (int i = 0; i < sizeof(pins); i++) {
        digitalWrite(pins[i], HIGH);
        };
    } else if (ledCount % ledInvertValue == 1) {
      for (int i = 0; i < sizeof(pins); i++) {
        digitalWrite(pins[i], LOW);
        };
    };
   
    // wait for some time to switch states
    ledCount++; // also increment LED count
    Serial.println(randomDelay); 
    delay(randomDelay);
  }
}


void displayText() {
  display.clearDisplay();

  String firstLine = vampLines[index].substring(0, maxCharLine); // this is to format multi-line things
  String secondLine = vampLines[index].substring(maxCharLine, SCREEN_WIDTH);

  display.setCursor(2,10);
  display.println(firstLine);
  display.setCursor(2, 17);
  display.println(secondLine);

  display.display();
}










// everything below is not my code !
// credit: https://simple-circuit.com/draw-bmp-images-arduino-sd-card-st7735/
// function to de-clutter main loop
void getNewImg(File entry) {
  uint8_t nameSize = String(entry.name()).length();            // get file name size
  String str1 = String(entry.name()).substring(nameSize - 4);  // save the last 4 characters (file extension)

  if (str1.equalsIgnoreCase(".bmp"))  // if the file has '.bmp' extension
    bmpDraw(entry.name(), 0, 0);      // draw it  // close the file
}

void bmpDraw(char *filename, uint8_t x, uint16_t y) {

  File bmpFile;
  int bmpWidth, bmpHeight;             // W+H in pixels
  uint8_t bmpDepth;                    // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;             // Start of image data in file
  uint32_t rowSize;                    // Not always = bmpWidth; may have padding
  uint8_t sdbuffer[3 * BUFFPIXEL];     // pixel buffer (R+G+B per pixel)
  uint8_t buffidx = sizeof(sdbuffer);  // Current position in sdbuffer
  boolean goodBmp = false;             // Set to true on valid header parse
  boolean flip = true;                 // BMP is stored bottom-to-top
  int w, h, row, col;
  uint8_t r, g, b;
  uint32_t pos = 0, startTime = millis();

  if ((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print(F("File not found"));
    return;
  }

  // Parse BMP header
  if (read16(bmpFile) == 0x4D42) {  // BMP signature
    Serial.print(F("File size: "));
    Serial.println(read32(bmpFile));
    (void)read32(bmpFile);             // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile);  // Start of image data
    Serial.print(F("Image Offset: "));
    Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: "));
    Serial.println(read32(bmpFile));
    bmpWidth = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if (read16(bmpFile) == 1) {    // # planes -- must be '1'
      bmpDepth = read16(bmpFile);  // bits per pixel
      Serial.print(F("Bit Depth: "));
      Serial.println(bmpDepth);
      if ((bmpDepth == 24) && (read32(bmpFile) == 0)) {  // 0 = uncompressed

        goodBmp = true;  // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if ((x + w - 1) >= tft.width()) w = tft.width() - x;
        if ((y + h - 1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.startWrite();
        tft.setAddrWindow(x, y, w, h);

        for (row = 0; row < h; row++) {  // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if (flip)  // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else  // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if (bmpFile.position() != pos) {  // Need seek?
            tft.endWrite();
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer);  // Force buffer reload
          }

          for (col = 0; col < w; col++) {  // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) {  // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0;  // Set index to beginning
              tft.startWrite();
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.color565(r, g, b));
          }  // end pixel
        }    // end scanline
        tft.endWrite();
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      }  // end goodBmp
    }
  }

  bmpFile.close();
  if (!goodBmp) Serial.println(F("BMP format not recognized."));
}


// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read();  // LSB
  ((uint8_t *)&result)[1] = f.read();  // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read();  // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read();  // MSB
  return result;
}


void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry = dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}


// end of code.