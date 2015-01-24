#include "MemoryFree.h"
#include "Adafruit_NeoPixel.h"
#include "TimerOne.h"
#include <StandardCplusplus.h>
#include <vector>

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

typedef std::vector<LedDef*> Frame;
typedef std::vector<Frame*> Pattern;
typedef std::vector<Pattern*> PatternLib;

const int numPatterns = 1;
volatile bool newpattern = true;

PatternLib patterns;
Pattern *activePattern;
/* volatile Frame *activeFrame; */

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
            flashCount++;
            if(newpattern)
            {
                randPattern = 0;//random(0, numPatterns-1);
                activePattern = patterns.at(randPattern);
                period = activePattern->size() * cycle * 4;// number of frames
                newpattern = false;
                Serial.print(activePattern->at(0)->size());
                Serial.print("\n");
            }
            if(pause == 0)
            {
                color = strip.Color(0,0,0);
                setAllPixels(color);
                frame = flashCount/4;
                if( frame < 0 || frame > activePattern->size())
                {
                    frame = 0;
                }
                for(int i = 0; i < activePattern->at(frame)->size(); i++)
                {
                    brightness =  activePattern->at(frame)->at(i)->b * maxbrightness;
                    Serial.print(brightness);
                    Serial.print("\n");
                    color = strip.Color(brightness, brightness, brightness);
                    strip.setPixelColor(ledmatrix[activePattern->at(frame)->at(i)->x][activePattern->at(frame)->at(i)->y], color);
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
    LedDef* led;
    Frame* frame;
    Pattern* pattern;

    Serial.print(freeMemory());
    Serial.print("\n");
    // pattern 0
    pattern = new Pattern();
    // pattern 0, frame 0
    frame = new Frame();
    led = new LedDef();
    led->x = 0;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 0;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    pattern->push_back(frame);
    // pattern 0, frame 1
    Serial.print(freeMemory());
    Serial.print("\n");
    frame = new Frame();
    led = new LedDef();
    led->x = 0;
    led->y = 0;
    led->b = 0.5;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 0;
    led->b = 0.5;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 0;
    led->y = 1;
    led->b = 0.5;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 1;
    led->b = 0.5;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    pattern->push_back(frame);
    Serial.print("s\n");
    // pattern 0, frame 2
    frame = new Frame();
    led = new LedDef();
    led->x = 0;
    led->y = 0;
    led->b = 0.1;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 0;
    led->b = 0.1;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 0;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    pattern->push_back(frame);
    Serial.print("s\n");
    // pattern 0, frame 3
    frame = new Frame();
    led = new LedDef();
    led->x = 0;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 0;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    pattern->push_back(frame);
    Serial.print("s\n");
    // pattern 0, frame 4
    frame = new Frame();
    led = new LedDef();
    led->x = 0;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 0;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    pattern->push_back(frame);
    Serial.print("s\n");
    // pattern 0, frame 5
    frame = new Frame();
    led = new LedDef();
    led->x = 0;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 0;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    pattern->push_back(frame);
    Serial.print("s\n");
    // pattern 0, frame 6
    frame = new Frame();
    led = new LedDef();
    led->x = 0;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 0;
    led->b = 0.2;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 0;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    pattern->push_back(frame);
    Serial.print("s\n");
    // pattern 0, frame 7
    frame = new Frame();
    led = new LedDef();
    led->x = 0;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 0;
    led->b = 0.1;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 0;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    pattern->push_back(frame);
    Serial.print("s\n");
    // pattern 0, frame 8
    frame = new Frame();
    led = new LedDef();
    led->x = 0;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 0;
    led->b = 0.4;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 0;
    led->b = 0.4;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 0;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 1;
    led->b = 0.1;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 1;
    led->b = 0.1;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    pattern->push_back(frame);
    Serial.print("s\n");
    // pattern 0, frame 9
    frame = new Frame();
    led = new LedDef();
    led->x = 0;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 0;
    led->b = 0.1;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 0;
    led->b = 0.6;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 0;
    led->b = 0.6;
    frame->push_back(led);
    led = new LedDef();
    led->x = 0;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 1;
    led->b = 0.1;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 1;
    led->b = 0.1;
    frame->push_back(led);
    pattern->push_back(frame);
    Serial.print("s\n");
    // pattern 0, frame 10
    frame = new Frame();
    led = new LedDef();
    led->x = 0;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 0;
    led->b = 0.1;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 0;
    led->b = 0.1;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 0;
    led->b = 0.2;
    frame->push_back(led);
    led = new LedDef();
    led->x = 0;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    pattern->push_back(frame);
    Serial.print("s\n");
    // pattern 0, frame 11
    frame = new Frame();
    led = new LedDef();
    led->x = 0;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 0;
    led->b = 0.4;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 0;
    led->b = 0.1;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 0;
    led->b = 0.1;
    frame->push_back(led);
    led = new LedDef();
    led->x = 0;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 1;
    led->b = 0.2;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 1;
    led->b = 0.1;
    frame->push_back(led);
    pattern->push_back(frame);
    Serial.print("s\n");
    // pattern 0, frame 12
    frame = new Frame();
    led = new LedDef();
    led->x = 0;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 0;
    led->b = 0.1;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 0;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    pattern->push_back(frame);
    Serial.print("s\n");
    // pattern 0, frame 13
    frame = new Frame();
    led = new LedDef();
    led->x = 0;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 0;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 0;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 1;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 2;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    led = new LedDef();
    led->x = 3;
    led->y = 1;
    led->b = 0.0;
    frame->push_back(led);
    pattern->push_back(frame);
    patterns.push_back(pattern);

    Serial.print("f\n");
}
