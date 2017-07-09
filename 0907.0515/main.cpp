#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "iGraphics.h"
#include <sys\timeb.h>

#define and &&
#define  or || 


// window size and current gameState(in String)
const double width = 900, height = 480;
char gameState[100];
int sound, level, story, subLevel = 1;	// if story <= level, show story. else gameplay

// read file
void read_file() {
	FILE *fp = fopen("bin/sav.txt", "r");
	if(fp == NULL) {
		printf("Error opening file while reading\n");
		sound = 1, level = 1, story = 1;
	}
	else {
		fscanf(fp, "%d %d %d", &sound, &level, &story);
		fclose(fp);
	}
}


// write file
void write_file() {
	FILE *fp = fopen("bin/sav.txt", "w");
	if(fp == NULL) {
		printf("Error opening file while writing\n");
	}
	else {
		fprintf(fp, "%d %d %d", sound, level, story);
		fclose(fp);
	}
}



// shows the loading line, the passed img is the filename of an image
// returns 1 if fully loaded, 0 otherwise
int loading(char *img) {
	const double loadingHeight = 2;
	const double loadingWidth = 8 * (width/10);
	const double stX = width / 10;
	const double stY = height / 20 - loadingHeight / 2;
	static double loadingPercent = 0;
	
	loadingPercent += 1;
	double loadedX = loadingWidth * (loadingPercent/100);
	
	iShowBMP(0, 0, img);
	iSetColor(127, 127, 127);
	iFilledRectangle(stX, stY, loadingWidth, loadingHeight);
	iSetColor(255, 0, 0);
	iFilledRectangle(stX, stY, loadedX, loadingHeight);
	
	if(loadingPercent >= 100) return 1;
	return 0;
}



// variables for the main-menu
struct var_main_menu {
	// sound for the gif
	int music;

	// the play sidebar properties
	int play_width, play_height;
	
	// the exit sidebar properties
	int exit_width, exit_height;
	
	var_main_menu() { 
		music = 0; 
		play_width = 90;
		play_height = 60;
		exit_width = 90;
		exit_height = 60;
	}
} var_main;

