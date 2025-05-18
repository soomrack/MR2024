#include <VGAX.h>
#include <math.h>
#include <VGAXUtils.h>

#define FNT_NANOFONT_HEIGHT 6
#define FNT_NANOFONT_SYMBOLS_COUNT 95

#define BUTTON_1 12  
#define BUTTON_2 11  
#define BUTTON_3 10 
#define BUTTON_4 13 

VGAX vga;
VGAXUtils vgaU;

const unsigned char fnt_nanofont_data[FNT_NANOFONT_SYMBOLS_COUNT][1+FNT_NANOFONT_HEIGHT] PROGMEM={
{ 1, 128, 128, 128, 0, 128, 0, }, //glyph '!' code=0
{ 3, 160, 160, 0, 0, 0, 0, }, //glyph '"' code=1
{ 5, 248, 248, 248, 248, 248, 0, },  //glyph '#' code=2 - full rectangle
{ 5, 120, 160, 112, 40, 240, 0, },  //glyph '$' code=3
{ 5, 136, 16, 32, 64, 136, 0, },  //glyph '%' code=4
{ 5, 96, 144, 104, 144, 104, 0, },  //glyph '&' code=5
{ 2, 128, 64, 0, 0, 0, 0, },  //glyph ''' code=6
{ 2, 64, 128, 128, 128, 64, 0, }, //glyph '(' code=7
{ 2, 128, 64, 64, 64, 128, 0, },  //glyph ')' code=8
{ 3, 0, 160, 64, 160, 0, 0, },  //glyph '*' code=9
{ 3, 0, 64, 224, 64, 0, 0, }, //glyph '+' code=10
{ 2, 0, 0, 0, 0, 128, 64, },  //glyph ',' code=11
{ 3, 0, 0, 224, 0, 0, 0, }, //glyph '-' code=12
{ 1, 0, 0, 0, 0, 128, 0, }, //glyph '.' code=13
{ 5, 8, 16, 32, 64, 128, 0, },  //glyph '/' code=14
{ 4, 96, 144, 144, 144, 96, 0, }, //glyph '0' code=15
{ 3, 64, 192, 64, 64, 224, 0, },  //glyph '1' code=16
{ 4, 224, 16, 96, 128, 240, 0, }, //glyph '2' code=17
{ 4, 224, 16, 96, 16, 224, 0, },  //glyph '3' code=18
{ 4, 144, 144, 240, 16, 16, 0, }, //glyph '4' code=19
{ 4, 240, 128, 224, 16, 224, 0, },  //glyph '5' code=20
{ 4, 96, 128, 224, 144, 96, 0, }, //glyph '6' code=21
{ 4, 240, 16, 32, 64, 64, 0, }, //glyph '7' code=22
{ 4, 96, 144, 96, 144, 96, 0, },  //glyph '8' code=23
{ 4, 96, 144, 112, 16, 96, 0, },  //glyph '9' code=24
{ 1, 0, 128, 0, 128, 0, 0, }, //glyph ':' code=25
{ 2, 0, 128, 0, 0, 128, 64, },  //glyph ';' code=26
{ 3, 32, 64, 128, 64, 32, 0, }, //glyph '<' code=27
{ 3, 0, 224, 0, 224, 0, 0, }, //glyph '=' code=28
{ 3, 128, 64, 32, 64, 128, 0, },  //glyph '>' code=29
{ 4, 224, 16, 96, 0, 64, 0, },  //glyph '?' code=30
{ 4, 96, 144, 176, 128, 112, 0, },  //glyph '@' code=31
{ 4, 96, 144, 240, 144, 144, 0, },  //glyph 'A' code=32
{ 4, 224, 144, 224, 144, 224, 0, }, //glyph 'B' code=33
{ 4, 112, 128, 128, 128, 112, 0, }, //glyph 'C' code=34
{ 4, 224, 144, 144, 144, 224, 0, }, //glyph 'D' code=35
{ 4, 240, 128, 224, 128, 240, 0, }, //glyph 'E' code=36
{ 4, 240, 128, 224, 128, 128, 0, }, //glyph 'F' code=37
{ 4, 112, 128, 176, 144, 112, 0, }, //glyph 'G' code=38
{ 4, 144, 144, 240, 144, 144, 0, }, //glyph 'H' code=39
{ 3, 224, 64, 64, 64, 224, 0, },  //glyph 'I' code=40
{ 4, 240, 16, 16, 144, 96, 0, },  //glyph 'J' code=41
{ 4, 144, 160, 192, 160, 144, 0, }, //glyph 'K' code=42
{ 4, 128, 128, 128, 128, 240, 0, }, //glyph 'L' code=43
{ 5, 136, 216, 168, 136, 136, 0, }, //glyph 'M' code=44
{ 4, 144, 208, 176, 144, 144, 0, }, //glyph 'N' code=45
{ 4, 96, 144, 144, 144, 96, 0, }, //glyph 'O' code=46
{ 4, 224, 144, 224, 128, 128, 0, }, //glyph 'P' code=47
{ 4, 96, 144, 144, 144, 96, 16, },  //glyph 'Q' code=48
{ 4, 224, 144, 224, 160, 144, 0, }, //glyph 'R' code=49
{ 4, 112, 128, 96, 16, 224, 0, }, //glyph 'S' code=50
{ 3, 224, 64, 64, 64, 64, 0, }, //glyph 'T' code=51
{ 4, 144, 144, 144, 144, 96, 0, },  //glyph 'U' code=52
{ 3, 160, 160, 160, 160, 64, 0, },  //glyph 'V' code=53
{ 5, 136, 168, 168, 168, 80, 0, },  //glyph 'W' code=54
{ 4, 144, 144, 96, 144, 144, 0, },  //glyph 'X' code=55
{ 3, 160, 160, 64, 64, 64, 0, },  //glyph 'Y' code=56
{ 4, 240, 16, 96, 128, 240, 0, }, //glyph 'Z' code=57
{ 2, 192, 128, 128, 128, 192, 0, }, //glyph '[' code=58
{ 5, 128, 64, 32, 16, 8, 0, },  //glyph '\' code=59
{ 2, 192, 64, 64, 64, 192, 0, },  //glyph ']' code=60
{ 5, 32, 80, 136, 0, 0, 0, }, //glyph '^' code=61
{ 4, 0, 0, 0, 0, 240, 0, }, //glyph '_' code=62
{ 2, 128, 64, 0, 0, 0, 0, },  //glyph '`' code=63
{ 3, 0, 224, 32, 224, 224, 0, },  //glyph 'a' code=64
{ 3, 128, 224, 160, 160, 224, 0, }, //glyph 'b' code=65
{ 3, 0, 224, 128, 128, 224, 0, }, //glyph 'c' code=66
{ 3, 32, 224, 160, 160, 224, 0, },  //glyph 'd' code=67
{ 3, 0, 224, 224, 128, 224, 0, }, //glyph 'e' code=68
{ 2, 64, 128, 192, 128, 128, 0, },  //glyph 'f' code=69
{ 3, 0, 224, 160, 224, 32, 224, },  //glyph 'g' code=70
{ 3, 128, 224, 160, 160, 160, 0, }, //glyph 'h' code=71
{ 1, 128, 0, 128, 128, 128, 0, }, //glyph 'i' code=72
{ 2, 0, 192, 64, 64, 64, 128, },  //glyph 'j' code=73
{ 3, 128, 160, 192, 160, 160, 0, }, //glyph 'k' code=74
{ 1, 128, 128, 128, 128, 128, 0, }, //glyph 'l' code=75
{ 5, 0, 248, 168, 168, 168, 0, }, //glyph 'm' code=76
{ 3, 0, 224, 160, 160, 160, 0, }, //glyph 'n' code=77
{ 3, 0, 224, 160, 160, 224, 0, }, //glyph 'o' code=78
{ 3, 0, 224, 160, 160, 224, 128, }, //glyph 'p' code=79
{ 3, 0, 224, 160, 160, 224, 32, },  //glyph 'q' code=80
{ 3, 0, 224, 128, 128, 128, 0, }, //glyph 'r' code=81
{ 2, 0, 192, 128, 64, 192, 0, },  //glyph 's' code=82
{ 3, 64, 224, 64, 64, 64, 0, }, //glyph 't' code=83
{ 3, 0, 160, 160, 160, 224, 0, }, //glyph 'u' code=84
{ 3, 0, 160, 160, 160, 64, 0, },  //glyph 'v' code=85
{ 5, 0, 168, 168, 168, 80, 0, },  //glyph 'w' code=86
{ 3, 0, 160, 64, 160, 160, 0, },  //glyph 'x' code=87
{ 3, 0, 160, 160, 224, 32, 224, },  //glyph 'y' code=88
{ 2, 0, 192, 64, 128, 192, 0, },  //glyph 'z' code=89
{ 3, 96, 64, 192, 64, 96, 0, }, //glyph '{' code=90
{ 1, 128, 128, 128, 128, 128, 0, }, //glyph '|' code=91
{ 3, 192, 64, 96, 64, 192, 0, },  //glyph '}' code=92
{ 3, 96, 192, 0, 0, 0, 0, },  //glyph '~' code=93
{ 4, 48, 64, 224, 64, 240, 0, },  //glyph '£' code=94
};

