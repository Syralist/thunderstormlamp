#include "Adafruit_NeoPixel.h"
#include "TimerOne.h"

#define INDEX(x,y) ((x)+(WIDTH*(y)))

//define the data pin
#define PIN 9
//define number of pixels
#define NUMPIXEL 96
//define number of columns and rows
#define ROWS 8
#define COLUMNS 12

//define input pins for decade switch
#define DEK2 5
#define DEK4 4
#define DEK8 3
#define DEK16 2

typedef struct
{
    int x;
    int y;
    float b;
} LedDef;

const int pattern01num = 8;
const int pattern01frames = 14;
LedDef *pattern01;//[pattern01frames][pattern01num];

typedef struct
{
    int num;
    int frames;
    LedDef *pattern;
} PatternDef;

const int numPatterns = 1;
PatternDef patterns[numPatterns];
volatile bool newpattern = true;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXEL, PIN, NEO_GRB | NEO_KHZ800);

//cycle counter
volatile unsigned int cycles = 0;

//variables for the flash functions
volatile unsigned long flashCount = 0;
volatile unsigned long loopCount = 0;
volatile float period = 0;
volatile int pixel = 1;
volatile int pause = 0;

//global parameters
float maxbrightness = 127.0;
const float cycle = 10;

//variables for decade switch
unsigned int last_dek_change = 0;
unsigned int letzte_dekade = 0;
bool neue_dekade = false;
unsigned int dekade = 0;

//variables for led adressing
const int ledmatrix[ROWS][COLUMNS] = {{61, 60, 59, 58, 57,  1,  2,  3,  4,  5,  6,  7},
                                {62, 63, 64, 65, 66, 14, 13, 12, 11, 10,  9,  8},
                                {71, 70, 69, 68, 67, 15, 16, 17, 18, 19, 20, 21},
                                {72, 73, 74, 75, 76, 28, 27, 26, 25, 24, 23, 22},
                                {81, 80, 79, 78, 77, 29, 30, 31, 32, 33, 34, 35},
                                {82, 83, 84, 85, 86, 42, 41, 40, 39, 38, 37, 36},
                                {91, 90, 89, 88, 87, 43, 44, 45, 46, 47, 48, 49},
                                {92, 93, 94, 95, 96, 56, 55, 54, 53, 52, 51, 50}};

/*****Setup*****/
void setup() {
    strip.begin();                  // Initialize NeoPixel object
    strip.show();                   // Initialize all pixels to '}ff'
    Serial.begin(9600);             // Initialize serial connecti}n
    Timer1.initialize(10000);       // Initialize timer to 10ms
    Timer1.attachInterrupt(flash);  // Attach funciton flash to timer interupt
    randomSeed(analogRead(0));      // Initialize random numbers with open analog pin
    pinMode(DEK2, INPUT_PULLUP);    // Set Pinmode
    pinMode(DEK4, INPUT_PULLUP);    // Set Pinmode
    pinMode(DEK8, INPUT_PULLUP);    // Set Pinmode
    pinMode(DEK16, INPUT_PULLUP);   // Set Pinmode
    initPatterns();
}