// generates the main-menu front
void main_menu() {
	static int cnt = 0, cur_frame = 0;
	const int lim = 50;	// set matched to the sound file("bin/main-menu/sound-front.wav")
	
	++cnt;
	if(cnt >= lim) {
		cnt = 0;
		cur_frame = 1 - cur_frame;
	}
	
	if(cur_frame == 0) iShowBMP(0, 0, "bin/main-menu/front-1-1.bmp");
	else iShowBMP(0, 0, "bin/main-menu/front-1-2.bmp");
	
	// the play sidebar is 90x60
	iShowBMP(width-var_main.play_width, height-var_main.play_height, "bin/main-menu/play.bmp");
	// the exit sidebar is 90x60
	iShowBMP(width-var_main.exit_width, 0, "bin/main-menu/exit.bmp");

	if(!var_main.music && sound) {
		PlaySound(TEXT("bin/main-menu/sound-front.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
		var_main.music = 1;
	}
}





struct var_play_menu {
	// back opt
	int back_stX, back_stY, back_width, back_height; 
	
	// resume game
	int resume_stX, resume_stY, resume_width, resume_height;
	
	// new game
	int new_stX, new_stY, new_width, new_height;
	
	// sound
	int sound_stX, sound_stY, sound_width, sound_height;
	
	// credits
	int credits_stX, credits_stY, credits_width, credits_height;
	
	// extra
	int extra_stX, extra_stY, extra_width, extra_height;
	
	var_play_menu() {
		// back
		back_stX = 10;
		back_stY = 480 - 45;
		back_width = 37;
		back_height = 35;
		
		// resume
		resume_stX = 350;
		resume_stY = 480 - 290;
		resume_width = 200;
		resume_height = 40;
		
		// new game
		new_stX = 317;
		new_stY = 480 - 335;
		new_width = 275;
		new_height = 40;
		
		// sound 
		sound_stX = 369;
		sound_stY = 480 - 389;
		if(sound) sound_width = 155;
		else sound_width = 165;
		sound_height = 40;
		
		// credits
		credits_stX = 389;
		credits_stY = 480 - 430;
		credits_width = 115;
		credits_height = 40;
		
		// extra
		extra_stX = 403;
		extra_stY = 480 - 472;
		extra_width = 91;
		extra_height = 37;
	}
} var_play;

void play_menu() {
	if(sound) iShowBMP(0, 0, "bin/play-menu/pic-s-on-1.bmp");
	else iShowBMP(0, 0, "bin/play-menu/pic-s-off-1.bmp");
}



struct var_exit_menu {
	// properties of the yes option box
	int yes_stX, yes_stY, yes_width, yes_height;
	
	// properties of the no option box
	int no_stX, no_stY, no_width, no_height;
	
	var_exit_menu() {
		yes_stX = 270;
		yes_stY = 130;
		no_stX = 480;
		no_stY = 130;
		yes_width = no_width = 150;
		yes_height = no_height = 150;
	}
} var_exit;

void exit_menu() {
	iShowBMP(0, 0, "bin/exit-menu/final.bmp");
}

void story_1()
{
	++story;
}

double distance(double ax, double ay, double bx, double by)
{
	return sqrt(pow(ax-bx, 2) + pow(ay-by, 2));
}	

clock_t startingTime;
struct varLevel{
	// level properties
	int flyNow, birdsRemaining, targetsRemaining, lastHit;
	double theta, velocity;
	bool flag_1;

	// bird properties
	double redPosX, redPosY;
	double chuckPosX, chuckPosY;
	double bombPosX, bombPosY;

	// constructor
	varLevel()
	{
		lastHit = 4;
		flag_1 = false;
		birdsRemaining = 3;
		targetsRemaining = 2;

		redPosX = 20;
		redPosY = 400;

		chuckPosX = 100;
		chuckPosY = 400;

		bombPosX = 180;
		bombPosY = 400;
	}
	
	// this re-initiate the variable
	void restart()
	{
		lastHit = 4;
		flag_1 = false;
		birdsRemaining = 3;
		targetsRemaining = 2;

		redPosX = 20;
		redPosY = 400;

		chuckPosX = 100;
		chuckPosY = 400;

		bombPosX = 180;
		bombPosY = 400;
	}
} lev1, lev2;

void level_1()
{
	printf("level 1");
	if (lev1.targetsRemaining == 0)
		subLevel = 2;
	if (lev1.birdsRemaining == 0)
		subLevel = 3;
	
	
	if (subLevel == 1)
	{
		if (lev1.flag_1) // any of the bird is flying now.
        {
			// the path function needs to be modified for a better view.
            if (lev1.flyNow == 1) // red is flying
            {
                double time = double(clock() - startingTime)/CLOCKS_PER_SEC/10;
                lev1.redPosX += lev1.velocity * cos(lev1.theta) * time;
                lev1.redPosY += lev1.velocity * sin(lev1.theta) * time - 25 * time * time;
				
				if (distance(lev1.redPosX, lev1.redPosY, 720, 178) <= 165 and lev1.lastHit != 1)
				{
					--lev1.targetsRemaining;
					lev1.lastHit = 1;
				}
				
                if (lev1.redPosX < -64 or lev1.redPosX > width or lev1.redPosY < -64 or lev1.redPosY > height)
				{
					--lev1.birdsRemaining;
					lev1.flag_1 = false;
				}
            }
            else if (lev1.flyNow == 2) // chuck is flying
            {
                double time = double(clock() - startingTime)/CLOCKS_PER_SEC/10;
                lev1.chuckPosX += lev1.velocity * cos(lev1.theta) * time;
                lev1.chuckPosY += lev1.velocity * sin(lev1.theta) * time - 25 * time * time;
				
				if (distance(lev1.chuckPosX, lev1.chuckPosY, 720, 178) <= 165 and lev1.lastHit != 2)
				{
					--lev1.targetsRemaining;
					lev1.lastHit = 2;
				}
				
                if (lev1.chuckPosX < -64 or lev1.chuckPosX > width or lev1.chuckPosY < -64 or lev1.chuckPosY > height)
                {
					--lev1.birdsRemaining;
					lev1.flag_1 = false;
				}   
            }
            else if (lev1.flyNow == 3) // bomb is flying
            {
                double time = double(clock() - startingTime)/CLOCKS_PER_SEC/10;
                lev1.bombPosX += lev1.velocity * cos(lev1.theta) * time;
                lev1.bombPosY += lev1.velocity * sin(lev1.theta) * time - 25 * time * time;

				if (distance(lev1.bombPosX, lev1.bombPosY, 720, 178) <= 165 and lev1.lastHit != 3)
				{
					--lev1.targetsRemaining;
					lev1.lastHit = 3;
				}
				
                if (lev1.bombPosX < -64 or lev1.bombPosX > width or lev1.bombPosY < -64 or lev1.bombPosY > height)
                {
					--lev1.birdsRemaining;
					lev1.flag_1 = false;
				}
            }
        }
        printf("%lf %lf\n", lev1.chuckPosX, lev1.chuckPosY);
    }
	
	// now loading the images according to their corresponding co-ordinates
    iShowBMP(0, 0, "bin/level-1/background.bmp");
    iShowBMP2(lev1.redPosX, lev1.redPosY, "bin/level-1/red.bmp", 0xFFFFFF);
    iShowBMP2(lev1.bombPosX, lev1.bombPosY, "bin/level-1/bomb.bmp", 0);
    iShowBMP2(lev1.chuckPosX, lev1.chuckPosY, "bin/level-1/chuck.bmp", 0xFFFFFF);

	if (subLevel == 1)
		iShowBMP2(600, 50, "bin/level-1/target.bmp", 0xFFFFFF);
	// the level has been completed. User will said to save and go to next level
	else if (subLevel == 2)
	{
		iText(200, 300, "congrats! level completed!");
		iFilledRectangle(300, 200, 100, 50);
		iText(325, 225, "Save progress and move forward!");
	}
	// the level has been failed. user will promped to restart. CAUTIONS: needs re-initiating the lev1
	else if (subLevel == 3)
	{
		iText(200, 200, "Level failed. Restart?");
		iFilledRectangle(300, 200, 100, 50);
		iText(325, 225, "Restart Level?");
	}
}

void story_2()
{
	
}

void level_2()
{
	
}

void story_3()
{
	
}


void level_3()
{
	
}

void story_4()
{
	
}

void level_4() 
{
	
}

void story_5()
{
	
}


void gameplay() {
	
	if(story <= level) {
		if(story == 1) {
			story++;
			//story_1();
		}
		else if(story == 2) {
			story_2();
		}
		else if(story == 3) {
			story_3();
		}
		else if(story == 4) {
			story_4();
		}
		else if(story == 5) {
			story_5();
		}
	}
	else {
		if(level == 1) {
			level_1();
		}
		else if(level == 2) {
			level_2();
		}
		else if(level == 3) {
			level_3();
		}
		else if(level == 4) {
			level_4();
		}
	}

	//level_1();
}



void credits() {
	iSetColor(0, 255, 0);
	iText(0, 240, "Credits");
	iText(0, 220, "-------------------");
	iText(0, 200, "1605109\tRafid Bin Mostofa");
	iText(0, 180, "1605113\tZunaed Karim Sifat");
	iText(0, 160, "1605121\tBasit Ahmed Mir");
}

void extra() {
	iSetColor(0, 0, 255);
	iText(0, 240, "Extra: Not Porn -__-");
}




/*
function iDraw() is called again and again by the system.
*/
void iDraw()
{
	//place your drawing codes here
	iClear();
	
	if(!strcmp(gameState, "loading")) {
		if(loading("bin/first-screen.bmp") == 1) {
			strcpy(gameState, "main-menu");
		}
	}
	else if(!strcmp(gameState, "main-menu")) {
		main_menu();
	}
	else if(!strcmp(gameState, "play-menu")) {
		play_menu();
	}
	else if(!strcmp(gameState, "exit-menu")) {
		exit_menu();
	}
	else if(!strcmp(gameState, "gameplay")) {
		gameplay();
	}
	else if(!strcmp(gameState, "credits")) {
		credits();
	}
	else if(!strcmp(gameState, "extra")) {
		extra();
	}
}

/*
function iMouseMove() is called when the user presses and drags the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseMove(int mx, int my)
{
	printf("Dragged to %d %d\n", mx, my);
	//place your codes here
}

/*
function iMouse() is called when the user presses/releases the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouse(int button, int state, int mx, int my)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		printf("Left mouse at %d %d\n", mx, my);
		if(!strcmp(gameState, "main-menu")) {
			if(mx >= width - var_main.play_width && mx <= width && my >= height-var_main.play_height && my <= height) {
				// play button is clicked
				strcpy(gameState, "play-menu");
				
				// stop the sound
				PlaySound(NULL, NULL, SND_NODEFAULT);
				var_main.music = 0;
			}
			else if(mx >= width - var_main.exit_width && mx <= width && my >= 0 && my <= height - var_main.exit_height) {
				// exit button is clicked
				strcpy(gameState, "exit-menu");
				
				// stop the sound
				PlaySound(NULL, NULL, SND_NODEFAULT);
				var_main.music = 0;
			}
		}
		else if(!strcmp(gameState, "exit-menu")) {
			if(mx >= var_exit.yes_stX && mx <= var_exit.yes_stX + var_exit.yes_width && my >= var_exit.yes_stY && my <= var_exit.yes_stY + var_exit.yes_height) {
				// write the file
				write_file();
				
				// quit the game
				exit(0);
			}
			else if(mx >= var_exit.no_stX && mx <= var_exit.no_stX + var_exit.no_width && my >= var_exit.no_stY && my <= var_exit.no_stY + var_exit.no_height) {
				// back to main menu
				strcpy(gameState, "main-menu");
			}
		}
		else if(!strcmp(gameState, "play-menu")) {
			if(mx >= var_play.back_stX && mx <= var_play.back_stX + var_play.back_width && my >= var_play.back_stY && my <= var_play.back_stY + var_play.back_height) {
				// back to main menu
				strcpy(gameState, "main-menu");
			}
			else if(mx >= var_play.resume_stX && mx <= var_play.resume_stX + var_play.resume_width && my >= var_play.resume_stY && my <= var_play.resume_stY + var_play.resume_height) {
				// go to gameplay
				strcpy(gameState, "gameplay");
			}
			else if(mx >= var_play.new_stX && mx <= var_play.new_stX + var_play.new_width && my >= var_play.new_stY && my <= var_play.new_stY + var_play.new_height) {
				// go to new game
				strcpy(gameState, "gameplay");
				level = 1;
				story = 1;
			}
			else if(mx >= var_play.sound_stX && mx <= var_play.sound_stX + var_play.sound_width && my >= var_play.sound_stY && my <= var_play.sound_stY + var_play.sound_height) {
				// sound off
				sound = 1 - sound;
			}
			else if(mx >= var_play.credits_stX && mx <= var_play.credits_stX + var_play.credits_width && my >= var_play.credits_stY && my <= var_play.credits_stY + var_play.credits_height) {
				// go to credits
				strcpy(gameState, "credits");
			}
			else if(mx >= var_play.extra_stX && mx <= var_play.extra_stX + var_play.extra_width && my >= var_play.extra_stY && my <= var_play.extra_stY + var_play.extra_height) {
				// go to extra
				strcpy(gameState, "extra");
			}
		}
		if (!strcmp(gameState, "gameplay"))
		{
			if (level == 1)
			{
				if (subLevel == 1)
				{
					if (mx >= 20 and mx <= 84 and my >= 400 and my <= 464 and lev1.redPosX == 20 and lev1.redPosY == 400)
					{
						lev1.redPosX = 100;
						lev1.redPosY = 100;
					}
					else if (mx >= 50 and mx <= 114 and my >= 50 and my <= 114 and lev1.redPosX == 100 and lev1.redPosY == 100)
					{
						lev1.flag_1 = true;
						lev1.flyNow = 1;
						lev1.theta = atan(double(100-my) / (100-mx));
						lev1.velocity = sqrt(pow(100-my, 2) + pow(100-mx, 2));
						startingTime = clock();
					}


					if (mx >= 100 and mx <= 164 and my >= 400 and my <= 464 and lev1.chuckPosX == 100 and lev1.chuckPosY == 400)
					{
						lev1.chuckPosX = 100;
						lev1.chuckPosY = 100;
					}
					else if (mx >= 50 and mx <= 114 and my >= 50 and my <= 114 and lev1.chuckPosX == 100 and lev1.chuckPosY == 100)
					{
						lev1.flag_1 = true;
						lev1.flyNow = 2;
						lev1.theta = atan(double(100-my) / (100-mx));
						lev1.velocity = sqrt(pow(100-my, 2) + pow(100-mx, 2));
						startingTime = clock();
					}

					if (mx >= 180 and mx <= 244 and my >= 400 and my <= 464 and lev1.bombPosX == 180 and lev1.bombPosY == 400)
					{
						lev1.bombPosX = 100;
						lev1.bombPosY = 100;
					}
					else if (mx >= 50 and mx <= 114 and my >= 50 and my <= 114 and lev1.bombPosX == 100 and lev1.bombPosY == 100)
					{
						lev1.flag_1 = true;
						lev1.flyNow = 3;
						lev1.theta = atan(double(100-my) / (100-mx));
						lev1.velocity = sqrt(pow(100-my, 2) + pow(100-mx, 2));
						startingTime = clock();
					}
				}
				else if (subLevel == 2)
				{
					if (mx >= 300 and mx <= 400 and my >= 200 and my <= 250)
					{
						level = 2;
						story = 2;
						subLevel = 1;
					}
				}
				else if (subLevel == 3)
				{
					if (mx >= 300 and mx <= 400 and my >= 200 and my <= 250)
					{
						level = 1;
						story = 2;
						subLevel = 1;
						lev1.restart();
					}
				}
			}

		}
	}
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		//place your codes here
	}
}

/*
function iKeyboard() is called whenever the user hits a key in keyboard.
key- holds the ASCII value of the key pressed.
*/
void iKeyboard(unsigned char key)
{
	if(!strcmp(gameState, "play-menu")) {
		if(key == 'q') strcpy(gameState, "main-menu");
	}
	else if(!strcmp(gameState, "exit-menu")) {
		if(key == 'q') strcpy(gameState, "main-menu");
	}
	else if(!strcmp(gameState, "gameplay")) {
		if(key == 'q') strcpy(gameState, "play-menu");
	}
	else if(!strcmp(gameState, "credits")) {
		if(key == 'q') strcpy(gameState, "play-menu");
	}
	else if(!strcmp(gameState, "extra")) {
		if(key == 'q') strcpy(gameState, "play-menu");
	}
}

/*
function iSpecialKeyboard() is called whenver user hits special keys likefunction keys, home, end, pg up, pg down, arraows etc. you have to use
appropriate constants to detect them. A list is:
GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11,
GLUT_KEY_F12, GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN,
GLUT_KEY_PAGE UP, GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END,
GLUT_KEY_INSERT */
void iSpecialKeyboard(unsigned char key)
{
	if(key == GLUT_KEY_END)
	{
		exit(0);
	}
	//place your codes for other keys here
}

int main()
{
	/*
		// for elapsed time
		// https://stackoverflow.com/questions/17250932/how-to-get-the-time-elapsed-in-c-in-milliseconds-windows
		struct timeb start, end;
		double diff;
		int i = 0;
		ftime(&start);

		while(++i < 1e8) ;
		ftime(&end);
		diff = (double) (end.time - start.time)
			+ (double) (end.millitm - start.millitm) / 1000;

		printf("\nOperation took %lf seconds\n", diff);
	*/
	
	read_file();
	strcpy(gameState, "loading");
	iInitialize(width, height, "angry -___-");
	return 0;
}