static const char str0[] PROGMEM="0"; 
static const char str1[] PROGMEM="1"; 
static const char str2[] PROGMEM="2"; 
static const char str3[] PROGMEM="3"; 
static const char str4[] PROGMEM="4"; 
static const char str5[] PROGMEM="5"; 
static const char str6[] PROGMEM="6"; 
static const char str7[] PROGMEM="7"; 
static const char str8[] PROGMEM="8"; 
static const char str9[] PROGMEM="9"; 
static const char str10[] PROGMEM="#"; 
static const char str20[] PROGMEM="Arduino VGA Snake"; 
static const char str22[] PROGMEM="Game Over"; 
static const char str23[] PROGMEM="Score"; 


void setup() {
  vga.begin();
  randomSeed(analogRead(5)); 
}

boolean button1 = 0;
boolean button2 = 0;
boolean button3 = 0;
boolean button4 = 0;
boolean button; 
byte counterMenu = 0;
byte counterMenu2 = 0; 
byte state = 1;
byte score = 0; 
byte scoreMax = 10; 
byte foodX = 60; 
byte foodY = 30;
byte snakeMaxLength = 55; 
byte sx[55];      // > slength + scoreMax*delta + 1 = 40
byte sy[55];
byte slength = 3; // snake starting length 
byte delta = 5;   // snak length increment 
byte wleft = 36; 
int i;
byte x; 
byte y; 
byte direct = 3; 
int speedDelay = 100; 