/*****Flash*****/
void flash()
{
    float brightness = 0.0;
    uint32_t color = 0;
    uint32_t colorfade = 0;
    int frame = 0;

    LedDef *pattern;
    int patternframes;
    int patternnum;
    int WIDTH;

    int randPattern;

    cycles++;
    switch(dekade)
    {
        case 1:
            period = 4000;
            brightness = pattern2(flashCount*cycle/period);
            flashCount++;
            color = strip.Color(maxbrightness,maxbrightness,maxbrightness);
            colorfade = strip.Color(brightness*maxbrightness,brightness*maxbrightness,brightness*maxbrightness);
            setAllPixels(color);
            strip.setPixelColor(pixel, colorfade);
            if(flashCount * cycle == period)
            {
                flashCount = 0;
                pixel = random(NUMPIXEL);
            }
            break;
        case 2:
            period = 2000;
            brightness = pattern1(flashCount*cycle/period);
            flashCount++;
            if(pause == 0)
            {
                color = strip.Color(brightness*maxbrightness,brightness*maxbrightness,brightness*maxbrightness);
                if(flashCount * cycle >= period)
                {
                    flashCount = 0;
                    pause = 1000 * random(2, 15);
                }
            }
            else
            {
                color = strip.Color(0,0,0);
                if(flashCount * cycle >= pause)
                {
                    flashCount = 0;
                    pause = 0;
                }
            }
            setAllPixels(color);
            break;
        case 3:
            period = pattern01frames * cycle * 4;
            flashCount++;
            if(newpattern)
            {
                randPattern = 0;//random(0, numPatterns-1);
                pattern = patterns[randPattern].pattern;
                patternframes = patterns[randPattern].frames;
                patternnum = patterns[randPattern].num;
                WIDTH = patternnum;
                newpattern = false;
                Serial.print(patternframes);
                Serial.print("\n");
                Serial.print(patternnum);
                Serial.print("\n");
            }
            if(pause == 0)
            {
                color = strip.Color(0,0,0);
                setAllPixels(color);
                frame = flashCount/4;
                if( frame < 0 || frame > patternframes )
                {
                    frame = 0;
                }
                for(int i = 0; i < patternnum; i++)
                {
                    brightness =  pattern[INDEX(i,frame)].b * maxbrightness;
                    Serial.print(brightness);
                    Serial.print("\n");
                    color = strip.Color(brightness, brightness, brightness);
                    strip.setPixelColor(ledmatrix[0+pattern[INDEX(i,frame)].y][6+pattern[INDEX(i,frame)].x], color);
                }
                if(flashCount * cycle >= period)
                {
                    flashCount = 0;
                    pause = 1000 * random(2, 15);
                    newpattern = true;
                    Serial.print(pause);
                    Serial.print("\n");
                }
            }
            else
            {
                color = strip.Color(0,0,0);
                setAllPixels(color);
                if(flashCount * cycle >= pause)
                {
                    flashCount = 0;
                    pause = 0;
                }
            }
            break;
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        default:
            color = strip.Color(0,0,0);
            setAllPixels(color);
            break;
    }
    strip.show();
}

/*****Loop*****/
void loop()
{
    dekade = dekadenschalter();
    switch(dekade)
    {
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        default:
            break;
    }
}

/*****Dekade*****/
unsigned int dekadenschalter()
{
    bool bit1 = !digitalRead(DEK2);
    bool bit2 = !digitalRead(DEK4);
    bool bit3 = !digitalRead(DEK8);
    bool bit4 = !digitalRead(DEK16);
    dekade = (((((bit4 << 1) + bit3) << 1) + bit2) << 1) + bit1;
    if(dekade != letzte_dekade)
    {
        letzte_dekade = dekade;
        last_dek_change = cycles;
        neue_dekade = true;
    }
    if((cycles>last_dek_change) && ((cycles-last_dek_change)>=5) && neue_dekade)
    {
        Serial.print(dekade);
        Serial.print('\n');
        neue_dekade = false;
    }
    return dekade;
}

/*****setAllPixels*****/
void setAllPixels(uint32_t color)
{
    for(uint16_t i=0;i<NUMPIXEL;i++)
    {
        strip.setPixelColor(i, color);
    }
}

/*****setPixelRow*****/
void setPixelRow(int row, uint32_t color)
{
    for(uint16_t i=0;i<COLUMNS;i++)
    {
        strip.setPixelColor(ledmatrix[row][i], color);
    }
}

/*****setPixelColumn*****/
void setPixelColumn(int column, uint32_t color)
{
    for(uint16_t i=0;i<ROWS;i++)
    {
        strip.setPixelColor(ledmatrix[i][column], color);
    }
}

/*****pattern1*****/
/****flash the whole cloud****/
float pattern1(float num)
{
    return ((cos((num)*3.2*M_PI)+cos((1.5*num)*3.2*M_PI))+2)/4;;
}

