
/*
 TELEPHONE RINGER BY MakeItHackin
 FOR MORE INFORMATION, VISIT: https://github.com/MakeItHackin/TelephoneRinger
 Version: 1.1 with Optional Display
 */



/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#define OLEDSCREEN //false // set true for using a SSD1306 OLED Display
#define ADAFRUIT_IO // false // set true to connect to and control board using Adafruit IO

#if ADAFRUIT_IO == true
  #include "config.h" // config file for wifi credentials
  // set up the 'digital' feed
  AdafruitIO_Feed *RingFeed = io.feed("RingFeed");
  AdafruitIO_Feed *BritishRingFeed = io.feed("BritishRingFeed");
  AdafruitIO_Feed *SingleRingFeed = io.feed("SingleRingFeed");
#endif

#if OLEDSCREEN == true
  #include <SPI.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>

  #define SCREEN_WIDTH 128 // OLED display width, in pixels
  #define SCREEN_HEIGHT 32 // OLED display height, in pixels
  
  // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
  #define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
  
  // ADAFRUIT LOGO
  static const unsigned char PROGMEM logo_bmp[] =
  { B00000000, B11000000,
    B00000001, B11000000,
    B00000001, B11000000,
    B00000011, B11100000,
    B11110011, B11100000,
    B11111110, B11111000,
    B01111110, B11111111,
    B00110011, B10011111,
    B00011111, B11111100,
    B00001101, B01110000,
    B00011011, B10100000,
    B00111111, B11100000,
    B00111111, B11110000,
    B01111100, B11110000,
    B01110000, B01110000,
    B00000000, B00110000 };
#endif

// PIN ASSIGNMENTS FOR A,B,C BUTTONS, MOTOR CONTROLLER, AND LEDS
#if defined(ESP8266)
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
  #define IN1 14
  #define IN2 12
  #define LED1 13
  #define LED2 15
#elif defined(ESP32)
  #define BUTTON_A 15
  #define BUTTON_B 32
  #define BUTTON_C 14
  #define IN1 13
  #define IN2 12
  #define LED1 27
  #define LED2 33
#elif defined(ARDUINO_STM32_FEATHER)
  #define BUTTON_A PA15
  #define BUTTON_B PC7
  #define BUTTON_C PC5
  #define IN1 PB5
  #define IN2 PA13
  #define LED1 PA4
  #define LED2 PB4
#elif defined(TEENSYDUINO)
  #define BUTTON_A  4
  #define BUTTON_B  3
  #define BUTTON_C  8
  #define IN1 5
  #define IN2 6
  #define LED1 9
  #define LED2 10
#elif defined(ARDUINO_FEATHER52832)
  #define BUTTON_A 31
  #define BUTTON_B 30
  #define BUTTON_C 27
  #define IN1 16
  #define IN2 15
  #define LED1 7
  #define LED2 11
#else // 32u4, M0, M4, nrf52840 and 328p
  #define BUTTON_A  9
  #define BUTTON_B  6
  #define BUTTON_C  5
  #define IN1 13
  #define IN2 12
  #define LED1 11
  #define LED2 10
#endif


// LOOP COUNT FOR AMERICAN RINGER
// HIGHER VALUE INCREASES RING DURATION
int americanLoopCount = 30; 

// LOOP DELAY FOR AMERICAN RINGER
// A VALUE OF 25 WILL PRODUCE 20 HERTZ.  YOU PROBABLY DON'T NEED TO CHANGE THIS VALUE. SOME PHONES WILL NOT RING WITH VALUES BEYOND A COUPLE HERTZ.
int americanLoopDelay = 26;  

// LOOP COUNT FOR BRITISH RINGER
// HIGHER VALUE INCREASES RING DURATION
int britishLoopCount = 10;

// LOOP DELAY FOR BRITISH RINGER
// THE DEFAULT VALUE OF 19 IS PRETTY LOW AND MAY NOT WORK ON ALL PHONES 
int britishLoopCountDelay = 19;

