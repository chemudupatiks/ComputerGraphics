#include<vector>
#include <string>
#include <iostream>
#include <fstream>
#include <strstream>
#include <windows.h> //change if using xWindows
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <regex>
#include <sstream>
#include "glut.h"
using namespace std;

//@@@@@@@@@@@@@@@@@@ Point3 class @@@@@@@@@@@@@@@@
class Point3 {
public:
	float x, y, z;
	void set(float dx, float dy, float dz) { x = dx; y = dy; z = dz; }
	void set(Point3& p) { x = p.x; y = p.y; z = p.z; }
	Point3(float xx, float yy, float zz) { x = xx; y = yy; z = zz; }
	Point3() { x = y = z = 0; }
	void build4tuple(float v[]){// load 4-tuple with this color: v[3] = 1 for homogeneous
		v[0] = x; v[1] = y; v[2] = z; v[3] = 1.0f;
	}
};

//@@@@@@@@@@@@@@ A new class I added called heightRadius @@@@@@@@@@@@@@@@@@@@@@@@
class heightRadius {
public:
	float height = 0;
	float radius = 0;
	heightRadius() { height = 0; radius = 0; }
	heightRadius(float h, float r) { height = h; radius = r; }
};


//@@@@@@@@@@@@@@@@@@ Vector3 class @@@@@@@@@@@@@@@@
class Vector3 {
public:
	float x, y, z;
	void set(float dx, float dy, float dz) { x = dx; y = dy; z = dz; }
	void set(Vector3& v) { x = v.x; y = v.y; z = v.z; }
	void flip() { x = -x; y = -y; z = -z; } // reverse this vector
	void setDiff(Point3& a, Point3& b){//set to difference a - b
		x = a.x - b.x; y = a.y - b.y; z = a.z - b.z;
	}
	void normalize(){//adjust this vector to unit length
		double sizeSq = x * x + y * y + z * z;
		if (sizeSq < 0.0000001){
			//	cerr << "\nnormalize() sees vector (0,0,0)!";
			return; // does nothing to zero vectors;
		}
		float scaleFactor = 1.0 / (float)sqrt(sizeSq);
		x *= scaleFactor; y *= scaleFactor; z *= scaleFactor;
	}
	Vector3(float xx, float yy, float zz) { x = xx; y = yy; z = zz; }
	Vector3(Vector3& v) { x = v.x; y = v.y; z = v.z; }
	Vector3() { x = y = z = 0; } //default constructor
	Vector3 cross(Vector3 b){ //return this cross b
		Vector3 c(y*b.z - z * b.y, z*b.x - x * b.z, x*b.y - y * b.x);
		return c;
	}
	float dot(Vector3 b){ // return this dotted with b
		return x * b.x + y * b.y + z * b.z;
	}
};

//################# VertexID ###################

class VertexID {
public:
	int vertIndex; // index of this vert in the vertex list
	int normIndex; // index of this vertex's normal
};

//#################### Face ##################
class Face {
public:
	int nVerts = 4; // number of vertices in this face
	VertexID * vert; // the list of vertex and normal indices
	Face() { nVerts = 0; vert = NULL; } // constructor
	~Face() { delete[] vert; nVerts = 0; } // destructor
};

//###################### Mesh #######################
class Mesh {
private:
	int numVerts;	// number of vertices in the mesh
	Point3 * pt;	      // array of 3D vertices
	int numNorms;   // number of normal vectors for the mesh
	Vector3 *norm;    // array of normals 
	int numFaces; 	// number of faces in the mesh
	Face* face;	      // array of face data
public:
	Mesh() {} 		// constructor
	~Mesh();          // destructor

