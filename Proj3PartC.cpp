// Modified Professor's Matrix.h and Matrix.cpp to create 
// Matrix.h and Matrix.cpp


#include <GL/glut.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include "Matrix.h"

using namespace std;

//Created a Sphere class to create inverse matrix automatically.
class Sphere {
public:
	Sphere(float xPos, float yPos, float zPos, float radius, float ar, float ag, float ab, float aa, float dr, float dg, float db, float da, float sr, float sg, float sb, float sa, float shine) {
		x = xPos;
		y = yPos;
		z = zPos;
		r = radius;
		//unsigned char redC, unsigned char greenC, unsigned char blueC, unsigned char alphaC,
		//red = redC;
		//blue = blueC;
		//green = greenC;
		//alpha = alphaC;
		setMatrix();
		setInverseMatrix();
		materialAmb = Vector(ar, ag, ab, aa);
		materialDiff = Vector(dr, dg, db, da);
		materialSpec = Vector(sr, sg, sb, sa);
		shininess = shine;
	}
	//Sphere(const Sphere& sphere) {}
	Sphere() { x = 0; y = 0; z = 0; r = 1; materialAmb = Vector(0, 0, 0, 1); materialDiff = Vector(0, 0, 0, 1);	materialSpec = Vector(0, 0, 0, 1); shininess = 0; /*red = 0; blue = 0; green = 0; alpha = 0;*/ }
	void setMatrix() {
		transform.SetToIdentity();
		transform.Translate(x, y, z);
		transform.Scale(r, r, r);
	}
	
	void setInverseMatrix() {
		Matrix mt;
		mt.SetToIdentity();
		transform.storeInverse(mt);
		inverse = mt;
	}
	//------------------------------------------------------
	float x;
	float y;
	float z;
	float r;
	Matrix transform;
	Matrix inverse;
	//unsigned char red;
	//unsigned char blue;
	//unsigned char green;
	//unsigned char alpha;
	Vector materialAmb;
	Vector materialDiff;
	Vector materialSpec;
	float shininess;
};

//RGBAPixel for pixmaps.
class RGBApixel {
public:
	unsigned char r, g, b, a;
};

//Ray class to store the calculated rays.
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

//class Illumination {
//public:
//	Vector rgba;
//	Illumination() {
//		rgba.h = 1;
//	}
//	Illumination(float r, float g, float b, float a) {
//		rgba.x = r;
//		rgba.y = g;
//		rgba.z = b;
//		rgba.h = a;
//	}
//};


class Light {
public:
	int type = -1;
	Point position;
	Vector direction;
	Vector Amb;
	Vector Diff;
	Vector Spec;
	float angle = 0;
	float exponent = 0;
	Light(){
		Amb = Vector(0, 0, 0, 1);
		Diff = Vector(0, 0, 0, 1);
		Spec = Vector(0, 0, 0, 1);
		type = 0;
	}
	//Light(const Light& light) {}
	Light(int t, float x, float y, float z, float xd, float yd, float zd, float ar, float ag, float ab, float aa, float dr, float dg, float db, float da, float sr, float sg, float sb, float sa, float ang, float exp) {
		position = Point(x, y, z);
		direction = Vector(xd, yd, zd);
		Amb = Vector(ar, ag, ab, aa);
		Diff = Vector(dr, dg, db, da);
		Spec = Vector(sr, sg, sb, sa);
		angle = ang;
		exponent = exp;
		type = t;
	}
};
//Setting the Height and the width of the viewScreen. It also sets the blocksize.
//Increase the blocksize to get a faster render but much pizelated result.
int Height = 480;
int Width = 640;
int blocksize =1;
RGBApixel ** pixmap2d;
int rows = Height;
int columns = Width;
int depth = 2;

//Draws the pixmap.
void drawScreen() {
	glRasterPos2i(0, 0);
	glDrawPixels(Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, pixmap2d[0]);
	glFlush();
}

