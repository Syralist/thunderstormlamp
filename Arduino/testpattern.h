#define FRAMES 6
#define LEDS 6
#define R 0
#define G 1
#define B 2
uint8_t PATTERN[FRAMES][LEDS][3] = { { {127,0,0},{0,127,0},{0,0,127},{127,127,0},{127,0,127},{0,127,127} },
				     { {0,127,127},{127,0,0},{0,127,0},{0,0,127},{127,127,0},{127,0,127} },
  			             { {127,0,127},{0,127,127},{127,0,0},{0,127,0},{0,0,127},{127,127,0} },
				     { {127,127,0},{127,0,127},{0,127,127},{127,0,0},{0,127,0},{0,0,127} },
				     { {0,0,127},{127,127,0},{127,0,127},{0,127,127},{127,0,0},{0,127,0} },
				     { {0,127,0},{0,0,127},{127,127,0},{127,0,127},{0,127,127},{127,0,0} } };