/*****pattern2*****/
/****dim down and back up****/
float pattern2(float num)
{
    return (-sin((num)*1.0*M_PI)+1);
}

void initPatterns()
{
    int WIDTH;
    pattern01 = new LedDef[pattern01frames*pattern01num];
    WIDTH = pattern01num;
    pattern01[INDEX(0,0)].x = 0;
    pattern01[INDEX(0,0)].y = 0;
    pattern01[INDEX(0,0)].b = 0.0;
    pattern01[INDEX(0,1)].x = 1;
    pattern01[INDEX(0,1)].y = 0;
    pattern01[INDEX(0,1)].b = 0.0;
    pattern01[INDEX(0,2)].x = 2;
    pattern01[INDEX(0,2)].y = 0;
    pattern01[INDEX(0,2)].b = 0.0;
    pattern01[INDEX(0,3)].x = 3;
    pattern01[INDEX(0,3)].y = 0;
    pattern01[INDEX(0,3)].b = 0.0;
    pattern01[INDEX(0,4)].x = 0;
    pattern01[INDEX(0,4)].y = 1;
    pattern01[INDEX(0,4)].b = 0.0;
    pattern01[INDEX(0,5)].x = 1;
    pattern01[INDEX(0,5)].y = 1;
    pattern01[INDEX(0,5)].b = 0.0;
    pattern01[INDEX(0,6)].x = 2;
    pattern01[INDEX(0,6)].y = 1;
    pattern01[INDEX(0,6)].b = 0.0;
    pattern01[INDEX(0,7)].x = 3;
    pattern01[INDEX(0,7)].y = 1;
    pattern01[INDEX(0,7)].b = 0.0;
    pattern01[INDEX(1,0)].x = 0;
    pattern01[INDEX(1,0)].y = 0;
    pattern01[INDEX(1,0)].b = 0.5;
    pattern01[INDEX(1,1)].x = 1;
    pattern01[INDEX(1,1)].y = 0;
    pattern01[INDEX(1,1)].b = 0.5;
    pattern01[INDEX(1,2)].x = 2;
    pattern01[INDEX(1,2)].y = 0;
    pattern01[INDEX(1,2)].b = 0.0;
    pattern01[INDEX(1,3)].x = 3;
    pattern01[INDEX(1,3)].y = 0;
    pattern01[INDEX(1,3)].b = 0.0;
    pattern01[INDEX(1,4)].x = 0;
    pattern01[INDEX(1,4)].y = 1;
    pattern01[INDEX(1,4)].b = 0.5;
    pattern01[INDEX(1,5)].x = 1;
    pattern01[INDEX(1,5)].y = 1;
    pattern01[INDEX(1,5)].b = 0.5;
    pattern01[INDEX(1,6)].x = 2;
    pattern01[INDEX(1,6)].y = 1;
    pattern01[INDEX(1,6)].b = 0.0;
    pattern01[INDEX(1,7)].x = 3;
    pattern01[INDEX(1,7)].y = 1;
    pattern01[INDEX(1,7)].b = 0.0;
    pattern01[INDEX(2,0)].x = 0;
    pattern01[INDEX(2,0)].y = 0;
    pattern01[INDEX(2,0)].b = 0.1;
    pattern01[INDEX(2,1)].x = 1;
    pattern01[INDEX(2,1)].y = 0;
    pattern01[INDEX(2,1)].b = 0.1;
    pattern01[INDEX(2,2)].x = 2;
    pattern01[INDEX(2,2)].y = 0;
    pattern01[INDEX(2,2)].b = 0.0;
    pattern01[INDEX(2,3)].x = 3;
    pattern01[INDEX(2,3)].y = 0;
    pattern01[INDEX(2,3)].b = 0.0;
    pattern01[INDEX(2,4)].x = 0;
    pattern01[INDEX(2,4)].y = 1;
    pattern01[INDEX(2,4)].b = 0.0;
    pattern01[INDEX(2,5)].x = 1;
    pattern01[INDEX(2,5)].y = 1;
    pattern01[INDEX(2,5)].b = 0.0;
    pattern01[INDEX(2,6)].x = 2;
    pattern01[INDEX(2,6)].y = 1;
    pattern01[INDEX(2,6)].b = 0.0;
    pattern01[INDEX(2,7)].x = 3;
    pattern01[INDEX(2,7)].y = 1;
    pattern01[INDEX(2,7)].b = 0.0;
    pattern01[INDEX(3,0)].x = 0;
    pattern01[INDEX(3,0)].y = 0;
    pattern01[INDEX(3,0)].b = 0.0;
    pattern01[INDEX(3,1)].x = 1;
    pattern01[INDEX(3,1)].y = 0;
    pattern01[INDEX(3,1)].b = 0.0;
    pattern01[INDEX(3,2)].x = 2;
    pattern01[INDEX(3,2)].y = 0;
    pattern01[INDEX(3,2)].b = 0.0;
    pattern01[INDEX(3,3)].x = 3;
    pattern01[INDEX(3,3)].y = 0;
    pattern01[INDEX(3,3)].b = 0.0;
    pattern01[INDEX(3,4)].x = 0;
    pattern01[INDEX(3,4)].y = 1;
    pattern01[INDEX(3,4)].b = 0.0;
    pattern01[INDEX(3,5)].x = 1;
    pattern01[INDEX(3,5)].y = 1;
    pattern01[INDEX(3,5)].b = 0.0;
    pattern01[INDEX(3,6)].x = 2;
    pattern01[INDEX(3,6)].y = 1;
    pattern01[INDEX(3,6)].b = 0.0;
    pattern01[INDEX(3,7)].x = 3;
    pattern01[INDEX(3,7)].y = 1;
    pattern01[INDEX(3,7)].b = 0.0;
    pattern01[INDEX(4,0)].x = 0;
    pattern01[INDEX(4,0)].y = 0;
    pattern01[INDEX(4,0)].b = 0.0;
    pattern01[INDEX(4,1)].x = 1;
    pattern01[INDEX(4,1)].y = 0;
    pattern01[INDEX(4,1)].b = 0.0;
    pattern01[INDEX(4,2)].x = 2;
    pattern01[INDEX(4,2)].y = 0;
    pattern01[INDEX(4,2)].b = 0.0;
    pattern01[INDEX(4,3)].x = 3;
    pattern01[INDEX(4,3)].y = 0;
    pattern01[INDEX(4,3)].b = 0.0;
    pattern01[INDEX(4,4)].x = 0;
    pattern01[INDEX(4,4)].y = 1;
    pattern01[INDEX(4,4)].b = 0.0;
    pattern01[INDEX(4,5)].x = 1;
    pattern01[INDEX(4,5)].y = 1;
    pattern01[INDEX(4,5)].b = 0.0;
    pattern01[INDEX(4,6)].x = 2;
    pattern01[INDEX(4,6)].y = 1;
    pattern01[INDEX(4,6)].b = 0.0;
    pattern01[INDEX(4,7)].x = 3;
    pattern01[INDEX(4,7)].y = 1;
    pattern01[INDEX(4,7)].b = 0.0;
    pattern01[INDEX(5,0)].x = 0;
    pattern01[INDEX(5,0)].y = 0;
    pattern01[INDEX(5,0)].b = 0.0;
    pattern01[INDEX(5,1)].x = 1;
    pattern01[INDEX(5,1)].y = 0;
    pattern01[INDEX(5,1)].b = 0.0;
    pattern01[INDEX(5,2)].x = 2;
    pattern01[INDEX(5,2)].y = 0;
    pattern01[INDEX(5,2)].b = 0.0;
    pattern01[INDEX(5,3)].x = 3;
    pattern01[INDEX(5,3)].y = 0;
    pattern01[INDEX(5,3)].b = 0.0;
    pattern01[INDEX(5,4)].x = 0;
    pattern01[INDEX(5,4)].y = 1;
    pattern01[INDEX(5,4)].b = 0.0;
    pattern01[INDEX(5,5)].x = 1;
    pattern01[INDEX(5,5)].y = 1;
    pattern01[INDEX(5,5)].b = 0.0;
    pattern01[INDEX(5,6)].x = 2;
    pattern01[INDEX(5,6)].y = 1;
    pattern01[INDEX(5,6)].b = 0.0;
    pattern01[INDEX(5,7)].x = 3;
    pattern01[INDEX(5,7)].y = 1;
    pattern01[INDEX(5,7)].b = 0.0;
    pattern01[INDEX(6,0)].x = 0;
    pattern01[INDEX(6,0)].y = 0;
    pattern01[INDEX(6,0)].b = 0.0;
    pattern01[INDEX(6,1)].x = 1;
    pattern01[INDEX(6,1)].y = 0;
    pattern01[INDEX(6,1)].b = 0.2;
    pattern01[INDEX(6,2)].x = 2;
    pattern01[INDEX(6,2)].y = 0;
    pattern01[INDEX(6,2)].b = 0.0;
    pattern01[INDEX(6,3)].x = 3;
    pattern01[INDEX(6,3)].y = 0;
    pattern01[INDEX(6,3)].b = 0.0;
    pattern01[INDEX(6,4)].x = 0;
    pattern01[INDEX(6,4)].y = 1;
    pattern01[INDEX(6,4)].b = 0.0;
    pattern01[INDEX(6,5)].x = 1;
    pattern01[INDEX(6,5)].y = 1;
    pattern01[INDEX(6,5)].b = 0.0;
    pattern01[INDEX(6,6)].x = 2;
    pattern01[INDEX(6,6)].y = 1;
    pattern01[INDEX(6,6)].b = 0.0;
    pattern01[INDEX(6,7)].x = 3;
    pattern01[INDEX(6,7)].y = 1;
    pattern01[INDEX(6,7)].b = 0.0;
    pattern01[INDEX(7,0)].x = 0;
    pattern01[INDEX(7,0)].y = 0;
    pattern01[INDEX(7,0)].b = 0.0;
    pattern01[INDEX(7,1)].x = 1;
    pattern01[INDEX(7,1)].y = 0;
    pattern01[INDEX(7,1)].b = 0.1;
    pattern01[INDEX(7,2)].x = 2;
    pattern01[INDEX(7,2)].y = 0;
    pattern01[INDEX(7,2)].b = 0.0;
    pattern01[INDEX(7,3)].x = 3;
    pattern01[INDEX(7,3)].y = 0;
    pattern01[INDEX(7,3)].b = 0.0;
    pattern01[INDEX(7,4)].x = 0;
    pattern01[INDEX(7,4)].y = 1;
    pattern01[INDEX(7,4)].b = 0.0;
    pattern01[INDEX(7,5)].x = 1;
    pattern01[INDEX(7,5)].y = 1;
    pattern01[INDEX(7,5)].b = 0.0;
    pattern01[INDEX(7,6)].x = 2;
    pattern01[INDEX(7,6)].y = 1;
    pattern01[INDEX(7,6)].b = 0.0;
    pattern01[INDEX(7,7)].x = 3;
    pattern01[INDEX(7,7)].y = 1;
    pattern01[INDEX(7,7)].b = 0.0;
    pattern01[INDEX(8,0)].x = 0;
    pattern01[INDEX(8,0)].y = 0;
    pattern01[INDEX(8,0)].b = 0.0;
    pattern01[INDEX(8,1)].x = 1;
    pattern01[INDEX(8,1)].y = 0;
    pattern01[INDEX(8,1)].b = 0.4;
    pattern01[INDEX(8,2)].x = 2;
    pattern01[INDEX(8,2)].y = 0;
    pattern01[INDEX(8,2)].b = 0.4;
    pattern01[INDEX(8,3)].x = 3;
    pattern01[INDEX(8,3)].y = 0;
    pattern01[INDEX(8,3)].b = 0.0;
    pattern01[INDEX(8,4)].x = 0;
    pattern01[INDEX(8,4)].y = 1;
    pattern01[INDEX(8,4)].b = 0.0;
    pattern01[INDEX(8,5)].x = 1;
    pattern01[INDEX(8,5)].y = 1;
    pattern01[INDEX(8,5)].b = 0.1;
    pattern01[INDEX(8,6)].x = 2;
    pattern01[INDEX(8,6)].y = 1;
    pattern01[INDEX(8,6)].b = 0.1;
    pattern01[INDEX(8,7)].x = 3;
    pattern01[INDEX(8,7)].y = 1;
    pattern01[INDEX(8,7)].b = 0.0;
    pattern01[INDEX(9,0)].x = 0;
    pattern01[INDEX(9,0)].y = 0;
    pattern01[INDEX(9,0)].b = 0.0;
    pattern01[INDEX(9,1)].x = 1;
    pattern01[INDEX(9,1)].y = 0;
    pattern01[INDEX(9,1)].b = 0.1;
    pattern01[INDEX(9,2)].x = 2;
    pattern01[INDEX(9,2)].y = 0;
    pattern01[INDEX(9,2)].b = 0.6;
    pattern01[INDEX(9,3)].x = 3;
    pattern01[INDEX(9,3)].y = 0;
    pattern01[INDEX(9,3)].b = 0.6;
    pattern01[INDEX(9,4)].x = 0;
    pattern01[INDEX(9,4)].y = 1;
    pattern01[INDEX(9,4)].b = 0.0;
    pattern01[INDEX(9,5)].x = 1;
    pattern01[INDEX(9,5)].y = 1;
    pattern01[INDEX(9,5)].b = 0.0;
    pattern01[INDEX(9,6)].x = 2;
    pattern01[INDEX(9,6)].y = 1;
    pattern01[INDEX(9,6)].b = 0.1;
    pattern01[INDEX(9,7)].x = 3;
    pattern01[INDEX(9,7)].y = 1;
    pattern01[INDEX(9,7)].b = 0.1;
    pattern01[INDEX(10,0)].x = 0;
    pattern01[INDEX(10,0)].y = 0;
    pattern01[INDEX(10,0)].b = 0.0;
    pattern01[INDEX(10,1)].x = 1;
    pattern01[INDEX(10,1)].y = 0;
    pattern01[INDEX(10,1)].b = 0.1;
    pattern01[INDEX(10,2)].x = 2;
    pattern01[INDEX(10,2)].y = 0;
    pattern01[INDEX(10,2)].b = 0.1;
    pattern01[INDEX(10,3)].x = 3;
    pattern01[INDEX(10,3)].y = 0;
    pattern01[INDEX(10,3)].b = 0.2;
    pattern01[INDEX(10,4)].x = 0;
    pattern01[INDEX(10,4)].y = 1;
    pattern01[INDEX(10,4)].b = 0.0;
    pattern01[INDEX(10,5)].x = 1;
    pattern01[INDEX(10,5)].y = 1;
    pattern01[INDEX(10,5)].b = 0.0;
    pattern01[INDEX(10,6)].x = 2;
    pattern01[INDEX(10,6)].y = 1;
    pattern01[INDEX(10,6)].b = 0.0;
    pattern01[INDEX(10,7)].x = 3;
    pattern01[INDEX(10,7)].y = 1;
    pattern01[INDEX(10,7)].b = 0.0;
    pattern01[INDEX(11,0)].x = 0;
    pattern01[INDEX(11,0)].y = 0;
    pattern01[INDEX(11,0)].b = 0.0;
    pattern01[INDEX(11,1)].x = 1;
    pattern01[INDEX(11,1)].y = 0;
    pattern01[INDEX(11,1)].b = 0.4;
    pattern01[INDEX(11,2)].x = 2;
    pattern01[INDEX(11,2)].y = 0;
    pattern01[INDEX(11,2)].b = 0.1;
    pattern01[INDEX(11,3)].x = 3;
    pattern01[INDEX(11,3)].y = 0;
    pattern01[INDEX(11,3)].b = 0.1;
    pattern01[INDEX(11,4)].x = 0;
    pattern01[INDEX(11,4)].y = 1;
    pattern01[INDEX(11,4)].b = 0.0;
    pattern01[INDEX(11,5)].x = 1;
    pattern01[INDEX(11,5)].y = 1;
    pattern01[INDEX(11,5)].b = 0.2;
    pattern01[INDEX(11,6)].x = 2;
    pattern01[INDEX(11,6)].y = 1;
    pattern01[INDEX(11,6)].b = 0.0;
    pattern01[INDEX(11,7)].x = 3;
    pattern01[INDEX(11,7)].y = 1;
    pattern01[INDEX(11,7)].b = 0.1;
    pattern01[INDEX(12,0)].x = 0;
    pattern01[INDEX(12,0)].y = 0;
    pattern01[INDEX(12,0)].b = 0.0;
    pattern01[INDEX(12,1)].x = 1;
    pattern01[INDEX(12,1)].y = 0;
    pattern01[INDEX(12,1)].b = 0.1;
    pattern01[INDEX(12,2)].x = 2;
    pattern01[INDEX(12,2)].y = 0;
    pattern01[INDEX(12,2)].b = 0.0;
    pattern01[INDEX(12,3)].x = 3;
    pattern01[INDEX(12,3)].y = 0;
    pattern01[INDEX(12,3)].b = 0.0;
    pattern01[INDEX(12,4)].x = 0;
    pattern01[INDEX(12,4)].y = 1;
    pattern01[INDEX(12,4)].b = 0.0;
    pattern01[INDEX(12,5)].x = 1;
    pattern01[INDEX(12,5)].y = 1;
    pattern01[INDEX(12,5)].b = 0.0;
    pattern01[INDEX(12,6)].x = 2;
    pattern01[INDEX(12,6)].y = 1;
    pattern01[INDEX(12,6)].b = 0.0;
    pattern01[INDEX(12,7)].x = 3;
    pattern01[INDEX(12,7)].y = 1;
    pattern01[INDEX(12,7)].b = 0.0;
    pattern01[INDEX(13,0)].x = 0;
    pattern01[INDEX(13,0)].y = 0;
    pattern01[INDEX(13,0)].b = 0.0;
    pattern01[INDEX(13,1)].x = 1;
    pattern01[INDEX(13,1)].y = 0;
    pattern01[INDEX(13,1)].b = 0.0;
    pattern01[INDEX(13,2)].x = 2;
    pattern01[INDEX(13,2)].y = 0;
    pattern01[INDEX(13,2)].b = 0.0;
    pattern01[INDEX(13,3)].x = 3;
    pattern01[INDEX(13,3)].y = 0;
    pattern01[INDEX(13,3)].b = 0.0;
    pattern01[INDEX(13,4)].x = 0;
    pattern01[INDEX(13,4)].y = 1;
    pattern01[INDEX(13,4)].b = 0.0;
    pattern01[INDEX(13,5)].x = 1;
    pattern01[INDEX(13,5)].y = 1;
    pattern01[INDEX(13,5)].b = 0.0;
    pattern01[INDEX(13,6)].x = 2;
    pattern01[INDEX(13,6)].y = 1;
    pattern01[INDEX(13,6)].b = 0.0;
    pattern01[INDEX(13,7)].x = 3;
    pattern01[INDEX(13,7)].y = 1;
    pattern01[INDEX(13,7)].b = 0.0;

    patterns[0].num = pattern01num;
    patterns[0].frames = pattern01frames;
    patterns[0].pattern = pattern01;
    Serial.print("pattern init\n");
}