//Calculates hits and pushes them into vector which is later returned.
vector<float> hit(Point p, Vector v) {
	float A = v.dot(v);
	float B = v.dot(p);
	float C = p.dot(p) - 1;
	//cout << "A: " << A << endl;
	//cout << "B: " << B << endl;
	//cout << "C: " << C << endl;
	float det = (B*B)-(A*C);
	//cout << "DET: " << det << endl;
	vector<float> t;
	if (det < 0) {
	}
	else if (det == 0) {
		t.push_back(-1 * B / A);
		/*cout << "A: " << A << endl;
		cout << "B: " << B << endl;
		cout << "C: " << C << endl;
		cout << "DET: " << det << endl;*/
	}
	else {
		float t1 = (-1 * B / A) + (sqrt(det) / A);
		float t2 = (-1 * B / A) - (sqrt(det) / A);
		if (t1 > 0) {
			t.push_back(t1);
		}
		if (t2 > 0) {
			t.push_back(t2);
		}
		/*cout << "A: " << A << endl;
		cout << "B: " << B << endl;
		cout << "C: " << C << endl;
		cout << "DET: " << det << endl;*/
	}
	return t;
}

Vector shade(Point eye, Vector dir, float th, bool hitFound, int objIndex, Vector background, vector<Sphere>& objects, vector<Light>& lights, int &level, int depth) {
	Vector total(0, 0, 0, 1);
	if (level <= depth) {
		if (hitFound) {
			Point hitPoint(eye + dir * th);
			total = (lights[0].Amb * objects[objIndex].materialAmb);
			//total.Print();
			Vector Id(0, 0, 0, 1);
			Vector Is(0, 0, 0, 1);
			Vector L;
			Vector N = hitPoint - Point(objects[objIndex].x, objects[objIndex].y, objects[objIndex].z);
			N.normalize();
			for (int j = 1; j < lights.size(); j++) {
				if (lights[j].type != 3 /*&& lights[j].type != 2*/) {
					L = lights[j].position - hitPoint;
				}
				else {
					L = lights[j].direction;
				}
				L.normalize();
				/*if (lights[j].type == 3) {
				L.Print();
				}*/
				Vector V = eye - hitPoint;
				V.normalize();

				Vector H = (L + V) * Vector(0.5, 0.5, 0.5);
				H.normalize();
				float LdotN = max(L.dot(N), 0.0f);
				Id = Id + lights[j].Diff * objects[objIndex].materialDiff * Vector(LdotN, LdotN, LdotN);
				//Id.Print();
				float HdotNToPower = pow(max(H.dot(N), 0.0f), objects[objIndex].shininess);
				Is = Is + lights[j].Spec * objects[objIndex].materialSpec * Vector(HdotNToPower, HdotNToPower, HdotNToPower);
				if (lights[j].type == 2) {
					float Pi = 3.141592653;
					float cosAlpa = (L * Vector(-1, -1, -1)).dot(lights[j].direction);
					float cosBeta = cos(lights[j].angle * Pi / 180.0);
					if (cosAlpa > cosBeta) {
						float atten = 1;
						float csE = pow(cos(lights[j].angle * Pi / 180.0), lights[j].exponent);
						Is = Is * csE * atten;
					}
					else {
						Is = Vector(0, 0, 0, 1);
					}
				}
			}
			float correction = -0.01;
			Point hitPointCorrected = hitPoint + Vector(correction, correction, correction) * dir;
			float t = 2 * dir.dot(N);
			bool hitFoundRefl = false;
			Vector temp(t, t, t);
			Vector refl = dir - temp * N;
			float thRefl = FLT_MAX;
			int objIndexRefl = -1;
			for (int i = 0; i < objects.size(); i++) {
				Ray inverse(objects[i].inverse * hitPointCorrected, objects[i].inverse*refl);
				vector<float> hits(hit(inverse.eye, inverse.dir));
				if (hits.size() > 0) {
					//cout << "Inverted ray for row: " << row << " and col: " << col << " and object: " << i << " is:" << endl;
					hitFoundRefl = true;
					float minTime = hits[0];
					if (hits.size() == 2) {
						minTime = min(hits[0], hits[1]);
					}
					if (minTime < thRefl) {
						thRefl = minTime;
						objIndexRefl = i;
					}
				}
			}
			level++;
			Vector Ir = shade(hitPointCorrected, refl, thRefl, hitFoundRefl, objIndexRefl, background, objects, lights, level, depth);
			total = total + Id + Is + Ir;
			if (total.x > 1) { total.x = 1; }
			if (total.y > 1) { total.y = 1; }
			if (total.z > 1) { total.z = 1; }
			if (total.h > 1) { total.h = 1; }
		}
		else {
			total = lights[0].Amb * background;
		}
	}	
	return total;
}
void myKeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case '+':
		if (depth < 3) {
			depth++;
		}
		cout << "+ Recorded!" << endl;
		break;
	case '-':
		if (depth > 1) {
			depth--;
		}
		cout << "- Recorded!" << endl;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