void foodIni() {
  do{
     foodX = random(VGAX_WIDTH - 4 - wleft) + 2 + wleft;  
     foodY = random(VGAX_HEIGHT - 4) + 2;
  } while ( vga.getpixel(foodX, foodY) > 1 ); 
}


void processInputs() {
  button1 = digitalRead(BUTTON_1); 
  button2 = digitalRead(BUTTON_2);
  button3 = digitalRead(BUTTON_3); 
  button4 = digitalRead(BUTTON_4);
  button = button1 | button2 | button3 | button4; 
}


void drawMenu() {
  counterMenu2++; 
  vga.delay(10); 
  if (counterMenu2 > 50){
    counterMenu++; 
    vgaPrint(str20, 26, 16, (counterMenu%3) + 1);
    counterMenu2 = 0; 
  }
}


void drawBorder() {
    vgaU.draw_line(wleft, 0, VGAX_WIDTH-1, 0, 3);
    vgaU.draw_line(wleft, VGAX_HEIGHT-1, VGAX_WIDTH-1, VGAX_HEIGHT-1, 3);
    vgaU.draw_line(wleft, 0, wleft, VGAX_HEIGHT-1, 3);
    vgaU.draw_line(VGAX_WIDTH-1, 0, VGAX_WIDTH-1, VGAX_HEIGHT, 3);
}


void drawScore() {
  vgaPrint(str23, 10, 3, 2);
  vgaPrint(str10, 20, 10, 0);
  if (score < 10) {vgaPrintNumber(score, 20, 10, 2);}
  else { 
       vgaPrint(str10, 15, 10, 0);
       vgaPrintNumber(1, 15, 10, 2);
       if (score == 10) {vgaPrintNumber(0, 20, 10, 2);}
       if (score == 11) {vgaPrintNumber(1, 20, 10, 2);}
       if (score == 12) {vgaPrintNumber(2, 20, 10, 2);}

  }
}


