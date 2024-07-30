#include <SdFat.h>                 // SD card & FAT filesystem library
#include <Button.h>                // Library for easily reading buttons
#include <Adafruit_GFX.h>          // Core graphics library
#include <Adafruit_ILI9341.h>      // Hardware-specific library
#include <Adafruit_ImageReader.h>  // Image-reading functions

#define SD_CS 3                                              // SD card select pin
#define TFT_CS 6                                             // TFT select pin
#define TFT_DC 7                                             // TFT display/command pin
#define SPI_CLOCK SD_SCK_MHZ(12)                             // max SCK frequency in Hz
#define SD_CONFIG SdSpiConfig(SD_CS, SHARED_SPI, SPI_CLOCK)  // SPI SD card configuration.

#define FILENAME_LENGTH 13                    // 8.3 filename with null terminator
#define BMPS_PATH_LENGTH 6 + FILENAME_LENGTH  // dir + filename


SdFat sd;                                                 // SD file system for FAT volumes
File root;                                                // file/dir on SD card
Adafruit_ImageReader reader(sd);                          // Image-reader object, pass in SD filesys
ImageReturnCode imgReadStat;                              // Status from image-reading functions
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);  // TFT display

Button tap(digitalPinToInterrupt(0));  // button to detect taps on the screen

const char *bmpsDir = "/bmps/";           // directory of bitmap files
char filename[FILENAME_LENGTH] = { 0 };   // name of a file on the SD card
char fullPath[BMPS_PATH_LENGTH] = { 0 };  // dir and bmp filename
uint8_t bmpFileCount = 0;                 // number of bmp files

const unsigned long sleepMS = 3600000;  // sleep ms (minutes * seconds * millis)
unsigned long currentMillis = 0;
unsigned long previousMillis = sleepMS;




void setup() {
  currentMillis = millis();

  Serial.begin(115200);
  Serial.println("Serial started");

  if (sd.begin(SD_CONFIG)) {
    Serial.println("SD card started");
  } else {
    Serial.println("SD card failed");
    while (1) {}
  }

  root.open(bmpsDir);
  bmpFileCount = getFileCount(root);
  root.close();
  Serial.print("Number of bitmaps found: ");
  Serial.println(bmpFileCount);

  tft.begin();
  Serial.println("Display started");
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(1);
  // display a splash screen and wait a couple seconds
  reader.drawBMP("/boot/boot.bmp", tft, 0, 0);
  delay(3000);

  // start the button/tap
  tap.begin();

  // seed the RNG with a read from an analog pin
  pinMode(A1, INPUT_PULLUP);
  pinMode(A1, INPUT);
  randomSeed(analogRead(A1));
}

void loop() {
  // put your main code here, to run repeatedly:
}

uint8_t getFileCount(File dir) {
  File currentFile;
  uint8_t fileCount = 0;

  dir.rewindDirectory();
  // iterate through the directory and count
  // everything that's not a directory
  while (currentFile.openNext(&dir, O_RDONLY)) {
    if (currentFile.isFile()) {
      // currentFile.printName(&Serial);
      // Serial.println();
      fileCount++;
    }
    currentFile.close();
  }
  return fileCount;
}