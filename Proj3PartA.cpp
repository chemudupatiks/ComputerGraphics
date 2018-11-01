#include <GL/glut.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include "Matrix.h"

using namespace std;

class Sphere {
public:
	Sphere(float xPos, float yPos, float zPos, float radius, unsigned char redC, unsigned char greenC, unsigned char blueC, unsigned char alphaC) {
		x = xPos;
		y = yPos;
		z = zPos;
		r = radius;
		red = redC;
		blue = blueC;
		green = greenC;
		alpha = alphaC;
		setMatrix();
		setInverseMatrix();
	}
	Sphere() { x = 0; y = 0; z = 0; r = 1; red = 0; blue = 0; green = 0; alpha = 0; }
	void setMatrix() {
		transform.SetToIdentity();
		//transform.Print();
		//cout << "------------------------" << endl;
		transform.Translate(x, y, z);
		//transform.Print();
		//cout << "------------------------" << endl;
		transform.Scale(r, r, r);
		//cout << "Transform: " << endl;
		//transform.Print();
		//cout << "------------------------" << endl;
	}
	void setInverseMatrix() {
		Matrix mt;
		mt.SetToIdentity();
		transform.storeInverse(mt);
		inverse = mt;
	}
	float x;
	float y;
	float z;
	float r;
	Matrix transform;
	Matrix inverse;
	unsigned char red;
	unsigned char blue;
	unsigned char green;
	unsigned char alpha;
};

class RGBApixel {
public:
	unsigned char r, g, b, a;
};

class Ray {
public:
	Vector dir;
	Point eye;
	Ray() {
		eye = Point();
		dir = Vector();
	}
	Ray(Point Eye, Vector direction) {
		eye = Eye;
		dir = direction;
	}
};

int Height = 480;
int Width = 640;
int blocksize = 10;
RGBApixel ** pixmap2d;
int rows = Height;
int columns = Width;



void drawScreen() {

	glRasterPos2i(0, 0);
	glDrawPixels(Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, pixmap2d[0]);
	glFlush();
}

vector<float> hit(Point p, Vector v) {
	float A = v.dot(v);
	float B = v.dot(p);
	float C = p.dot(p) - 1;
	float det = (B*B)-(A*C);
	vector<float> t;
	if (det < 0) {
	}
	else if (det == 0) {
		t.push_back(-1 * B / A);
	}
	else {
		int t1 = (-1 * B / A) + (sqrt(det) / A);
		int t2 = (-1 * B / A) - (sqrt(det) / A);
		t.push_back(t1);
		t.push_back(t2);
	}
	return t;
}


int main() {
	int row, col;

	vector<Sphere> objects;
	objects.push_back(Sphere(0.125, -0.25, -1, 0.125, 255, 0, 0, 255));
	objects.push_back(Sphere(0.5, 0.5, -1.75, 0.375, 0, 0, 255, 255));
	objects.push_back(Sphere(-0.5, 0, -2.5, 0.75, 0, 255, 0, 255));

	Vector n(0,0,1);
	Vector v(0,1,0);
	Vector u(1,0,0);
	Point eye(0, 0, 1);

	pixmap2d = new RGBApixel*[Height];
	pixmap2d[0] = new RGBApixel[Width * Height];
	for (int i = 1; i < Height; i++) {
		pixmap2d[i] = pixmap2d[i - 1] + Width;
	}
	for (row = 0; row < Height; row++) {
		for (col = 0; col < Width; col++) {
			bool hitFound = false;
			int objIndex = -1;
			float th = FLT_MAX;
			float k = -1;
			float l = 0.5* ((2 * (float)col / (float) columns)-1);
			float m = 0.4* ((2 * (float)row / (float) rows)-1);
			Vector dir;
			dir = n * k + u * l + v * m;
			for (int i = 0; i < objects.size(); i++) {
				Ray inverse (objects[i].inverse * eye, objects[i].inverse*dir);
				//inverse.eye.Print();
				//inverse.dir.Print();
				//cout << "===================================" << endl;
				vector<float> hits (hit(inverse.eye, inverse.dir));
				if (hits.size() > 0) {
					hitFound = true;
					float minTime = hits[0];
					if (hits.size() == 2) {
						minTime = min(hits[0], hits[1]);
					}
					if (minTime < th) {
						th = minTime;
						objIndex = i;
					}
				}
			}
			if (hitFound) {
				pixmap2d[row][col].r = objects[objIndex].red; pixmap2d[row][col].g = objects[objIndex].green;
				pixmap2d[row][col].b = objects[objIndex].blue; pixmap2d[row][col].a = 255;
			}
			else {
				pixmap2d[row][col].r = 0; pixmap2d[row][col].g = 0;
				pixmap2d[row][col].b = 0; pixmap2d[row][col].a = 255;
			}
		}
	}
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("Raycaster");
	glMatrixMode(GL_PROJECTION); //set projection to 2D
	glLoadIdentity();
	gluOrtho2D(0, Width, 0, Height);
	glMatrixMode(GL_MODELVIEW); // position and aim the camera	
	glLoadIdentity();
	gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);
	
	// register display callback routine
	glutDisplayFunc(drawScreen);
	glViewport(0, 0, Width, Height);
	glutMainLoop();
	
	return 0;
}