void drawStartScreen() {
   vga.clear(0);
   drawBorder(); 
   drawSnakeIni(); 
   drawScore(); 
   button = 0;
   vga.delay(200);
} 


void drawSnakeIni() {
   for (byte i = 0; i < slength ; i++) {
      sx[i] = 80 + i;
      sy[i] = 30; 
      vga.putpixel(int(sx[i]), int(sy[i]), 2);
   }
   for (byte i = slength; i < snakeMaxLength ; i++) {
     sx[i] = 1;
     sy[i] = 1; 
  }
   vga.putpixel(foodX, foodY, 1);
}


void newMatch(){
  score = 0;
  slength = 3; 
  i = slength - 1;  
  vga.clear(0);  
  drawBorder();
  drawScore(); 
  vga.putpixel(foodX, foodY, 1);
}



void loop() {
  
  processInputs(); 
  
  if(state == 1) { 
     drawMenu();
     vga.delay(10);
     processInputs(); 
     if (button == 1){ 
        button = 0;
        vga.clear(0);
        drawStartScreen(); 
        state = 2; 
     }
  }
  
 if(state == 2){
     if(score == scoreMax || score == 0){
        processInputs(); 
     }
     if (button == 1){ 
        score = 0;
        drawScore(); 
        button = 0;
        button1 = 0;
        button2 = 0;
        button3 = 0;
        button4 = 0;
        direct = 3; 
        x = -1;
        y = 0; 
        i = slength - 1; 
        state = 3; 
     }
  }
  
  if(state == 3) { 
     processInputs(); 
     if (direct == 1){
        if (button2 == 1){ x = 0; y = -1; direct = 2; button4 = 0;}
        if (button4 == 1){ x = 0; y = +1; direct = 4;}
     }
     else {
        if (direct == 2){
           if (button1 == 1){ x = +1; y = 0; direct = 1; button3 = 0;}
           if (button3 == 1){ x = -1; y = 0; direct = 3;}
        }
        else {
           if (direct == 3){
              if (button2 == 1){ x = 0; y = -1; direct = 2; button4 = 0;}
              if (button4 == 1){ x = 0; y = +1; direct = 4;}
           }
           else { 
              if (direct == 4){
                 if (button1 == 1){ x = +1; y = 0; direct = 1; button3 = 0;}
                 if (button3 == 1){ x = -1; y = 0; direct = 3;}
              }
           }
        }
     }
  
     vga.putpixel(int(sx[i]), int(sy[i]), 0);
     if ( i == slength - 1){
        sx[i] = sx[0] + x; 
        sy[i] = sy[0] + y; 
     }
     else {
        sx[i] = sx[i + 1] + x; 
        sy[i] = sy[i + 1] + y; 
     }

     if ( sx[i] == foodX && sy[i] == foodY){ 
        foodIni(); 
        drawBorder(); 
        vga.putpixel(foodX, foodY, 1);
        slength = slength + delta; 
        score++; 
        if (score > scoreMax) {
           newMatch();
        }
        drawScore(); 
     }

     if (vga.getpixel(int(sx[i]), int(sy[i])) == 0 || vga.getpixel(int(sx[i]), int(sy[i])) == 1 ){
        vga.putpixel(int(sx[i]), int(sy[i]), 2); 
     }
     else
     {
        vgaPrint(str22, 58, 24, 1);
        vga.delay(300);
        button == 0; 
        while(button == 0){processInputs();}
        speedDelay = 100; 
        newMatch();
        drawSnakeIni();
     }
   
     i--;
     if ( i < 0){i = slength - 1;}
 
     vga.delay(speedDelay); 

  }
} 


void vgaPrint(const char* str, byte x, byte y, byte color){
   vga.printPROGMEM((byte*)fnt_nanofont_data, FNT_NANOFONT_SYMBOLS_COUNT, FNT_NANOFONT_HEIGHT, 3, 1, str, x, y, color);
}


void vgaPrintNumber(byte number, byte x, byte y, byte color){
   char scoreChar[2];
   sprintf(scoreChar,"%d",number);
   vga.printSRAM((byte*)fnt_nanofont_data, FNT_NANOFONT_SYMBOLS_COUNT, FNT_NANOFONT_HEIGHT, 1, 1, scoreChar, x, y, color);
}


