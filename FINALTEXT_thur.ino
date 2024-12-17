#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // for 32 height screen

#include <Fonts\Org_01.h>

// setting up LEDS....
int pins[2] = {2, 4};
int ledCount = 0;

// setting up screen & text lines...................
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int wordIndex = 0; // cycle through lines.
String vampLines[18] = {
  "WE\'RE \nEVERYWHERE", "void transform() {\n current_form = BAT;\n squeak(); \n}", "GOD HATES FANGS", "let\'s get the fuck \nback to church",
  "liberte,\negalite,\nhomosexualite,\nou la mort", "in evil i long took delight....", "APPETITE WITHOUT\nDISTINCTION",
  "if (uvIndex >= 2) {\n Vampire.hissAndScream();\n Vampire.burnAlive(); \n}", "IMMATERIAL GIRLS", "war er ein Tier,\nwenn Musik ihn so\nbewegte?",
  "clench my hands, \ndraw your claws", "KREATUR\nTEUFEL", "FORMLESS\nGREED",
  "class Vampire {\n  String origin, pronouns;\n  int age, numVictims;\n  Hunger level;\n}",
  "Vampire socks = Vampire(\n\"Dunedin\", \"they/them\", 23, 2, Hunger.RAVENOUS);",
  "VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!VAMP!",
  "void feed(Human victim) {\n  while (drained == false) { drink_blood(); }\n numVictims += 1;\n}", "fecemi la divina podestate, primo amore"
};
// customizing font size and cursor coords for each line - different printing requirements. doing manually LOL fml
int lineFontSizes[18] = { 2, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 1, 1, 1, 1, 1}; 
int wheresCursor[18][2] = { //  in {} - firstnum=x, next=y
  {0, 10}, {0, 6}, {0, 10}, {0, 6}, {0, 6}, {0, 10},
  {0, 6}, {0, 6}, {0, 10}, {0, 6}, {0, 6}, {0, 10}, 
  {0, 10}, {0, 6}, {0, 6}, {0, 6}, {0, 6}, {0, 6}
}; // ALSO, note all x=0: though useless now i wanna add customization optionin case.

void setup() {
  // setup pins
  for (int i = 0; i <sizeof(pins); i++) {
      pinMode(pins[i], OUTPUT);
  }

  // setup screen
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.setFont(&Org_01);
  display.setTextColor(SSD1306_WHITE);
 
  display.clearDisplay();
}

void loop() {
  // display text
  display.setCursor(wheresCursor[wordIndex][0], wheresCursor[wordIndex][1]);
  display.setTextSize(lineFontSizes[wordIndex]);
  display.println(vampLines[wordIndex]);
  display.display();      // Show initial text

  // reset change etc.
  wordIndex = (wordIndex + 1) % ((sizeof(vampLines)) / sizeof(vampLines[0]));
  delay( (int)random(7500, 1250) );
  display.clearDisplay();

  ledCount++;
  // change LED state on->off->on, etc.
  if (ledCount % 4 == 1) { 
    for (int i = 0; i < sizeof(pins); i++) {
      digitalWrite(pins[i], LOW);
    }
  } else {
    for (int i = 0; i <sizeof(pins); i++) {
      digitalWrite(pins[i], HIGH);
    }
  }
}
