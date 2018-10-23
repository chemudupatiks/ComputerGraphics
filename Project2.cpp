#include <string>
#include <iostream>
#include <fstream>
#include <strstream>
#include <windows.h> //change if using xWindows
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "glut.h" //modify this for your environment
#include"MeshClass.h"

using namespace std;

float angle = 20;
float angleU = 0.0;
float angleN = 0.0;
float angleV = 0.0;
float translateU = 0.0;
float translateN = 0.0;
float translateV = 0.0;
float xEye = 100.0;
float yEye = 100.0;
float zEye = 100.0;
Mesh * Obj;


void displayObjects(void) {
	glMatrixMode(GL_PROJECTION); // set the view volume shape
	glLoadIdentity();
	gluPerspective(45, 640 / 480.0, 1, 500);
	glMatrixMode(GL_MODELVIEW); // position and aim the camera	
	glLoadIdentity();

	gluLookAt(100.0, 100.0, 100.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3d(0, 0, 0); // draw black lines

	if (translateN != 0) {
		glTranslated(translateN, translateN, translateN);
		if (100 - xEye != translateN) {
			xEye = 100 - translateN;
		}
		if (yEye != 100 - translateN) {
			yEye = 100 - translateN;
		}
		if(zEye != 100 - translateN){
			zEye = 100 - translateN;
		}
	}
	if (translateU != 0) {
		glTranslated(-1*translateU, 0, translateU);
		if (100 - xEye != -1* translateU) {
			xEye = 100 + translateU;
		}
		if (100 - zEye != translateU) {
			zEye = 100 - translateU;
		}
	}
	if (translateV != 0) {
		glTranslated(-1*translateV, 2*translateV, -1*translateV);
		if (100 - xEye != -1*translateV) {
			xEye = 100 + translateV;
		}
		if (yEye != 100 - 2*translateV) {
			yEye = 100 - 2*translateV;
		}
		if (zEye != 100 + translateV) {
			zEye = 100 + translateV;
		}
	}

	if(angleN != 0) {
		glRotated(angleN, 1, 1, 1);
	}
	if (angleU != 0) {
		glTranslated(xEye, yEye, zEye);
		glRotated(angleU, -1, 0, 1);
		glTranslated(-1* xEye, -1* yEye, -1* zEye);
	}
	if (angleV != 0) {
		glTranslated(xEye, yEye, zEye);
		glRotated(angleV, -1, 2, -1);
		glTranslated(-1 * xEye, -1 * yEye, -1 * zEye);
	}

	glPushMatrix();
	glTranslated(-80, -80, 5);
	glScaled(20, 20, 20);
	Mesh * M;
    M = new Mesh();
    M->makeSurfaceMesh();
    M->draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(40, 40, 0);
	glRotated(40, 1, 0, 0);
	glRotated(30, 1, 1, 0);
	glScaled(30, 30, 30);
	Mesh * M1;
	M1 = new Mesh();
	M1->makeSurfaceMesh();
	M1->draw();
	glPopMatrix();

	glPushMatrix();
	//glTranslated(-10, -10, -10);
	glScaled(10, 10, 10);
	glRotated(45, 1, 0,0);
	Mesh * M2;
	M2 = new Mesh();
	M2->makeSurfaceMesh();
	M2->draw();
	glPopMatrix();

	glPushMatrix();
	///glScaled(15, 15, 15);
	glTranslated(-20, 20, 50);
	glScaled(15, 15, 15);
	glRotated(45, 1, 1, 0);
	//glScaled(15, 15, 15);
	Mesh * M3;
	M3 = new Mesh();
	M3->makeSurfaceMesh();
	M3->draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-50, 30, 5);
	glRotated(60, 1, 1,0);
	glScaled(23, 23, 23);
	Mesh * M4;
	M4 = new Mesh();
	M4->makeSurfaceMesh();
	M4->draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(30, -35, 5);
	glRotated(80, 1, 1, 1);
	glScaled(25, 25, 25);
	Mesh * M5;
	M5 = new Mesh();
	M5->makeSurfaceMesh();
	M5->draw();
	glPopMatrix();

	glPushMatrix();
	glScaled(25, 25, 25);
	Obj->draw();
	glPopMatrix();


	glFlush();
	glutSwapBuffers();
}

void initMaterial() {

}



void initLighting() {
	glEnable(GL_LIGHTING);

	// set up light colors (ambient, diffuse, specular)

	GLfloat lightKa[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // ambient light
	GLfloat lightKd[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // diffuse light
	GLfloat lightKs[] = { 1.0f, 1.0f, 1.0f, 1 };           // specular light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

	// position the light
	float lightPos[4] = { 0, 1.5, -1,1 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration
}

//void myKeyboardInt(int key, int x, int y) {
//	switch (key) {
//	case 5:
//		translateN += angle;
//		break;
//	case 1:
//		translateN -= angle;
//		break;
//	case 4:
//		translateU += angle;
//		break;
//	case 6:
//		translateU -= angle;
//		break;
//	case 8:
//		translateV += angle;
//		break;
//	case 2:
//		translateV -= angle;
//		break;
//	default:
//		break;
//	}
//	glutPostRedisplay();
//}

void myKeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case '5':
		translateN += angle;
		break;
	case '1':
		translateN -= angle;
		break;
	case '4':
		translateU -= angle;
		break;
	case '6':
		translateU += angle;
		break;
	case '8':
		translateV -= angle;
		break;
	case '2':
		translateV += angle;
		break;
	case 'a':
	case 'A':
		angleV -= angle;
		break;
	case 'd':
	case 'D':
		angleV += angle;
		break;
	case 'w':
	case 'W':
		angleU += angle;
		break;
	case 's':
	case 'S':
		angleU -= angle;
		break;
	case 'q':
	case 'Q':
		angleN += angle;
		break;
	case 'e':
	case 'E':
		angleN -= angle;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}
//<<<<<<<<<<<<<<<<<<<<<< main >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void main(int argc, char **argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Camera Transformation");	
	glutDisplayFunc(displayObjects);
	glEnable(GL_DEPTH_TEST);
	initLighting();
	initMaterial();
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // background is white
	//glEnable(GL_FLAT);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_SMOOTH);
	glViewport(0, 0, 640, 480);
	//glutSpecialFunc(myKeyboardInt);
	glutKeyboardFunc(myKeyboard);
	Obj = new Mesh();
	Obj->readFile("capsule.obj");
	glutMainLoop();

}
