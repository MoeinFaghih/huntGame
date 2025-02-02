/*Seyedmoein Faghih

*/

#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include<time.h>
#include <cmath>


#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 700
#define TIMER_PERIOD  20
#define TIMER_ON 1
#define D2R 0.0174532
#define PI 3.1415
#define MAX_FIRE 15
#define MAX_FIRERATE 9
#define MIN_FIRERATE 1
#define MAX_PROJSPEED 15
#define MIN_PROJSPEED 5
#define MAX_OBJ 5

#define MENU 0
#define OPTIONS 1
#define RUN 2
#define GAMEOVER 3
#define USER 4
#define HIGHSCORE 5

typedef struct {
	float x, y;
} point_t;

typedef struct {

	point_t Pos;
	double wingY;
	int wingDirection;
	bool active;
} fire_t;

typedef struct {
	char uname[40];
	double score;
}player_t;

point_t player = { -440,0 };
point_t string = { player.x - 7,player.y };
fire_t objects[MAX_OBJ];
fire_t arrow = { {player.x, player.y}, false };
int lastScore = 0, totallScore = 0, totallObj = 0;
double timer = 0;
bool activeTimer = true;

char fRateString[10], pSpeedString[10], scoreString[20], username[40] = "";

int clockFlag = 0, chance1, chance2, hp = 3, state = RUN, fireRate = 3, fRateCircleX = 0, projSpeed = 10, pSpeedCircleX = 0, currentScore;

double score = 0, multiplier = 2;

fire_t   fire[MAX_FIRE];
player_t arr[40];


void vprint(int x, int y, void* font, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
		glutBitmapCharacter(font, str[i]);
}

void vprint2(int x, int y, float size, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	glPopMatrix();
}

int testCollision(fire_t obj)
{
	if (arrow.Pos.x + 50 > obj.Pos.x - 7 && arrow.Pos.x + 50 < obj.Pos.x + 8)
		if (arrow.Pos.y > obj.Pos.y - 55 && arrow.Pos.y < obj.Pos.y + 78) {


			return(5 - abs(obj.Pos.y + 10 - arrow.Pos.y) / 16.62);
		}
	return(-1);
}

void resetVars()
{
	for (int i = 0; i < MAX_OBJ; i++)
		objects[i].active = false;
}

int findAvailableFire() {
	for (int i = 0; i < MAX_OBJ; i++)
		if (objects[i].active == false) {
			return i;

		}
	return -1;
}

