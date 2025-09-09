#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <Servo.h>
#include <qrcode.h>

//Set pin For Display 
  #define TFT_CS        9 
  #define TFT_RST        8 // Or set to -1 and connect to Arduino RESET pin
  #define TFT_DC         10

// For 1.44" and 1.8" TFT with ST7735 use:
// Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
// Adafruit_ST7735 tft = Adafruit_ST7735(A5,A3,A2,A1,A4);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC,11, 13, TFT_RST);   //This one is slower but workable

//Create object for servo motor
Servo myservo;

//For sonar sensor pin initialized
const int trigPin = 12;
const int echoPin = 6;



//For qr Code
#define OLED_WIDTH 128
#define OLED_HEIGHT 160
  QRCode qrcode;


void setup() {

  // Serial.begin(9600);

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;
  delay(500);

  // Machine Name and Developer Information
  printRVM();
  delay(4000);
  printDevInformation();
  delay(4000);

  // a single pixel
  tft.drawPixel(tft.width()/2, tft.height()/2, ST77XX_GREEN);
  delay(500);

  // line draw test
  testlines(ST77XX_YELLOW);
  delay(500);

  // optimized lines
  testfastlines(ST77XX_RED, ST77XX_BLUE);
  delay(500);

  testdrawrects(ST77XX_GREEN);
  delay(500);

  // testfillrects(ST77XX_YELLOW, ST77XX_MAGENTA);
  // delay(500);

  tft.fillScreen(ST77XX_BLACK);
  testfillcircles(10, ST77XX_BLUE);
  testdrawcircles(10, ST77XX_WHITE);
  delay(500);
                                         
  testroundrects();
  delay(500);
  // int IR_SensorInput = 8;
  // int IR_SensorOutput = 9;
  // Serial.begin(9600);

  //ALL PIN SETUP
  //Pin Setup for IR Sensor
  pinMode(7, INPUT);

  //Pin Setup for sonar sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //Pin Setup for servo motor
  myservo.attach(5);  

  //Pin Setup for Plastic, Metal and Complete Button
  pinMode(1, INPUT); //For Plastic Button
  pinMode(2, INPUT); //For Metal Button
  pinMode(3, INPUT); //For Complete Button
  
  myservo.write(45);
  delay(500);
}



