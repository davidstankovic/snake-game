// define constants based on the direction in which snake is moving
#define UP 0
#define DOWN 1
#define RIGHT 2
#define LEFT 3

char GLCD_DataPort at PORTD; //defining data port - configuring GLCD
sbit GLCD_CS1 at LATB0_bit;  //Column Select 1
sbit GLCD_CS2 at LATB1_bit;  // Column Select 2
sbit GLCD_RS at LATB2_bit;	 // Reset
sbit GLCD_RW at LATB3_bit;	 // Read/Write
sbit GLCD_EN at LATB4_bit;	 // Enable
sbit GLCD_RST at LATB5_bit;	 // Register Selelect on port B

sbit GLCD_CS1_Direction at TRISB0_bit;
sbit GLCD_CS2_Direction at TRISB1_bit;
sbit GLCD_RS_Direction at TRISB2_bit;
sbit GLCD_RW_Direction at TRISB3_bit;
sbit GLCD_EN_Direction at TRISB4_bit;
sbit GLCD_RST_Direction at TRISB5_bit;

int snakeX[20]; int snakeY[20]; // arrays that hold x and y coordinates of the snake

// detecting descending edge
bit oldstate4;
bit oldstate5;
bit oldstate6;
bit oldstate7
bit flag4; // coresponding with button RC4
bit flag5; // coresponding with button RC5
bit flag6; // coresponding with button RC6
bit flag7; // coresponding with button RC7

int flag; // coresponding with Timer break, flag indicatest that break happened
int direction; // integer which holds the state of the snake (e.g. UP)
int ii; //counter i
int k; // counter k
int jj; // counter jj
int xx; // variable for checking snakes position x
int yy;// variable for checking snakes position y

void interrupt(){ // Interrupt Service Routine which services interruption from the Timer. 
				  // When this Timer clocks this interrupt, snake moves by 1px.
				  // Timer is set to end every 50ms
	if(TMR0IF_bit){
		TMR0IF_bit = 0;
		TMR0H = 0x3C
		TMR0L = 0xB0;
		flag=1
	}
}
// colision detection - with obstacles
int obstacle_colision(){
	 int side_x_small[11] = {10,11,12,13,14,15,16,17,18,19,20}; // obstacle side x - smaller obstacle
	 int side_y_small[11] = {10,11,12,13,14,15,16,17,18,19,20}; // obstacle side y - smaller obstacle
     int side_x_big[21] = {54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74}; // obstacle side x - bigger obstacle
     int side_y_big[21] = {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42}; // obstacle side y - bigger obstacle
	 // colision detection - smaller obstacle
	 for(xx=0; xx<11; xx++){
		 for(yy=0; yy<11; yy++){
			if(snakeX[0] == side_x_small[xx] && snakeY[0] == side_y_small[yy]){
				return 1;
			}
		 }
	 }
	 // colision detection - bigger obstacle
	 for(xx=0; xx<21; xx++){
		 for(yy=0; yy<21; yy++){
			 if(snakeX[0] == side_x_big[xx] && snakeY[0] == side_y_big[yy]){
				 return 1;
			 }
		 }
	 }
	 return 0;
}
// draws obstacles
void draw_obstacles(){
	Glcd_Box(10,10,20,20,1);
    Glcd_Box(54,22,74,42,1);
}
// reset snake state - setting the snake on start position.
void clear_screen() {
	Glcd_Fill(0); // erases everything from the screen
	draw_obstacles(); // function call to draw obstacles
	// erases snake, pixel by pixel, starting from the tail up to the head
	for (ii=19; ii>0; ii--){
		snakeX[ii]=0;
		snakeY[ii]=0;
	}
	// places snake in start position and makes her starting direction - DOWN
	snakeX[0]=0;
	snakeY[0]=0;
	direction=DOWN;
}
// pauses snake in position after RC5 button is being pressed.
void pause(){
	if(flag5==1) flag5=0; // pause every odd time when RC5 is pressed
	do { // detects unpause call - RC5 pressed second time
		for(jj=0; jj<20; jj++){
			Glcd_Dot(snakeX[jj], snakeY[jj], 1); // continously draws snake on same position
		}
		if (Button(&PORTC, 5, 1, 1)) oldstate5=1; //checks if the button is pressed (pulldown)
		if(oldstate5 && Button(&PORTC, 5, 1, 0)){ // checks if the button is released (pullup)
			oldstate5=0;
			flag5=1;
		}
	} while(flag5==0); // until RC5 is pressed second time, flag5 keeps being 0.
}
// writes text on the screen
void write_text(){
	Glcd_Write_Text("You lose!", 80,3,2);
	Glcd_Write_Text("Press RC4 to play again.", 80,4,1);
}

