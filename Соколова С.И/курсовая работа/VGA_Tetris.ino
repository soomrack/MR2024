#include <math.h>
#define FNT_NANOFONT_HEIGHT 6
#define FNT_NANOFONT_SYMBOLS_COUNT 95
#include <VGAXUtils.h>


VGAX vga;
VGAXUtils vgaU;


// Используется при отображении символов как пиксельной графики.
const unsigned char fnt_nanofont_data[FNT_NANOFONT_SYMBOLS_COUNT][1+FNT_NANOFONT_HEIGHT] PROGMEM={
{ 1, 128, 128, 128, 0, 128, 0, }, //glyph '!' code=0
{ 3, 160, 160, 0, 0, 0, 0, }, //glyph '"' code=1
//{ 5, 80, 248, 80, 248, 80, 0, },  //glyph '#' code=2
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


// Текстовые строки, необходимы для текстового вывода
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
static const char str11[] PROGMEM="Arduino VGA Tetris"; 
static const char str13[] PROGMEM="Game"; 
static const char str14[] PROGMEM="Over!"; 

void setup() {
  vga.begin();
  randomSeed(analogRead(5)); 
}

boolean button = 0; // используются для хранения состояния кнопок
boolean button_1 = 0;
boolean button_2 = 0;
boolean button_3 = 0;
boolean button_4 = 0;
boolean button_5 = 0;
int block[4][2]={{0,0},{0,0},{0,0},{0,0}}; // Основной текущий блок, состоящий из 4-х ячеек (x, y), смещение относительно центра фигуры
int blockExt[4][2]={{0,0},{0,0},{0,0},{0,0}}; // Копия
int blockOld[4][2]={{0,0},{0,0},{0,0},{0,0}}; // Предыдущая позиция блока — используется для стирания перед перерисовкой
int blockTmp[4][2]={{0,0},{0,0},{0,0},{0,0}}; // Временный буфер — для расчетов (например, при повороте фигуры)
int blockTr[4][2]={{0,0},{0,0},{0,0},{0,0}}; // Трансформированный блок (после поворота, перемещения и т.п.)
int yLine[4] = {0,0,0,0}; // Для хранения координат строк
int yLineTmp[4] = {0,0,0,0}; 
int yCounter = 0; 
int x = 60; 
int y = 6; 
int z = 10; 
int score; // счет
int noLoop = -1; // Флаг, запрещающий повторный запуск цикла
int clock = 1; // Флаг поворота
int delta = 0;  // Разница времени (временной шаг для перемещения фигур вниз)
int color = 1; // Цвет текущего блока — используется для отрисовки в разном цвете
int colorOld; // Цвет предыдущего блока — используется для отрисовки в разном цвете
int blockN; // Номер текущего блока
int blockNext; // Номер следующего блока
int busy; // Флаг занятости
int noDelete = 0; // Отключает удаление строк
int k = 0; 
int a = 40;
int b = 10; 
int counterMenu = 0; // Счетчик пунктов меню
unsigned long time = 0; // Время последнего события
int fast = 14; // Скорость падения фигуры


// Обработки нажатий кнопок
void processInputs() 
{
  if(button_1 == 1) {
   // Если кнопка нажата считываем другие кнопки, сбрасываем button_1, аналогично для остальных
     button_2 = digitalRead(11);
     button_3 = digitalRead(12); 
     button_4 = digitalRead(13);
     button_1 = 0;
     vga.delay(25);
  }
  else{
     if(button_2 == 1) {
        button_1 = digitalRead(10);
        button_3 = digitalRead(12); 
        button_4 = digitalRead(13);
        button_2 = 0;
        vga.delay(25);
     }
     else{
        if(button_3 == 1) {
           button_1 = digitalRead(10);
           button_2 = digitalRead(11); 
           button_4 = digitalRead(13);
           button_3 = 0;
           vga.delay(25);
        }
        else{
           if(button_4 == 1) {
              button_1 = digitalRead(10);
              button_2 = digitalRead(11); 
              button_3 = digitalRead(12);                                          
              button_4 = 0;
              //vga.delay(25);                                                                    
           }
           else{
             // читаем все кнопки заново
              button_1 = digitalRead(10); 
              button_2 = digitalRead(11);
              button_3 = digitalRead(12); 
              button_4 = digitalRead(13);
           }
        }
     }
  }
  // Флаг button = 1, если нажата кнока 2 или 4, запуск движения фигуры влево или вправо 
  button = button_2 || button_4; 
}


// Сбросить состояние всех кнопок
void clearInputs() 
{
  button_1 = 0; 
  button_2 = 0;
  button_3 = 0; 
  button_4 = 0;
  button = 0; 
}


// Отображение стартового окна
void drawMenu() 
{
   // Ни одна кнопка не нажата, то есть игрок еще не начал игру
  while (button_1 == 0 && button_2 == 0 && button_3 == 0 && button_4 == 0) {
     processInputs(); // Обрабатываем кнопки, обновляем флаги
     vga.printPROGMEM((byte*)fnt_nanofont_data, FNT_NANOFONT_SYMBOLS_COUNT, FNT_NANOFONT_HEIGHT, 3, 1, str11, 26, 16, (counterMenu%3) + 1); // Печатаем Arduino VGA Tetris
     vga.delay(1000); // Создает меделенное мигание, смену цветов текста
     counterMenu++; // величивает счётчик, чтобы изменить поведение counterMenu % 3 + 1 на следующей итерации
  }
  vga.clear(0); // Когда нажата любая кнока, отчищаем игровое поле
  drawGameScreen(); // Рисуем игровой экран
  drawScore(score); // Отображаем очки
}


// Отображение текущего уровня очков
void drawScore(int idx) 
{
    if (idx > 39){ // Если очки превысили 39 счет обнуляется
       score = 0; 
       idx = 0; 
       fast = fast - 4; // Игра ускоряется, уменьшается или интервал движения блоков
       if (fast < 3) {fast = 2;} // устанавливается минимальный предел скорости, чтобы игра не стала слишком быстрой
    }
    vgaU.draw_line(20, 10, 20, 50, 3); // Рисует внешний контур вертикального столбика (цвет 3), от точки (20,10) до (20,50).
    vgaU.draw_line(20, 50, 20, 50 - idx, 1); // Заполняет внутри от низа вверх на i пикселей, цветом 1
    vgaU.draw_line(19, 10, 22, 10, 3); // Горизонтальная линия вверху столбика (граница)
    vgaU.draw_line(19, 50, 22, 50, 3); // Горизонтальная линия внизу столбика (граница)
}


// Рисует границу (рамку) вокруг игрового поля     
void drawBorder() 
{
   // общий размер экрана = 120/60
   // игровое поле тетриса: ширина = 30; высота = 60
   vgaU.draw_line(44,0,78,0,3); // верхняя граница
   vgaU.draw_line(44,59,78,59,3); // нижняя граница
   vgaU.draw_line(44,0,44,59,3); // левая граница
   vgaU.draw_line(78,0,78,60,3); // правая граница
}


// Отображение стартового экрана
void drawStartScreen() 
{
   drawBorder(); // Рисуем рамку игрового поля
   drawGameScreen(); // Рисуем игровое поле
   button = 0; // Флаг 0
   vga.delay(200); // Задержка 0,2 с
} 


// Главная функция отображения игрового экрана
void drawGameScreen() 
{
  drawBorder(); 
}


// Определение формы тетрамино
void blockDef(int idx) // В зависимости от idx выбирается фигура
{
  if (idx == 1){
  // O
  block[0][0] = 0; // Заполняем массив задавая форму фигуры
  block[0][1] = 0;
  block[1][0] = 1;
  block[1][1] = 0;
  block[2][0] = 0;
  block[2][1] = 1;
  block[3][0] = 1;
  block[3][1] = 1;
  color = 1; 
  }
  if (idx == 2){
  // L
  block[0][0] = -1;
  block[0][1] = 0;
  block[1][0] = 0;
  block[1][1] = 0;
  block[2][0] = 1;
  block[2][1] = 0;
  block[3][0] = -1;
  block[3][1] = 1;
  color = 2; 
  }
  if (idx == 3){
  // J
  block[0][0] = -1;
  block[0][1] = 0;
  block[1][0] = 0;
  block[1][1] = 0;
  block[2][0] = 1;
  block[2][1] = 0;
  block[3][0] = 1;
  block[3][1] = 1;
  color = 3; 
  }
  if (idx == 4){
  // I
  block[0][0] = -1;
  block[0][1] = 0;
  block[1][0] = 0;
  block[1][1] = 0;
  block[2][0] = 1;
  block[2][1] = 0;
  block[3][0] = 2;
  block[3][1] = 0;
    color = 1; 
  }
  if (idx == 5){
  // S
  block[0][0] = -1;
  block[0][1] = 0;
  block[1][0] = 0;
  block[1][1] = 0;
  block[2][0] = 0;
  block[2][1] = 1;
  block[3][0] = 1;
  block[3][1] = 1;
  color = 2; 
  }
  if (idx == 6){
  // Z
  block[0][0] = -1;
  block[0][1] = 1;
  block[1][0] = 0;
  block[1][1] = 1;
  block[2][0] = 0;
  block[2][1] = 0;
  block[3][0] = 1;
  block[3][1] = 0;
  color = 3; 
  }
  if (idx == 7){
  // T
  block[0][0] = -1;
  block[0][1] = 0;
  block[1][0] = 0;
  block[1][1] = 0;
  block[2][0] = 0;
  block[2][1] = 1;
  block[3][0] = 1;
  block[3][1] = 0;
  color = 1; 
  }
}

// Масштабируем под экран 4:3
void blockExtension() 
{
   for (int idx = 0; idx < 4; idx++){
      blockExt[idx][0] = block[idx][0] * 3;  // X координата умножается на 3
      blockExt[idx][1] = block[idx][1] * 2;  // Y координата умножается на 2
   }
}
 

// Поворот блока
void blockRotation(int clock)
{
   // Сохраняем предыдущую позицию
  for (int idx = 0; idx < 4; idx++){
     blockOld[idx][0] = block[idx][0]; 
     blockOld[idx][1] = block[idx][1];
  }
  for (int n = 0; n < 4; n++){
      block[n][0] = blockOld[n][1] * clock; // Поворот на 90 градусов; 1 по часовой, 0 против часовой
      block[n][1] = -blockOld[n][0] * clock;
  }
}


// Cмещение фигуры по координатам
void blockTranslation(int x, int y) 
{
   for (int idx = 0; idx < 4; idx++){
      blockTr[idx][0] = blockExt[idx][0] + x;
      blockTr[idx][1] = blockExt[idx][1] + y;
   }
}


// Удаление блока
void delBlock()
{
  if (noDelete == 1) {noDelete = 0;} // Смена флага
  else {
      for (int idx = 0; idx < 4; idx++){
         // Рисуем 2 горизонтальные линии для каждой из 4 клеток фигуры, цвет (0) - фон
         vgaU.draw_line(blockTr[idx][0],blockTr[idx][1],blockTr[idx][0] + 3,blockTr[idx][1],0); // используем экранные координаты, с учтенным масштабированием
         vgaU.draw_line(blockTr[idx][0],blockTr[idx][1] + 1,blockTr[idx][0] + 3,blockTr[idx][1] + 1,0);   
      }
   }
}


// Отрисовка блока
void drawBlock()
{
  for (int idx = 0; idx < 4; idx++){
     vgaU.draw_line(blockTr[idx][0],blockTr[idx][1],blockTr[idx][0] + 3,blockTr[idx][1], color);
     vgaU.draw_line(blockTr[idx][0],blockTr[idx][1] + 1,blockTr[idx][0] + 3,blockTr[idx][1] + 1, color);   
  }
  for (int n = 0; n < 4; n++){
     blockTmp[n][0] = blockTr[n][0]; // Сохраняем текущие экранные координаты
     blockTmp[n][1] = blockTr[n][1];
  }
}


// Повторная отрисовка ранее нарисованного блока
void drawBlockTmp() // Используется для перерисовки фигуры на том же месте после, например, временного удаления (delBlock()), либо при обновлении экрана.
{
  for (int idx = 0; idx < 4; idx++){
     vgaU.draw_line(blockTmp[idx][0],blockTmp[idx][1],blockTmp[idx][0] + 3,blockTmp[idx][1], color);
     vgaU.draw_line(blockTmp[idx][0],blockTmp[idx][1] + 1,blockTmp[idx][0] + 3,blockTmp[idx][1] + 1, color);   
  }
}


// Проверяет, может ли текущая фигура быть размещена на экране в новой позициим
void checkBlock()
{ 
  busy = 0; // если после цикла busy = 0 -> все пиксели пустые, фигура может быть размещена.
  for (int idx = 0; idx < 4; idx++){
     busy = busy + vga.getpixel(blockTr[idx][0], blockTr[idx][1]) + vga.getpixel(blockTr[idx][0] + 2, blockTr[idx][1]);
  }
}


// 
void replaceBlock()
{
     blockExtension(); // масштабируем размер
     blockTranslation(x, y); // перемещаем
     checkBlock(); // проверяем на столкновение
     if (busy == 0){ // если место свободно, фигура пермещается
        drawBlock(); 
     }
     else // если блок уперся 
     {
        drawBlockTmp(); // рисует блок в предыдущей позиции
        checkForFullLine(); // проверяет заполнение линии 
        noLoop = 0; // снимаем флаг, цикл падения завершен
        noDelete = 1; // запрещает стирать блок (он уже «упал» и стал частью поля)
        if (y < 6) { // если блок остановился слишком высоко, значит места больше нет
           gameOver();
        }
     }
     vga.delay(50);
}


// Конец игры
void gameOver(){
   noLoop = -1; 
   score = 0; // обнуляются очки
   fast = 14; // значение скорости возвращается к начальному
   clearInputs(); // отчищаем флагм для кнопок
   time = 0; 
   while (button_1 == 0 && button_2 == 0 && button_3 == 0 && button_4 == 0) { // пока не нажата какая-нибудь кновка, выводим GAME OVER
      processInputs(); 
      vga.printPROGMEM((byte*)fnt_nanofont_data, FNT_NANOFONT_SYMBOLS_COUNT, FNT_NANOFONT_HEIGHT, 3, 1, str13, 92, 30, 1);
      vga.printPROGMEM((byte*)fnt_nanofont_data, FNT_NANOFONT_SYMBOLS_COUNT, FNT_NANOFONT_HEIGHT, 3, 1, str14, 92, 38, 1);
      vga.delay(200);
   }
   vga.clear(0);  // отчищаем экран
   
}


// Отображение следующего блока
void drawBlockNext()
{
     blockExtension(); 
     blockTranslation(100, 10); // смещаем блок в правую часть экрана
     vgaU.draw_line(95, 8, 112, 8, 0);
     vgaU.draw_line(95, 9, 112, 9, 0);
     vgaU.draw_line(95, 10, 112, 10, 0);
     vgaU.draw_line(95, 11, 112, 11, 0);
     vgaU.draw_line(95, 12, 112, 12, 0);
     vgaU.draw_line(95, 13, 112, 13, 0);
     vgaU.draw_line(95, 14, 112, 14, 0);
     vgaU.draw_line(95, 15, 112, 15, 0);
     drawBlock(); 
}


// Проверка сдвига блока по горизонтали
void checkBlockTranslation()
{
     x = x + delta; // учитываем смещение
     blockExtension(); 
     blockTranslation(x, y); // формируем реальные координаты
     checkBlock(); // проверяем на столкновение
     if (busy == 0){
        drawBlock(); 
     }
     else
     {
        x = x - delta; 
        blockExtension(); 
        blockTranslation(x, y); 
        drawBlock(); 
     }
     vga.delay(50);
}


// Проверка возможности поворота
void checkBlockRotation()
{
     //x = x + delta; 
     blockExtension(); 
     blockTranslation(x, y); 
     checkBlock();
     if (busy == 0){ 
        drawBlock(); 
     }
     else // Если блок упирается (занято место)
     {
        clock = +1; // устанавливает направление поворота для отмены
        blockRotation(clock); // отмена поворота
        blockExtension(); 
        blockTranslation(x, y); 
        drawBlock(); 
     }
     vga.delay(50);
}


// Проверка заполненности линии
void checkForFullLine() 
{ 
   // Для каждого из 4 блкоков берем их коорлинаты
   for (int idx = 0; idx < 4; idx++){
      // По горизонтали (от x=45 до x=75 с шагом 3 пикселя) проверяем, сколько точек заполнено
      for (int jdx = 45; jdx < 76; jdx += 3) {
         if (vga.getpixel(jdx, blockTmp[idx][1]) >0){k++; }
      }
      if (k == 11) { // линия заполнена и должна быть удалена
         vgaU.draw_line(45, blockTmp[idx][1], 78, blockTmp[idx][1], 0); // линия отчищается с цветом 0
         vgaU.draw_line(45, blockTmp[idx][1] + 1, 78, blockTmp[idx][1] + 1, 0);   
         yLineTmp[yCounter] = blockTmp[idx][1]; // Сохраняется координата этой линии в массивах yLineTmp и yLine
         yLine[yCounter] = blockTmp[idx][1]; 
         yCounter++; // Счётчик найденных линий yCounter увеличивается
      }
      k = 0; 
    }

    // Проверяется порядок координат линий
    if (yLineTmp[yCounter - 1] < yLine[0]) { 
       for (int idx = 0; idx < yCounter; idx++) { 
         // Инверсия, чтобы упорядочить линии по возрастанию
         yLine[idx] = yLineTmp[yCounter - idx - 1]; 
       }
    }

    for (int idx = 0; idx < yCounter; idx++){ // смещение блоков вниз после удаления линий
      for (int y = yLine[idx] - 2; y > 0; y = y - 2) {
         for (int x = 45; x < 76; x += 3) {
            colorOld = vga.getpixel(x, y); 
            if (colorOld > 0) {
               // очищаем старую позицию
               vgaU.draw_line(x, y , x + 3, y , 0);
               vgaU.draw_line(x, y + 1, x + 3, y + 1, 0);
                // рисуем цвет на позиции ниже
               vgaU.draw_line(x, y + 2, x + 3, y + 2, colorOld);
               vgaU.draw_line(x, y + 3, x + 3, y + 3, colorOld);
            } 
         }
      }
   }

   //Если удалены линии (yCounter > 0), увеличивается счёт.
   if (yCounter != 0) {score = score + 2*int(pow(2, yCounter));} 
   drawScore(score); 
   yCounter = 0; 
}


void loop() {
  processInputs(); 
  if (noLoop < 1){
     blockN = blockNext; 
     if (noLoop == -1 ) { // только в начале игры
        drawMenu(); 
        while (button_1 == 0 && button_2 == 0 && button_3 == 0 && button_4 == 0) {
           blockN = int(random(7)) + 1; // рандомно выбирается блок
           processInputs(); // ждем нажатия кнопок
        }
     }
     drawGameScreen();
     drawScore(score);
     blockNext = int(random(8));  // случайная следующая фигура 0..7
     blockDef(blockNext); // задаём следующую фигуру
     drawBlockNext(); // рисуем следующую фигуру справа
     blockDef(blockN); // задаём текущую фигуру
     x = 57; // начальная позиция фигуры
     y = 3; 
     button_1 = 1; 
     noLoop = 1; // флаг, что игра в процессе
  }
  if (button_2 == 1){ //поворот
     if (button_2 == 1){clock = -1;} // направление поворота (против часовой)
     delBlock();  // стираем текущий блок с экрана
     blockRotation(clock);  // поворачиваем блок
     checkBlockRotation(); // проверяем, можно ли поворачивать (столкновения)
  }
  if (button_1 == 1 || button_3 == 1){ // перемещение
     if (button_1 == 1){delta = 3;} // перемещение вправо
     if (button_3 == 1){delta = -3;}  // перемещение влево
     delBlock(); // стираем текущий блок 
     checkBlockTranslation(); // проверяем можно ли сдвинуть, и если да — сдвигаем
  }
  time++; 
  if (time % fast > fast - 2 || button_4 == 1){ // если прошло достаточно времени или нажата кнопка ускорения падения
     if (fast < 3) {fast = 2;}  // минимальная скорость падения
     y = y + 2; // сдвигаем фигуру вниз
     delBlock();  // стираем текущий блок
     replaceBlock(); // проверяем место и рисуем фигуру на новом месте или фиксируем её
  }
  vga.delay(10 + 2*fast);
}