void loop() { 

  int PButtonPressed = 0;
  int MButtonPressed = 0;
  int totalPlastic = 0;
  int totalMetal = 0;

  //For sonar sensor
  // establish variables for duration of the ping, and the distance result
  long duration, inches;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  // convert the time into a distance
  inches = duration / 74 / 2;

  //All intro 
  printRVM();
  delay(1000);
  printTeamName();
  delay(1000);
  printDevInformation();
  delay(1000);

  //IF container is full show message
  if(inches <= 2){   
        //Display output code for each plastic
        tft.fillScreen(ST77XX_WHITE);
        tft.setCursor(2, 70);
        tft.setTextColor(ST77XX_RED);
          tft.println("--------------------");
          tft.println("| CONTAINER IS FULL |");
          tft.println("---------------------");
          delay(6000);
  } 


  else{
  displayInstruction1();
  delay(4000);
  PButtonPressed = digitalRead(1); //Read input from plastic button
  delay(2000);
  MButtonPressed = digitalRead(2); //Read input from metal button
  delay(2000);

  if(PButtonPressed){   //If button pressed for Plastic
    PButtonPressed = 0;
    MButtonPressed = 0;
    myservo.write(0); 
    delay(1000);
    displayInstruction2();
    delay(1500);

    while(true){
        int status = digitalRead(7);   //this is the ir sensor pin
        if(status == LOW){  //If IR Sensor detect object status is low
          totalPlastic += 1;

          //Display output code for each plastic
          tft.fillScreen(ST77XX_WHITE);
          tft.setCursor(25, 20);
          tft.setTextColor(ST77XX_BLACK);
          tft.println("You submitted ");
          tft.setCursor(15, 30);   
          tft.print(totalPlastic);
          tft.println(" plastic bottle");
          tft.setCursor(10, 45);  
          tft.print("|________________|");
          delay(1000);

          //Display output Code to show message for submitting more plastic or complete
          tft.setCursor(15, 80);
          tft.setTextColor(ST77XX_BLACK);
          tft.println("Give more plastic");
          tft.setCursor(60, 95);
          tft.println("OR");
          tft.setCursor(5, 105); 
          tft.println("____________________");          
          tft.println("| To finish         |");
          tft.setCursor(0, 130);
          tft.println("| Press BLUE button |");
          tft.println("|___________________|");
          delay(1000);

        }

        int completeButtonPressed = digitalRead(3);  //Here i've to write complete Button code;
        
        if(completeButtonPressed == HIGH){
          completeButtonPressed = 0;
          myservo.write(45);
          delay(1000);

          //Display output code for total plastic
          tft.fillScreen(ST77XX_WHITE);
          tft.setCursor(15, 20);
          tft.setTextColor(ST77XX_BLACK);
          tft.println("You submitted ");   
          tft.print(totalPlastic);
          tft.print(" plastic waste");
          delay(1000);

          //Display output Code to show message to thanks
          tft.setCursor(5, 60);
          tft.setTextColor(ST77XX_BLACK);
          tft.println("Take photo");
          tft.setCursor(5, 75);
          tft.println("Of the QR Code");
          delay(3000);


          //Display QR Code
          tft.fillScreen(ST7735_WHITE);
          delay(300);

          const uint8_t ecc = 0;  //lowest level of error correction
          const uint8_t version = 3; 
          uint8_t qrcodeData[qrcode_getBufferSize(version)];
          const int xy_scale = 3;
          const int x_offset = (tft.width() - xy_scale*qrcode.size)/2;
          const int y_offset = (tft.height() - xy_scale*qrcode.size)/2;
          char data[100] ;
          String a = "Machine Code: 03997: Your submitted plastic ";
          a.concat(totalPlastic);
          a.toCharArray(data, 100);


          qrcode_initText(&qrcode, 
                    qrcodeData, 
                    version, ecc, 
                    data);


    
          for (uint8_t y = 0; y < qrcode.size; y++) {
           // Each horizontal module
           for (uint8_t x = 0; x < qrcode.size; x++) {

            // Print each module (UTF-8 \u2588 is a solid block)
            bool mod = qrcode_getModule(&qrcode, x, y);
            if(mod){
              int px = x_offset + (x * xy_scale);
              int py = y_offset + (y * xy_scale);
              tft.fillRect(px, py, xy_scale, xy_scale, ST7735_BLACK);
              
              }
            }
          }

          delay(10000);
          //Display output Code to show message to thanks
          tft.fillScreen(ST77XX_WHITE);
          tft.setCursor(5, 40);
          tft.setTextColor(ST77XX_BLACK);
          tft.println("THANKS");
          tft.setCursor(5, 55);
          tft.println("FOR SAVING EARTH");
          delay(3000);
          break;
        }
    }
  }

  else if(MButtonPressed){   //If button pressed for Metal
    MButtonPressed = 0;
    PButtonPressed = 0;
    myservo.write(100); 
    delay(1000);
    displayInstruction3();
    delay(1500);

    while(true){
        int status = digitalRead(7);   //this is the ir sensor pin
        if(status == LOW){  //If IR Sensor detect object status is low
          totalMetal += 1;

          //Display output code for each plastic
          tft.fillScreen(ST77XX_WHITE);
          tft.setCursor(25, 20);
          tft.setTextColor(ST77XX_BLACK);
          tft.println("You submitted ");
          tft.setCursor(15, 30);   
          tft.print(totalMetal);
          tft.println(" metal can");
          tft.setCursor(10, 45);  
          tft.print("|________________|");
          delay(1000);

          //Display output Code to show message for submitting more metal or complete
          tft.setCursor(15, 80);
          tft.setTextColor(ST77XX_BLACK);
          tft.println("Give more metal");
          tft.setCursor(60, 95);
          tft.println("OR");
          tft.setCursor(5, 105); 
          tft.println("____________________");          
          tft.println("| To finish         |");
          tft.setCursor(0, 130);
          tft.println("| Press BLUE button |");
          tft.println("|___________________|");
          delay(1000);

        }

        int completeButtonPressed = digitalRead(3);  //Here i've to write complete Button code;
        
        if(completeButtonPressed == HIGH){
          completeButtonPressed = 0;
          myservo.write(45);
          delay(1000);

          //Display output code for total metal
          tft.fillScreen(ST77XX_WHITE);
          tft.setCursor(15, 20);
          tft.setTextColor(ST77XX_BLACK);
          tft.println("You submitted ");   
          tft.print(totalMetal);
          tft.print(" metal waste");
          delay(1000);

          //Display output Code to show message to thanks
          tft.setCursor(5, 60);
          tft.setTextColor(ST77XX_BLACK);
          tft.println("Take photo");
          tft.setCursor(5, 75);
          tft.println("Of the QR Code");
          delay(3000);


           //Display QR Code
          tft.fillScreen(ST7735_WHITE);
          delay(300);

          const uint8_t ecc = 0;  //lowest level of error correction
          const uint8_t version = 3; 
          uint8_t qrcodeData[qrcode_getBufferSize(version)];
          const int xy_scale = 3;
          const int x_offset = (tft.width() - xy_scale*qrcode.size)/2;
          const int y_offset = (tft.height() - xy_scale*qrcode.size)/2;
          char data[100] ;
          String a = "Machine Code: 03997: Your submitted plastic ";
          a.concat(totalMetal);
          a.toCharArray(data, 100);


          qrcode_initText(&qrcode, 
                    qrcodeData, 
                    version, ecc, 
                    data);


    
          for (uint8_t y = 0; y < qrcode.size; y++) {
           // Each horizontal module
           for (uint8_t x = 0; x < qrcode.size; x++) {

            // Print each module (UTF-8 \u2588 is a solid block)
            bool mod = qrcode_getModule(&qrcode, x, y);
            if(mod){
              int px = x_offset + (x * xy_scale);
              int py = y_offset + (y * xy_scale);
              tft.fillRect(px, py, xy_scale, xy_scale, ST7735_BLACK);
              
              }
            }
          }

          delay(10000);
          //Display output Code to show message to thanks
          tft.fillScreen(ST77XX_WHITE);
          tft.setCursor(5, 40);
          tft.setTextColor(ST77XX_BLACK);
          tft.println("THANKS");
          tft.setCursor(5, 55);
          tft.println("FOR SAVING EARTH");
          delay(3000);
          break;
        }
    }
  }

  }
}