void main() {
	// digital I/O - active.
	ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;
	// port C is in input regime
	TRISC=0xFF;
	// starting configuration of Timer 0, this is called after first start of the game.
	T0CON = 0x80;
    TMR0H = 0x3C;
    TMR0L = 0xB0;
    TMR0IE_bit = 1;
    GIE_bit = 1;  //enable interrupts

    Glcd_Init();  //initialize GLCD module
	Glcd_Set_Font(Font_Glcd_Character8x7, 8, 7, 32); // set font for the displaying text
	
	clear_screen();
	// flag initialization
	flag4=0;
	flag5=0;
	flag6=0;
	flag7=0;
	
	do {
		if(Button(&PORTC, 7, 1, 1)) oldstate7=1; // checks if RC7 is pressed
		if(oldstate7 && Button(&PORTC, 7, 1, 0)) {
			flag7=1;
			oldstate7=0;
		}
		if(Button(&PORTC, 6, 1, 1)) oldstate6=1; // checks if RC6 is pressed
		if(oldstate6 && Button(&PORTC, 6, 1, 0)) {
			flag6=1;
			oldstate6=0;
		}
		if(Button(&PORTC, 5, 1, 1)) oldstate5=1; // checks if RC5 is pressed for the pause call
		if(oldstate5 && Button(&PORTC, 5, 1, 0)){
			oldstate5=0;
			pause();
		}
		if(Button(&PORTC, 4, 1, 1)) oldstate4=1; // checks if RC4 is pressed - start again call
		if(oldstate4 && Button(&PORTC, 4, 1, 0)){
			flag4=1;
			oldstate4=0;
		}
		if(flag){ // checks for breakes from Timer in interrupt.
			Glcd_Dot(snakeX[19], snakeY[19], 0); // Removes piece of tale to simulate moving of the snake.
			for(ii=19; ii>0; ii--){
				// checking for two types of colision: obstacle colision and if the snake bit herself
				
				// Colision with an obstacle
				if(obstacle_colision()){
					Glcd_Fill(0); // erases whole screen
					write_text(); // Writes text - game over.
					do { // in continous loop, program waits for user to press RC4 to start again.
						if(Button(&PORTC, 4, 1, 1)) oldstate4=1;
						if(oldstate4 && Button(&PORTC, 4, 1, 0)){
							flag4=1;
							oldstate4=0;
						}
					} while(flag4!=1); // program waits for flag4 to become 1 - to start again.
				}
				
				// Snake bit herself colision
				if(snakeX[0] != snakeX[19]){
					if(snakeX[0]==snakeX[ii] && snakeY[0]==snakeY[ii]){ // checks for matching coordinates. If some coordinates match - colision happened.
						Glcd_Fill(0);
						write_text();
						do{ // in continous loop, program waits for user to press RC4 to start again.
							if(Button(&PORTC, 4, 1, 1)) oldstate4=1;
							if(oldstate4 && Button(&PORTC, 4, 1, 0)){
								flag4=1;
								oldstate4=0;
							}
						}while(flag4!=1); // programm waits for flag4 to become 1 - to start again.
					}
				}
				// if no colision happened, previous coordinate is being set for the current - current coordinate is being set for the next.
				snakeX[ii]=snakeX[ii-1];
				snakeY[ii]=snakeY[ii-1];
			}
			switch(direction){ // checking in which direction snake is moving.
				case DOWN:
					snakeY[0]++;
					if(snakeY[0]>63) snakeY[0]=0; // snake can go through wall - GLCDs height is 64 (0-63)
					if(flag6){ // checks if RC6 is being pressed, snake goes LEFT if so.
						direction=LEFT;
						flag6=0;
					}
					if(flag7){ // checks if RC7 is being pressed, snake goes RIGHT if so.
						direction=RIGHT;
						flag7=0;
					}
					if(flag4){
						flag4=0;
						clear_screen();
					}
				break;
				case UP:
					snakeY[0]--;
					if(snakeY[0]<0) snakeY[0]=63; // snake can go through wall - GLCDs height is 64 (0-63)
					if(flag6){ // checks if RC6 is being pressed, snake goes LEFT if so.
						direction=LEFT;
						flag6=0;
					}
					if(flag7){ // checks if RC7 is being pressed, snake goes RIGHT if so.
						direction=RIGHT; 
						flag7=0;
					}
					if(flag4){
						flag4=0;
						clear_screen();
					}
				break;
				case LEFT:
					snakeX[0]--;
					if(snakeX[0]>0) snakeX[0]=127; // snake can go through wall - GLCDs width is 128 (0-127)
					if(flag6){ // checks if RC6 is being pressed, snake goes DOWN if so.
						direction=DOWN;
						flag6=0;
					}
					if(flag7){ // checks if RC7 is being pressed, snake goes UP if so.
						direction=UP;
						flag7=0;
					}
					if(flag4){
						flag4=0;
						clear_screen();
					}
				break;
				case RIGHT:
					snakeX[0]++;
					if(snakeX[0]>127) snakeX[0]=0; // snake can go through wall - GLCDs width is 128 (0-127)
					if(flag6){ // checks if RC6 is being pressed, snake goes UP if so
						direction=UP;
						flag6=0;
					}
					if(flag7){ // checks if RC7 is being pressed, snake goes DOWN if so
						direction=DOWN;
						flag7=0;
					}
					if(flag4){
						flag4=0;
						clear_screen();
					}
				break;
			}
			flag=0; // resets flag for Timer 0.
			for(kk=0; kk<20; kk++){
				Glcd_Dot(snakeX[kk], snakeY[kk], 1); // after all validations and checks, snake is being drawn.
			}
		}
	} while(1);
}