	int readFile(string fileName) { // to read in a filed mesh
		fstream infile;
		infile.open(fileName);
		if (!infile) {
			cout << "Cannot open: Mesh.obj" << endl;
		}
		else {
			numVerts = 0;
			numNorms = 0;
			numFaces = 0;
			regex v("v (.*) (.*) (.*).*");
			regex vn("vn (.*) (.*) (.*).*");
			regex f("f (.*)");
			while (!infile.eof()) {
				string buffer;
				getline(infile, buffer);
				match_results<std::string::const_iterator> check;
				if (regex_match(buffer, check, v)) {
					numVerts++;
				}
				if (regex_match(buffer, check, vn)) {
					numNorms++;
				}
				if (regex_match(buffer, check, f)) {
					numFaces++;
				}
			}
			infile.close();
			infile.open(fileName);
			pt = new Point3[numVerts];
			norm = new Vector3[numNorms];
			face = new Face[numFaces];
			int counterv = 0;
			int countervn = 0;
			int counterf = 0;
			while (!infile.eof()) {
				string buffer;
				getline(infile, buffer);
				match_results<std::string::const_iterator> check;
				if (regex_match(buffer, check, v)) {
					pt[counterv].x = stof(check[1]);
					pt[counterv].y = stof(check[2]);
					pt[counterv].z = stof(check[3]);
					counterv++;
				}
				if (regex_match(buffer, check, vn)) {
					pt[countervn].x = stof(check[1]);
					pt[countervn].y = stof(check[2]);
					pt[countervn].z = stof(check[3]);
					countervn++;
				}
				if (regex_match(buffer, check, f)) {
					istringstream iss(check[1]);
					vector<string> facesInfo;
					string temp;
					while (!iss.eof()) {
						iss >> temp;
						facesInfo.push_back(temp);
					}
					face[counterf].nVerts = facesInfo.size();
					face[counterf].vert = new VertexID[face[counterf].nVerts];
					regex pattern("(.*)/.*/(.*)");
					for (int i = 0; i < face[counterf].nVerts; i++) {						
						match_results<std::string::const_iterator> check1;
						if (regex_match(facesInfo[i], check1, pattern)) {
							face[counterf].vert[i].vertIndex = stoi(check1[1]);
							face[counterf].vert[i].normIndex = stoi(check1[2]);
						}
						else {
							cout << "Error faces doesnt follow the format." << endl << endl;
							break;
						}
					}
					counterf++;
				}
			}
			return 1;
		}
	}
	int readmesh(char * fileName) {
		fstream infile;
		infile.open(fileName, ios::in);
		if (infile.fail()) return -1; // error - can't open file
		if (infile.eof())  return -1; // error - empty file
		infile >> numVerts >> numNorms >> numFaces;
		pt = new Point3[numVerts];
		norm = new Vector3[numNorms];
		face = new Face[numFaces];
		//check that enough memory was found:
		if (!pt || !norm || !face)return -1; // out of memory
		for (int p = 0; p < numVerts; p++) // read the vertices
			infile >> pt[p].x >> pt[p].y >> pt[p].z;
		for (int n = 0; n < numNorms; n++) // read the normals
			infile >> norm[n].x >> norm[n].y >> norm[n].z;
		for (int f = 0; f < numFaces; f++) {// read the faces
			infile >> face[f].nVerts;
			face[f].vert = new VertexID[face[f].nVerts];
			for (int i = 0; i < face[f].nVerts; i++)
				infile >> face[f].vert[i].vertIndex
				>> face[f].vert[i].normIndex;
		}
		return 0; // success
	}
	double X(double v, double radius) {
		return (radius*cos(v));
	}
	double Y(double v, double radius) {
		return (radius*sin(v));
	}
	double nx(double v, double r) {
		return (X(v, r));
	}
	double ny(double v, double r) {
		return((-1 * cos(v))*Y(v, r));
	}
	double nz(double v, double r) {
		return((-1 * cos(v)));
	}
	void makeSurfaceMesh() {
		int i, j, numValsU, numValsV = 100;// set these
		double u, v, uMin = -1, vMin = 0, uMax = 1, vMax = 2 * 3.14;
		double delV = (vMax - vMin) / (numValsV - 1);
		vector<heightRadius> zCoordinates;

		zCoordinates.push_back(heightRadius(1, 0));
		zCoordinates.push_back(heightRadius(0, 1));
		zCoordinates.push_back(heightRadius(-1, 0));

		numValsU = zCoordinates.size();
		numVerts = numValsU * numValsV + 1; // total # of vertices
		numFaces = (numValsU - 1) * (numValsV - 1); // # of faces
		numNorms = numVerts; // for smooth shading - one normal per vertex
		
		pt = new Point3[numVerts];  assert(pt != NULL); // make space 
		face = new Face[numFaces];    assert(face != NULL);
		norm = new Vector3[numNorms]; assert(norm != NULL);

		for (i = 0; i < numValsU; i++) {
			for (j = 0, v = vMin; j < numValsV; j++, v += delV) {
				int whichVert = i * numValsV + j; //index of the vertex and normal
				// set this vertex: use functions X, Y, and Z
				int r = zCoordinates.at(i).radius;
				int h = zCoordinates.at(i).height;
				pt[whichVert].set(X(v, r), Y(v, r), h);
				// cout << "v " << X(v, r) << " " << Y(v, r) << " " << h << " 1" << endl;
				//set the normal at this vertex: use functions nx, ny, nz
				norm[whichVert].set(nx(v, r), ny(v, r), nz(v, r));
				//cout << "vn " << nx(v, r) << " " << ny(v, r) << " " << nz(v,r) << endl;
				norm[whichVert].normalize();
				// make quadrilateral
			}
		}

		for (i = 1; i < numValsU; i++) {
			for (j = 1; j < numValsV; j++){
				int whichVert = i * numValsV + j; //index of the vertex and normal
				int whichFace = (i - 1) * (numValsV - 1) + (j - 1);
				face[whichFace].vert = new VertexID[3];
				face[whichFace].nVerts = 3;
				assert(face[whichFace].vert != NULL);
				if (i == 1) {

					face[whichFace].vert[0].vertIndex = whichVert;
					face[whichFace].vert[0].normIndex = whichVert;
					face[whichFace].vert[1].vertIndex = whichVert - 1;
					face[whichFace].vert[1].normIndex = whichVert - 1;
					face[whichFace].vert[2].vertIndex = whichVert - numValsV - 1;
					face[whichFace].vert[2].normIndex = whichVert - numValsV - 1;
				}
				if (i == 2) {
					face[whichFace].vert[0].vertIndex = whichVert - numValsV;
					face[whichFace].vert[0].normIndex = whichVert - numValsV;
					face[whichFace].vert[1].vertIndex = whichVert - numValsV - 1;
					face[whichFace].vert[1].normIndex = whichVert - numValsV - 1;
					face[whichFace].vert[2].vertIndex = whichVert;
					face[whichFace].vert[2].normIndex = whichVert;
				}

				/*face[whichFace].vert[3].vertIndex =
				face[whichFace].vert[3].normIndex = whichVert - numValsV;*/
			}
		}
	}
	void draw(){
	// use OpenGL to draw this mesh
		for (int f = 0; f < numFaces; f++){ // draw each face
			glBegin(GL_LINE_LOOP);
			for (int v = 0; v < face[f].nVerts; v++){ // for each one..
				int in = face[f].vert[v].normIndex; // index of this normal
				int iv = face[f].vert[v].vertIndex; // index of this vertex
				glNormal3f(norm[in].x, norm[in].y, norm[in].z);
				glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
			}
			glEnd();
		}
	}
};