//My created function

void testlines(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
    delay(0);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void printDevInformation(){
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(25, 20);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.println("DEVELOPER TEAM");
  tft.println();
  tft.setCursor(15, 40);
  tft.println("Faysal Al Mahmud");
    tft.setCursor(15, 55);
  tft.println("Khalid Saifullah");
    tft.setCursor(15, 70);
  tft.println("Yeasin Us Main");
    tft.setCursor(15, 85);
  tft.println("Kazi Masruzzaman");
}


//start screen animation
void printRVM(){
  tft.fillScreen(ST77XX_WHITE);
  tft.setCursor(15, 50);
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(2);
  tft.println("SFMU");
  tft.setCursor(15, 70);
  tft.println("Reverse");
  tft.setCursor(15, 90);
  tft.println("Vending");
  tft.setCursor(15, 110);
  tft.println("Machine");
}


//show team name function
void printTeamName(){
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(30, 60);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.println("DEVELOPED BY");
  tft.println();
  tft.setCursor(25, 75);
  tft.println("TEAM MUSKETEER");
}



//instruction display on screen
void displayInstruction1(){
  tft.fillScreen(ST77XX_WHITE);
  tft.setCursor(25, 20);
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(1);
  tft.println("--INSTRUCTION--");
  tft.println();
  tft.setCursor(5, 50);
  tft.println("1. Press-Hold 2sec 'GREEN' button for Plastic");
    tft.setCursor(5, 80);
  tft.println("2. Press-Hold 2sec 'RED' button for Metal");
    tft.setCursor(5, 100);
  tft.println("3.To finish Press 'BLUE' button");
}


//second type instruction showing function
void displayInstruction2(){
  tft.fillScreen(ST77XX_WHITE);
  tft.setCursor(25, 20);
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(1);
  tft.println("--INSTRUCTION--");
  tft.println();
  tft.setCursor(5, 50);
  tft.println("Give plastic Bottles");
}

void displayInstruction3(){
  tft.fillScreen(ST77XX_WHITE);
  tft.setCursor(25, 20);
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(1);
  tft.println("--INSTRUCTION--");
  tft.println();
  tft.setCursor(5, 50);
  tft.println("Give metal can");
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=radius; x < tft.width(); x+=radius*2) {
    for (int16_t y=radius; y < tft.height(); y+=radius*2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  tft.fillScreen(ST77XX_BLACK);
  uint16_t color = 0xF800;
  int t;
  int w = tft.width()/2;
  int x = tft.height()-1;
  int y = 0;
  int z = tft.width();
  for(t = 0 ; t <= 15; t++) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testroundrects() {
  tft.fillScreen(ST77XX_BLACK);
  uint16_t color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = tft.width()-2;
    int h = tft.height()-2;
    for(i = 0 ; i <= 16; i+=1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}

void tftPrintTest() {
  tft.setTextWrap(true);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_GREEN);
  // tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST77XX_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(" seconds.");
}

void mediabuttons() {
  // play
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, ST77XX_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_RED);
  delay(500);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, ST77XX_WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_GREEN);
  delay(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_BLUE);
  delay(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_GREEN);
}


