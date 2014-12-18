#include "Adafruit_NeoPixel.h"
#include "TimerOne.h"

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

//variables for decade switch
unsigned int last_dek_change = 0;
unsigned int letzte_dekade = 0;
bool neue_dekade = false;

//variables for led adressing
int ledmatrix[ROWS][COLUMNS] = {{61, 60, 59, 58, 57,  1,  2,  3,  4,  5,  6,  7},
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
}

/*****Flash*****/
void flash()
{
    cycles++;
}

/*****Loop*****/
void loop()
{
    switch(dekade())
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
unsigned int dekade()
{
    bool bit1 = !digitalRead(DEK2);
    bool bit2 = !digitalRead(DEK4);
    bool bit3 = !digitalRead(DEK8);
    bool bit4 = !digitalRead(DEK16);
    unsigned int dekade = (((((bit4 << 1) + bit3) << 1) + bit2) << 1) + bit1;
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
void setAllPixels(uint16_t color)
{
    for(uint16_t i=0;i<NUMPIXEL;i++)
    {
        strip.setPixelColor(i, color);
    }
}

/*****setPixelRow*****/
void setPixelRow(int row, uint16_t color)
{
    for(uint16_t i=0;i<COLUMNS;i++)
    {
        strip.setPixelColor(ledmatrix[row][i], color);
    }
}

/*****setPixelColumn*****/
void setPixelColumn(int column, uint16_t color)
{
    for(uint16_t i=0;i<ROWS;i++)
    {
        strip.setPixelColor(ledmatrix[i][column], color);
    }
}