void circle(int x, int y, int r) {
	float angle;

	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++) {
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void drawBow(float centerX, float centerY, float radius, float startAngle, float endAngle) {

	glLineWidth(10);
	glBegin(GL_LINE_STRIP);
	glColor3ub(102, 51, 0); // brown color


	// Convert angles from degrees to radians
	startAngle = startAngle * 3.14159 / 180.0;
	endAngle = endAngle * 3.14159 / 180.0;

	// Draw the arch
	for (float angle = startAngle; angle <= endAngle; angle += 0.01) {
		float x = centerX + radius * cos(angle);
		float y = centerY + radius * sin(angle);
		glVertex2f(x, y);
	}
	glEnd();

	glLineWidth(2);
	//Draw the string
	glBegin(GL_LINES);
	glColor3f(0, 0, 0);
	glVertex2f(centerX + radius * cos(startAngle), centerY + radius * sin(startAngle));
	glVertex2f(string.x, string.y);
	glVertex2f(string.x, string.y);
	glVertex2f(centerX + radius * cos(endAngle), centerY + radius * sin(endAngle));


	glEnd();


}


void drawArrow()
{
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex2f(arrow.Pos.x - 7, arrow.Pos.y);
	glVertex2f(arrow.Pos.x + 50, arrow.Pos.y);


	glBegin(GL_POLYGON);
	glColor3ub(51, 26, 0);
	glVertex2f(arrow.Pos.x + 46, arrow.Pos.y + 4);
	glVertex2f(arrow.Pos.x + 50, arrow.Pos.y);
	glVertex2f(arrow.Pos.x + 46, arrow.Pos.y - 4);


	glEnd();
}

void drawEllipse(float X, float Y, float radiusX, float radiusY, int numSegments) {
	glBegin(GL_POLYGON);
	for (int i = 0; i < numSegments; i++) {
		float theta = 2.0f * 3.1415926f * float(i) / float(numSegments);
		float x = radiusX * cosf(theta);
		float y = radiusY * sinf(theta);

		glVertex2f(x + X, y + Y);
	}
	glEnd();
}

void drawBird(int centerX, int centerY, int wingVertexY)
{
	//legs
	glLineWidth(2);
	glBegin(GL_LINES);
	glColor3f(0, 0, 0);
	glVertex2f(centerX - 2, centerY);
	glVertex2f(centerX - 2, centerY - 50);
	glVertex2f(centerX + 2, centerY);
	glVertex2f(centerX + 2, centerY - 50);
	glEnd();

	//beak
	glBegin(GL_TRIANGLES);
	glVertex2f(centerX - 2, centerY + 45);
	glVertex2f(centerX - 2, centerY + 72);
	glVertex2f(centerX + 4, centerY + 45);
	glEnd();

	//Body, Neck, and Head
	glColor3ub(255, 128, 128);
	glRectf(centerX + 3, centerY, centerX - 3, centerY + 45);
	drawEllipse(centerX, centerY, 13, 24, 800);
	circle(centerX, centerY + 45, 10);

	//Eye
	glColor3f(0, 0, 0);
	circle(centerX + 2, centerY + 45, 2);

	//Wings
	glColor3ub(255, 204, 204);
	glBegin(GL_TRIANGLES);
	glVertex2f(centerX, centerY + 8);
	glVertex2f(centerX, centerY - 8);
	glVertex2f(centerX - 32, wingVertexY);
	glEnd();

}




void displayRun()
{
	glBegin(GL_POLYGON);
	glColor3ub(0, 61, 153);
	glVertex2f(-450, 300);
	glVertex2f(450, 300);
	glColor3ub(250, 254, 255);
	glVertex2f(450, -300);
	glVertex2f(-450, -300);

	glEnd();

	glColor3f(0, 0, 0);
	drawBow(player.x, player.y, 50, -60, 60);
	drawArrow();

	glColor3f(0, 0, 0);
	for (int i = 0; i < MAX_OBJ; i++)
		if (objects[i].active == true)
			drawBird(objects[i].Pos.x, objects[i].Pos.y, objects[i].wingY);

	glColor3f(0.8, 0.8, 0.8);
	glRectf(-640, 350, 640, 300);
	glRectf(-640, -350, 640, -300);
	glRectf(450, -350, 640, 300);

	glColor3f(0, 0, 0);
	vprint(460, 300, GLUT_BITMAP_TIMES_ROMAN_24, "Remaining Time:");
	vprint(540, 270, GLUT_BITMAP_TIMES_ROMAN_24, "%.2f", 20 - timer);
	vprint(460, 200, GLUT_BITMAP_TIMES_ROMAN_24, "Totall Objects:");
	vprint(540, 170, GLUT_BITMAP_TIMES_ROMAN_24, "%d", totallObj);
	vprint(520, -320, GLUT_BITMAP_TIMES_ROMAN_10, "Developed by MoeinFaghih");

}



void display() {
	// Draw window
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glLineWidth(3);

	displayRun();




	



	glutSwapBuffers();
}

void onResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void onSpecialKeyDown(int key, int x, int y)
{
	if (key == GLUT_KEY_DOWN && player.y > -280)
	{
		player.y -= 7;
		string.y -= 7;
		if (!arrow.active)
			arrow.Pos.y -= 7;
	}
	if (key == GLUT_KEY_UP && player.y < 280)
	{
		player.y += 7;
		string.y += 7;
		if (!arrow.active)
			arrow.Pos.y += 7;
	}

	glutPostRedisplay();
}

void onKeyDown(unsigned char key, int x, int y) {
	if (key == ' ' && !arrow.active)
		arrow.active = true;
}


void Init() {
	//Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_POINT_SMOOTH);

}


void onTimer(int v)
{
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);

	if (activeTimer)
	{
		if (timer < 19.98)
			timer += 0.02;
		else
			activeTimer = !activeTimer;




		if (state == RUN)
		{
			clockFlag++;


			if (clockFlag % (100 / fireRate) == 0)  //Launching the objects
			{

				int availFire = findAvailableFire();

				if (availFire != -1)
				{
					objects[availFire].active = true;
					totallObj++;

					objects[availFire].Pos.x = rand() % 320 + 100;
					objects[availFire].Pos.y = objects[availFire].wingY = -350;

				}
			}



			//moving the objects
			for (int i = 0; i < MAX_OBJ; i++) {
				if (objects[i].active) {
					objects[i].Pos.y += 5;

					switch (objects[i].wingDirection)  //this variable indicates wether the wings are moving upwards(1), or downwards(2).
					{
					case 0:	 //Wings going downwards
						if (objects[i].Pos.y - objects[i].wingY > 35)  //check to see if the wings are at the lowest point
						{
							objects[i].wingDirection = 1;  //changes the direction to upwards
							objects[i].wingY += 5 + 1.3;	//Moves the wings vertically
						}
						else
							objects[i].wingY -= -5 + 1.3;	//continues the movement downwards
						break;
					case 1:  //Wings going upwards
						if (objects[i].wingY - objects[i].Pos.y > 35)	//check to see if the wings are at the highest point
						{
							objects[i].wingDirection = 0;	//changes the direction to downwards
							objects[i].wingY -= -5 + 1.3;	//Moves the wings vertically
						}
						else
							objects[i].wingY += 5 + 1.3;	//continues the movement upwards
					}

					if (objects[i].Pos.y > WINDOW_HEIGHT / 2)
						objects[i].active = false;
					currentScore = testCollision(objects[i]);
					if (currentScore != -1) {
						objects[i].active = false;
						printf("%d\n", currentScore);

					}
				}
			}

			//moving the arrow
			if (arrow.active)
			{
				arrow.Pos.x += 8;
				if (arrow.Pos.x > 460)
				{
					arrow.active = false;
					arrow.Pos.x = player.x;
					arrow.Pos.y = player.y;
					string.x = player.x - 9;
				}

				if (string.x < player.x + 25)
					string.x += 8;
			}

		}




		// To refresh the window it calls display() function
		glutPostRedisplay(); // display()
	}
}




int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Moein Faghih");
	srand(time(NULL));
	resetVars();


	// Window Events
	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);

	//glutMouseFunc(onClick);

	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	Init();
	glutMainLoop();
	return 0;
}