// Creating the Sphere objects and the vector u, v, and n is done in the main. 
// We set the pixel's color values by iterating through each pixel adn finding hits with all objects. 
// The pixel is colored according to the first object hit.
int main() {
	int row, col;
	vector<Sphere> objects;
	objects.push_back(Sphere(0.125, -0.25, -1, 0.125, 0.5, 0.5, 0.5, 1.0, 0.7, 0.7, 0.7, 1.0, 0.9, 0.9, 0.9, 1.0, 3.0));
	objects.push_back(Sphere(0.5, 0.5, -1.75, 0.375, 0.5, 0.5, 0.5, 1.0, 0.4, 0.4, 0.4, 1.0, 0.001, 0.001, 0.001, 1.0, 1.0));
	objects.push_back(Sphere(-0.5, 0, -2.5, 0.75, 0.23125, 0.23125, 0.23125, 1.0, 0.2775, 0.2775, 0.2775, 1.0, 0.6, 0.6, 0.6, 1.0, 89.6));
	//objects.push_back(Sphere(-0.5, 0, -2.5, 0.75, 0.5, 0.5, 0.5, 1.0, 0.4, 0.4, 0.4, 1.0, 0.001, 0.001, 0.001, 1.0, 1.0));
	vector<Light> lights;
	//Light(float x, float y, float z, float xd, float yd, float zd, float ar, float ag, float ab, float aa, float dr, float dg, float db, float da, float sr, float sg, float sb, float sa, float ang, float exp)
	lights.push_back(Light(0, 0, 0, 0, 0, 0, 0, 0.25, 0.05, 0.05, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0)); //global ambient
	lights.push_back(Light(1, 200, 100, 50, 0, 0, 0, 0.65, 0.65, 0.65, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0)); //point light
	lights.push_back(Light(2, -1, 0, 1, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, 1, 0.40, 0.40, 0.70, 1, 30, 0.5)); //spotlight
	lights.push_back(Light(3, 0, 0, 0, -400, 692, 0, 0, 0, 0, 1, 0.80, 0.80, 0.60, 1, 0, 0, 0, 1, 0, 0)); //directional light

	Vector n(0,0,1);
	Vector v(0,1,0);
	Vector u(1,0,0);
	Point eye(0, 0, 1);
	Vector background(0, 0, 0, 1);
	pixmap2d = new RGBApixel*[Height];
	pixmap2d[0] = new RGBApixel[Width * Height];
	for (int i = 1; i < Height; i++) {
		pixmap2d[i] = pixmap2d[i - 1] + Width;
	}
	for (row = blocksize-1; row < Height; row+=blocksize) {
		for (col = blocksize-1; col < Width; col+=blocksize) {
			bool hitFound = false;
			int level = 0;
			int objIndex = -1;
			float th = FLT_MAX;
			float k = -1;
			float l = 0.5* ((2 * (float)col / (float) columns)-1);
			float m = 0.4* ((2 * (float)row / (float) rows)-1);
			Vector dir;
			dir = n * k + u * l + v * m;
			for (int i = 0; i < objects.size(); i++) {
				Ray inverse (objects[i].inverse * eye, objects[i].inverse*dir);
				//cout << "Inverted ray for row: " << row << " and col: " << col << " and object: " << i << " is:" << endl;
				//inverse.eye.Print();
				//inverse.dir.Print();
				vector<float> hits (hit(inverse.eye, inverse.dir));
				if (hits.size() > 0) {
					//cout << "Inverted ray for row: " << row << " and col: " << col << " and object: " << i << " is:" << endl;
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
			Vector total = shade(eye, dir, th, hitFound, objIndex, background, objects, lights, level, depth);
			
			//Add reflected ray component.

			for (int i = row - blocksize + 1; i <= row; i++) {
				for (int j = col - blocksize + 1; j <= col; j++) {
					pixmap2d[i][j].r = total.x * 255; pixmap2d[i][j].g = total.y * 255;
					pixmap2d[i][j].b = total.z * 255; pixmap2d[i][j].a = total.h * 255;
				}
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
	glutKeyboardFunc(myKeyboard);
	glutMainLoop();
	system("pause");
	return 0;
}