// DELAY BETWEEN RINGS FOR BRITISH RINGER
// FEEL FREE TO EXPERIMENT WITH THIS VALUE
int britishDelayBetweenRings = 200;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // SETTING UP PIN DIRECTION
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  
  #if OLEDSCREEN == true
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
    display.display(); // ADAFRUIT LOGO
    // Clear the buffer
    display.clearDisplay();
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(WHITE);        // Draw white text
  #endif

  #if ADAFRUIT_IO == true
    // wait for serial monitor to open
    while(! Serial);
    // connect to io.adafruit.com
    Serial.print("Connecting to Adafruit IO");
    #if OLEDSCREEN == true
      display.setCursor(0,0);             // Start at top-left corner
      display.println(F("CONNECTING TO"));
      display.println(F("ADAFRUIT IO..."));
      display.display();
    #endif
    io.connect();
    // set up a message handler for the 'digital' feed.
    // the handleMessage function (defined below)
    // will be called whenever a message is
    // received from adafruit io.
    RingFeed->onMessage(handleMessage);
    BritishRingFeed->onMessage(handleBritish);
    SingleRingFeed->onMessage(handleSingle); 
    // wait for a connection
    while(io.status() < AIO_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
      // we are connected
    Serial.println();
    Serial.println(io.statusText());
    RingFeed->get();
    BritishRingFeed->get();
    SingleRingFeed->get();
  #endif
  
  Serial.println("Starting...");
}

void loop() {
  #if ADAFRUIT_IO == true
    io.run();
  #endif
  
  #if OLEDSCREEN == true
    displayRingMenu(); // DISPLAY RING MENU
  #endif
  // ASSIGN BUTTON INPUTS TO BOOLEAN VALUES
  bool buttonAState = digitalRead(BUTTON_A);
  bool buttonBState = digitalRead(BUTTON_B);
  bool buttonCState = digitalRead(BUTTON_C);

  if (buttonAState == LOW){  // BUTTON A PRESSED
    displayButtonPress('A');
    ringTelephone(2,1000,30,26,false,1);
  }
  else if (buttonBState == LOW){ // BUTTON B PRESSED
    displayButtonPress('b');  
    ringTelephone(3,1000,10,19,true,2);   
  }
  else if (buttonCState == LOW){ // BUTTON C PRESSED
    displayButtonPress('c');  
    ringTelephone(1,0,4,26,false,2);    
  }
  else{
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
  }
}

#if OLEDSCREEN == true
  // THIS MENU WILL BE DISPLAYED WHEN USER IS NOT PRESSING BUTTONS
  void displayRingMenu(){
    display.clearDisplay();
    display.setCursor(0,0);             // Start at top-left corner
    display.println(F("SELECT A RING:"));
    display.println(F("BUTTON A - AMERICAN"));
    display.println(F("BUTTON B - BRITISH"));
    display.println(F("BUTTON C - SINGLE"));
    display.display();
  }
#endif

// WHEN A USER PRESSES BUTTONS THESE SCREENS WILL APPEAR
void displayButtonPress(char button){
  if (button == 'a' || button == 'A'){
    Serial.println("BUTTON A PRESSED!");
    #if OLEDSCREEN == true
      display.clearDisplay();
      display.setCursor(0,0);             // Start at top-left corner
      display.println(F("BUTTON A PRESSED"));
      display.println(F("AMERICAN RING!"));
      display.display();
    #endif
  }
  else if (button == 'b' || button == 'B'){
    Serial.println("BUTTON B PRESSED!");
    #if OLEDSCREEN == true
      display.clearDisplay();
      display.setCursor(0,0);             // Start at top-left corner
      display.println(F("BUTTON B PRESSED"));
      display.println(F("BRITISH RING!"));
      display.display();
    #endif
  }
  else if (button == 'c' || button == 'C'){
    Serial.println("BUTTON C PRESSED!");
    #if OLEDSCREEN == true
      display.clearDisplay();
      display.setCursor(0,0);             // Start at top-left corner
      display.println(F("BUTTON C PRESSED"));
      display.println(F("SINGLE RING!"));
      display.display();
    #endif
  }
}

