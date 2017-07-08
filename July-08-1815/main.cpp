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

int current_user = -1;
struct user {
	char name[100];
	int sound, level, story;
	
	user() { sound = 1, level = 1, story = 1, strcpy(name, "rough"); }
} usr[5];


// read file
void read_file() {
	FILE *fp = fopen("bin/sav.txt", "r");
	if(fp == NULL) {
		printf("Error opening file while reading\n");
	}
	else {
		int i = 0;
		while(fgets(usr[i].name, 100, fp) != NULL) {
			fscanf(fp, "%d %d %d", &usr[i].sound, &usr[i].level, &usr[i].sound);
			++i;
		}
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
		int i = 0;
		for(i=0; i<5; ++i) {
			fprintf(fp, "%s\n%d %d %d\n", usr[i].name, usr[i].sound, usr[i].level, usr[i].story);
		}
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
	const int lim = 40;	// set matched to the sound file("bin/main-menu/sound-front.wav")
	
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


struct var_login_window {
	int box_width ;
	int box_height;
	int box_stX;
	int box_stY;
	
	char username[100];
	char temp[100];
	
	int inputted;
	
	var_login_window() {
		box_width = 200;
		box_height = 25;
		box_stX = width / 2 - box_width / 2;
		box_stY = height / 2 - box_height / 2;
		
		username[0] = 0;
		temp[0] = 0;
		inputted = 0;
	}
} var_login;

void login() {
	printf("login necessary\n");
	if(var_login.inputted == 0) {
		iSetColor(255, 127, 64);
		iRectangle(var_login.box_stX, var_login.box_stY, var_login.box_width, var_login.box_height);
		iText(var_login.box_stX, var_login.box_stY + var_login.box_height * 2, "Enter username:");
		iText(var_login.box_stX, var_login.box_stY + 5, var_login.temp);
	}
	else {
		int i;
		for(i=0; i<5; ++i) {
			if(!strcmp(usr[i].name, var_login.username)) {
				printf("Proof: Went through this!\n");
				current_user = i;
				level = usr[i].level;
				sound = usr[i].sound;
				story = usr[i].story;
				strcpy(gameState, "play-menu");
				break;
			}
		}
		
		if(current_user == -1) {
			var_login.inputted = 0;
			iSetColor(255, 0, 0);
			iText(140, 140, "username not found\n");
		}
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
	
	var_play_menu() {
		// back
		back_stX = 0;
		back_stY = 480-30;
		back_width = 65;
		back_height = 30;
		
		// resume
		resume_stX = 450 - 224/2;
		resume_stY = 480 - 170;
		resume_width = 224;
		resume_height = 40;
		
		// new game
		new_stX = 450 - 300/2;
		new_stY = 480 - 215;
		new_width = 300;
		new_height = 40;
		
		// sound 
		sound_stX = 450 - 156/2;
		sound_stY = 480 - 286;
		if(sound) sound_width = 150;
		else sound_width = 160;
		sound_height = 36;
		
		// credits
		credits_stX = 450 - 110/2;
		credits_stY = 480 - 326;
		credits_width = 110;
		credits_height = 36;
	}
} var_play;

void play_menu() {
	if(sound) iShowBMP(0, 0, "bin/play-menu/pic-s-on.bmp");
	else iShowBMP(0, 0, "bin/play-menu/pic-s-off.bmp");
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
	iText(0, 180, "1605112\tZunaed Karim Sifat");
	iText(0, 160, "1605121\tBasit Ahmed Mir");
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
	else if(!strcmp(gameState, "login")) {
		login();
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
				strcpy(gameState, "login");
				
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
				FILE *fp = fopen("bin/sav.txt", "w");
				if(fp == NULL) exit(1);
				else {
					fprintf(fp, "%d %d", sound, level);
					fclose(fp);
				}
				
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
				printf("back to main menu\n");
			}
			else if(mx >= var_play.resume_stX && mx <= var_play.resume_stX + var_play.resume_width && my >= var_play.resume_stY && my <= var_play.resume_stY + var_play.resume_height) {
				// go to gameplay
				strcpy(gameState, "gameplay");
				printf("Resume game\n");
			}
			else if(mx >= var_play.new_stX && mx <= var_play.new_stX + var_play.new_width && my >= var_play.new_stY && my <= var_play.new_stY + var_play.new_height) {
				// go to new game
				strcpy(gameState, "gameplay");
				level = 1;
				story = 1;
				printf("Start a new game\n");
			}
			else if(mx >= var_play.sound_stX && mx <= var_play.sound_stX + var_play.sound_width && my >= var_play.sound_stY && my <= var_play.sound_stY + var_play.sound_height) {
				// sound off
				sound = 1 - sound;
				printf("Sound off\n");
			}
			else if(mx >= var_play.credits_stX && mx <= var_play.credits_stX + var_play.credits_width && my >= var_play.credits_stY && my <= var_play.credits_stY + var_play.credits_height) {
				// go to credits
				strcpy(gameState, "credits");
				printf("To credits\n");
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
	if(!strcmp(gameState, "login")) {
		int len = strlen(var_login.temp);
		if(key == '\r') {
			var_login.inputted = 1;
			strcpy(var_login.username, var_login.temp);
			var_login.temp[0] = 0;
		}
		else if(key == '\b') {
			if(len) len--;
			var_login.temp[len] = 0;
		}
		else {
			var_login.temp[len] = key;
			++len;
			var_login.temp[len] = 0;
		}
	}
	else if(!strcmp(gameState, "play-menu")) {
		if(key == 'q') strcpy(gameState, "main-menu");
	}
	else if(!strcmp(gameState, "exit-menu")) {
		if(key == 'q') strcpy(gameState, "main-menu");
	}
	else if(!strcmp(gameState, "gameplay")) {
		if(key == 'q') strcpy(gameState, "main-menu");
	}
	else if(!strcmp(gameState, "credits")) {
		if(key == 'q') strcpy(gameState, "main-menu");
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
	
	strcpy(gameState, "loading");
	iInitialize(width, height, "angry -___-");
	return 0;
}
