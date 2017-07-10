#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "iGraphics.h"
#include <sys\timeb.h> 


// window size and current gameState(in String)
const double width = 900, height = 480;
char gameState[100];
int sound, level, story;	// if story <= level, show story. else gameplay

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
		back_stX = 0;
		back_stY = 480 - 50;
		back_width = 70;
		back_height = 50;
		
		// resume
		resume_stX = 632;
		resume_stY = 480 - 58;
		resume_width = 268;
		resume_height = 50;
		
		// new game
		new_stX = 589;
		new_stY = 480 - 114;
		new_width = 311;
		new_height = 50;
		
		// sound 
		sound_stX = 768;
		sound_stY = 480 - 360;
		sound_width = 130;
		sound_height = 48;
		
		// credits
		credits_stX = 750;
		credits_stY = 480 - 415;
		credits_width = 150;
		credits_height = 45;
		
		// extra
		extra_stX = 758;
		extra_stY = 480 - 468;
		extra_width = 142;
		extra_height = 44;
	}
} var_play;

void play_menu() {
	if(sound) iShowBMP(0, 0, "bin/play-menu/new-back-s-on.bmp");
	else iShowBMP(0, 0, "bin/play-menu/new-back-s-off.bmp");
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



bool level_passed = false;

struct var_level_passed {
	// play-menu button
	double play_x, play_y, play_r;
	
	// next level 
	double next_x, next_y, next_r;
	
	var_level_passed() {
		play_x = 338;
		play_y = 52;
		play_r = 42;
		
		next_x = 557;
		next_y = 52;
		next_r = 42;
	}
} var_passed;

void passed() {
	iShowBMP(450 - 360 / 2, 0, "bin/level-passed.bmp");
}


bool level_failed = false;

struct var_level_failed {
	// play-menu button
	double play_x, play_y, play_r;
	
	// restart button 
	double restart_x, restart_y, restart_r;
	
	var_level_failed() {
		play_x = 338;
		play_y = 52;
		play_r = 42;
		
		restart_x = 557;
		restart_y = 52;
		restart_r = 42;
	}
} var_failed;

void failed() {
	iShowBMP(450 - 360 / 2, 0, "bin/level-failed.bmp");
}

void story_1()
{
	
}

void level_1()
{
	
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


struct var_lev_03 {
	bool called_right_now;
	double bird_h;
	bool crashed;
	clock_t st, en;
	int cnt, loading;
	double px, py;
	int pillers;
	
	var_lev_03() {
		called_right_now = true;
		bird_h = 200;
		crashed = false;
		cnt = 0;
		loading = 1;
		px = 900;
		py = 200;
		pillers = 0;
	}
} lev3;

void level_3()
{
	// checking the time elapsed;
	if(lev3.called_right_now) {
		lev3.en = clock();
		lev3.called_right_now = false;
	}
	
	lev3.st = lev3.en;
	lev3.en = clock();
	double elapsed = (double) (lev3.en - lev3.st) / CLOCKS_PER_SEC;
	const double g = 9.8 * 7;
	
	iShowBMP(0, 0, "bin/lev-3/screen.bmp");

	++lev3.cnt;
	if(lev3.cnt > 5) {
		lev3.cnt = 0;
		lev3.loading = 1- lev3.loading;
	}
	
	if(lev3.loading == 0) iShowBMP(0, 0, "bin/lev-3/loading-1.bmp");
	else iShowBMP(0, 0, "bin/lev-3/loading-2.bmp");

	iShowBMP2(lev3.px, lev3.py, "bin/lev-3/pillar-down.bmp", 0xFFFFFF);

	const double bird_x = 400;
	const double bird_width = 100;
	const double bird_height = 45;
	const double pillar_width = 107;
	iShowBMP2(bird_x, lev3.bird_h, "bin/lev-3/eagle.bmp", 0xFFFFFF);

	
	if(!lev3.crashed && lev3.bird_h <= 50) {
		lev3.crashed = true;
		printf("lev3.crashed due to lower\n");
		printf("bird: %lf %lf\n", bird_x, lev3.bird_h);
	}
	if(!lev3.crashed && bird_x + bird_width >= lev3.px && bird_x <= lev3.px + pillar_width) {
		if(lev3.bird_h + bird_height >= lev3.py) { 
			lev3.crashed = true;
			printf("lev3.crashed due to pipe\n");
		}
	}

	if(!lev3.crashed) lev3.px -= 40;
	if(lev3.px <= -5) {
		lev3.px = 900;
		lev3.py = (double) (rand() % 480) + 150;
		if(lev3.py >= 450) lev3.py = 400;
		lev3.pillers += 1;
	}

	if(!lev3.crashed) lev3.bird_h -= elapsed * g;

	if(lev3.crashed) {
		level_failed = true;
		failed();
	}
	
	if(lev3.pillers >= 5) {
		level_passed = true;
		++level;
		passed();
	}
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
	iSetColor(255, 0, 0);
	iText(0, 240, "This is the gameplay archade\n");
	
	if(story <= level) {
		if(story == 1) {
			story_1();
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
		else if(!strcmp(gameState, "gameplay")) {
			printf("gameplay mouse option\n");
			if(level_failed) {
				printf("level_failed\n");
				printf("%lf %lf %lf ---- play menu\n", var_failed.play_x, var_failed.play_y, var_failed.play_r);
				if((mx-var_failed.play_x)*(mx-var_failed.play_x) + (my-var_failed.play_y)*(my-var_failed.play_y) - var_failed.play_r * var_failed.play_r <= 0) {
					// back to play menu
					printf("Inside left butt :D\n");
					strcpy(gameState, "play-menu");
					
					if(level == 3) {
						lev3.crashed = false;
						lev3.en = clock();
						lev3.px = 900, lev3.py = 200;
						lev3.bird_h = 200;
						level_failed = false;
						lev3.called_right_now = true;
						lev3.pillers = 0;
					}
				}
				else if((mx-var_failed.restart_x)*(mx-var_failed.restart_x) + (my-var_failed.restart_y)*(my-var_failed.restart_y) - var_failed.restart_r * var_failed.restart_r <= 0) {
					// restart level 3
					printf("Restart enters!!\n");
					if(level == 3) {
						lev3.crashed = false;
						lev3.en = clock();
						lev3.px = 900, lev3.py = 200;
						lev3.bird_h = 200;
						level_failed = false;
						lev3.called_right_now = true;
						lev3.pillers = 0;
					}
				}
			}
			
			if(level_passed) {
				if((mx-var_passed.play_x)*(mx-var_passed.play_x) + (my-var_passed.play_y)*(my-var_passed.play_y) - var_passed.play_r * var_passed.play_r <= 0) {
					// back to play menu
					printf("Inside left butt :D\n");
					strcpy(gameState, "play-menu");
					
					if(level == 3) {
						lev3.px = 900, lev3.py = 200;
						lev3.bird_h = 200;
						level_passed = false;
						lev3.called_right_now = true;
						lev3.pillers = 0;
					}
				}
				else if((mx-var_passed.next_x)*(mx-var_passed.next_x) + (my-var_passed.next_y)*(my-var_passed.next_y) - var_passed.next_r * var_passed.next_r <= 0) {
					// next level 3
					printf("next enters!!\n");
					if(level == 3) {
						lev3.px = 900, lev3.py = 200;
						lev3.bird_h = 200;
						level_passed = false;
						lev3.called_right_now = true;
						lev3.pillers = 0;
					}
				}
			}
			
			int mini = level < story ? level : story;
			if(mini == level) {
				if(level == 3) {
					if(!lev3.crashed) {
						lev3.bird_h += 25;
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
	srand(time(NULL));
	read_file();
	strcpy(gameState, "loading");
	iInitialize(width, height, "angry -___-");
	return 0;
}