// THIS FUNCTION IS USED TO RING THE TELEPHONE USING GIVEN PARAMETERS
// ringCount: This is how many times you want the phone to ring
// ringDelay: This is the amount of milliseconds you want between each ring
// loopCount: This is how long the phone should ring during one ring.
// loopDelay: This corresponds to the frequency of the ring.  You'll need to stay around 25 +- 6
// britishBoolean: true for "British" style ring or false for "American" style
// ledAssignment: 1 for LED1 or 2 for LED2

void ringTelephone (int ringCount, int ringDelay, int loopCount, int loopDelay, bool britishBoolean, byte ledAssignment){
  int interation = 1;
  Serial.print("ringCount: ");
  Serial.print(ringCount);
  Serial.print(" - ringDelay: ");
  Serial.print(ringDelay);
  Serial.print(" - loopCount: ");
  Serial.print(loopCount);
  Serial.print(" - loopDelay: ");
  Serial.print(loopDelay);
  Serial.print(" - britishBoolean: ");
  Serial.print(britishBoolean);
  Serial.print(" - ledAssignment: ");
  Serial.println(ledAssignment);
  
  for(int i = 0; i < ringCount; i++){
    if (ledAssignment == 1){
      digitalWrite(LED1, HIGH);
    }
    else {
      digitalWrite(LED2, HIGH);
    }
    
    if (britishBoolean == true){
      interation = 2; // "British" rings are two quick rings, so setting up a loop to ring twice
      //Serial.print("interation: ");
     //Serial.println(interation);
    }
    for (int j = 0; j < interation; j++){ // this for loop is for the american or british ring styles
      for(int x = 0; x < loopCount; x++){ // this for loop is for the duration of the ring.
        //Serial.println("loop");
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        delay(loopDelay);
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        delay(loopDelay);
      }
      //Serial.print("britishDelayBetweenRings: ");
      //Serial.println(britishDelayBetweenRings);
      if (britishBoolean == true){ // for the british ring, delay a little bit before the next quick ring
        delay(britishDelayBetweenRings);
      }
    }
    if (ledAssignment == 1){ // turn off the led at the end of each ring
      digitalWrite(LED1, LOW);
    }
    else {
      digitalWrite(LED2, LOW);
    }
    delay(ringDelay);
  }
  delay(100);
}


#if ADAFRUIT_IO == true
  // this function is called whenever an 'digital' feed message
  // is received from Adafruit IO. it was attached to
  // the 'digital' feed in the setup() function above.
  void handleMessage(AdafruitIO_Data *American_Ring) {
  
    Serial.print("received <- ");
  
    if(American_Ring->toPinLevel() == HIGH){
      Serial.println("AMERICAN HIGH");
    }
    else {
      Serial.println("AMERICAN LOW");
      display.clearDisplay();
      display.setCursor(0,0);             // Start at top-left corner
      display.println(F("IO BUTTON A PRESSED"));
      ringTelephone(1,1000,30,26,false,1);
    }
 // digitalWrite(LED_PIN, American_Ring->toPinLevel());
}
#endif

#if ADAFRUIT_IO == true
void handleBritish(AdafruitIO_Data *data) {

  Serial.print("received <- ");

  if(data->toPinLevel() == HIGH){
    Serial.println("BRITISH HIGH");
  }
  else {
    Serial.println("BRITISH LOW");
    display.clearDisplay();
    display.setCursor(0,0);             // Start at top-left corner
    display.println(F("IO BUTTON B PRESSED"));
    ringTelephone(1,1000,10,19,true,2); 
  }
//  digitalWrite(LED_PIN, data->toPinLevel());
}
#endif

#if ADAFRUIT_IO == true
void handleSingle(AdafruitIO_Data *data) {

  Serial.print("received <- ");

  if(data->toPinLevel() == HIGH){
    Serial.println("SINGLE HIGH");
  }
  else {
    Serial.println("SINGLE LOW");
    display.clearDisplay();
    display.setCursor(0,0);             // Start at top-left corner
    display.println(F("IO BUTTON C PRESSED"));
    ringTelephone(1,0,4,26,false,2);
  }
//  digitalWrite(LED_PIN, data->toPinLevel());
}
#endif
