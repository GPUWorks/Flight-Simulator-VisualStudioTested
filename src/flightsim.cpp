/*
 *  mp2.cpp
 *  
 *
 *  Created by Ryan Barril on 10/3/12.
 *
 */


#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

float sealevel;
float polysize;

class VecOps {
public:
	
	// cross product of vectors j and k
	GLfloat * cross(GLfloat * const& j, GLfloat * const& k) {
		GLfloat * ret = new GLfloat[3];
		ret[0] = j[1] * k[2] - j[2] * k[1];
		ret[1] = j[2] * k[0] - j[0] * k[2];
		ret[2] = j[0] * k[1] - j[1] * k[0];
		return ret;
	}
	
	// multiplies the 3x3 matrix (represented by a 4x4 homogenous matrix) by a 3x1 vector...not needed anymore
	GLfloat * matvec3(GLfloat * const& mat, GLfloat * const& vec) {
		GLfloat * ret = new GLfloat[3];
		
		ret[0] = mat[0] * vec[0] + mat[1] * vec[1] + mat[2] * vec[2];
		ret[1] = mat[4] * vec[0] + mat[5] * vec[1] + mat[6] * vec[2];
		ret[2] = mat[8] * vec[0] + mat[9] * vec[1] + mat[10] * vec[2];

		return ret;
	}
	
	// magnitude of vec
	GLfloat magnitude(GLfloat * const& vec) {
		return sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	}
	
	// unit vector in direction of vec
	GLfloat * normalize(GLfloat * const &vec) {
		GLfloat * ret = new GLfloat[3];
		GLfloat mag = magnitude(vec);
		ret[0] = vec[0] / mag;
		ret[1] = vec[1] / mag;
		ret[2] = vec[2] / mag;
		return ret;
	}
	
	// rotates vec by t degrees about dir (formula courtesy of Wikipedia)
	GLfloat * rotate(GLfloat * vec, GLfloat t, GLfloat * dir) {
		GLfloat * ret = new GLfloat[3];

		GLfloat a = 0; GLfloat b = 0; GLfloat c = 0;
		GLfloat x = vec[0]; GLfloat y = vec[1]; GLfloat z = vec[2];
		GLfloat u = dir[0]; GLfloat v = dir[1]; GLfloat w = dir[2];

		ret[0] = (a*(v*v+w*w) - u*(b*v+c*w - u*x - v*y - w*z))*(1-cos(t)) + x*cos(t)+(-c*v+b*w-w*y+v*z)*sin(t);
		ret[1] = (b*(u*u+w*w) - v*(a*u+c*w-u*x-v*y-w*z))*(1-cos(t)) + y*cos(t) + (c*u-a*w+w*x-u*z)*sin(t);
		ret[2] = (c*(u*u+v*v) - w*(a*u+b*v-u*x-v*y-w*z))*(1-cos(t))+z*cos(t)+(-b*u+a*v-v*x+u*y)*sin(t);
		
		return ret;
	}
};

class Camera {
public:
	GLfloat eye[3], look[3], up[3]; // camera vector system
	GLfloat * rotv;
	VecOps ops;
	
	// Initialize camera
	Camera() {
		eye[0] = 1.0; eye[1] = 0.0; eye[2] = 0.1;
		look[0] = -1.0; look[1] = 1.0; look[2] = 0.0;
		up[0] = 0.0; up[1] = 0.0; up[2] = 1.0;
		rotv = ops.cross(look, up);

		normalizeVectors();
	}
	
	// Move camera in direction of lookat vector by speed * magnitude(normalize(lookat vector))
	void moveForward(GLfloat speed) {
		GLfloat dir_x = look[0];
		GLfloat dir_y = look[1];
		GLfloat dir_z = look[2];

		GLfloat mag = sqrt(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);
		
		eye[0] += speed * dir_x / mag; 
		eye[1] += speed * dir_y / mag; 
		eye[2] += speed * dir_z / mag;
		
		/* Debugging Information
		printf("Eye: %f %f %f\n", eye[0], eye[1], eye[2]);
		printf("Up: %f %f %f\n", up[0], up[1], up[2]);
		printf("Look: %f %f %f\n", look[0], look[1], look[2]);
		printf("Rotv: %f %f %f\n", rotv[0], rotv[1], rotv[2]);
		 */

		normalizeVectors();
	}
	
	// Pitch by rotating lookat and up vectors about the rotation vector
	// Update rotation vector
	void pitch(GLfloat angle) {
		GLfloat * look_res = ops.rotate(look, angle, rotv);
		GLfloat * up_res = ops.rotate(up, angle, rotv);
		look[0] = look_res[0]; look[1] = look_res[1]; look[2] = look_res[2];
		up[0] = up_res[0]; up[1] = up_res[1]; up[2] = up_res[2];
		rotv = ops.cross(look, up);
		normalizeVectors();
	}
	
	// Roll by rotating up and rotation vectors about the lookat vector
	// Update lookat vector
	void roll(GLfloat angle) {
		GLfloat * up_res = ops.rotate(up, angle, look);
		GLfloat * rotv_res = ops.rotate(rotv, angle, look);
		up[0] = up_res[0]; up[1] = up_res[1]; up[2] = up_res[2];
		rotv[0] = rotv_res[0]; rotv[1] = rotv_res[1]; rotv[2] = rotv_res[2];
		GLfloat * look_res = ops.cross(up, rotv);
		look[0] = look_res[0]; look[1] = look_res[1]; look[2] = look_res[2];
		normalizeVectors();
	}
	
	// Normalizes the lookat, up, and rotation vectors
	void normalizeVectors() {
		GLfloat * look_res = ops.normalize(look);
		GLfloat * up_res = ops.normalize(up);
		GLfloat * rotv_res = ops.normalize(rotv);	
		look[0] = look_res[0]; look[1] = look_res[1]; look[2] = look_res[2];
		up[0] = up_res[0]; up[1] = up_res[1]; up[2] = up_res[2];
		rotv[0] = rotv_res[0]; rotv[1] = rotv_res[1]; rotv[2] = rotv_res[2];
	}
};

int seed(float x, float y) {
    static int a = 1588635695, b = 1117695901;
	int xi = *(int *)&x;
	int yi = *(int *)&y;
    return ((xi * a) % b) - ((yi * b) % a);
}

// Generates terrain. Not my code.
void mountain(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2, float s)
{
	float x01,y01,z01,x12,y12,z12,x20,y20,z20;
	
	if (s < polysize) {
		x01 = x1 - x0;
		y01 = y1 - y0;
		z01 = z1 - z0;
		
		x12 = x2 - x1;
		y12 = y2 - y1;
		z12 = z2 - z1;
		
		x20 = x0 - x2;
		y20 = y0 - y2;
		z20 = z0 - z2;
		
		float nx = y01*(-z20) - (-y20)*z01;
		float ny = z01*(-x20) - (-z20)*x01;
		float nz = x01*(-y20) - (-x20)*y01;
		
		float den = sqrt(nx*nx + ny*ny + nz*nz);
		
		if (den > 0.0) {
			nx /= den;
			ny /= den;
			nz /= den;
		}
		
		glNormal3f(nx,ny,nz);
		glBegin(GL_TRIANGLES);
		glVertex3f(x0,y0,z0);
		glVertex3f(x1,y1,z1);
		glVertex3f(x2,y2,z2);
		glEnd();
		
		return;
	}
	
	x01 = 0.5*(x0 + x1);
	y01 = 0.5*(y0 + y1);
	z01 = 0.5*(z0 + z1);
	
	x12 = 0.5*(x1 + x2);
	y12 = 0.5*(y1 + y2);
	z12 = 0.5*(z1 + z2);
	
	x20 = 0.5*(x2 + x0);
	y20 = 0.5*(y2 + y0);
	z20 = 0.5*(z2 + z0);
	
	s *= 0.5;
	
	srand(seed(x01,y01));
	z01 += 0.3*s*(2.0*((float)rand()/(float)RAND_MAX) - 1.0);
	srand(seed(x12,y12));
	z12 += 0.3*s*(2.0*((float)rand()/(float)RAND_MAX) - 1.0);
	srand(seed(x20,y20));
	z20 += 0.3*s*(2.0*((float)rand()/(float)RAND_MAX) - 1.0);
	
	mountain(x0,y0,z0,x01,y01,z01,x20,y20,z20,s);
	mountain(x1,y1,z1,x12,y12,z12,x01,y01,z01,s);
	mountain(x2,y2,z2,x20,y20,z20,x12,y12,z12,s);
	mountain(x01,y01,z01,x12,y12,z12,x20,y20,z20,s);
}

void init(void) 
{
	GLfloat white[] = {1.0,1.0,1.0,1.0};
	GLfloat lpos[] = {0.0,1.0,0.0,0.0};
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	
	glClearColor (0.5, 0.5, 1.0, 0.0);
	/* glShadeModel (GL_FLAT); */
	glEnable(GL_DEPTH_TEST);
	
	sealevel = 0.0;
	polysize = 0.01;
}

Camera cam;
GLfloat speed = .0004;

void display(void)
{	
	GLfloat tanamb[] = {0.2,0.15,0.1,1.0};
	GLfloat tandiff[] = {0.4,0.3,0.2,1.0};
	
	GLfloat seaamb[] = {0.0,0.0,0.2,1.0};
	GLfloat seadiff[] = {0.0,0.0,0.8,1.0};
	GLfloat seaspec[] = {0.5,0.5,1.0,1.0};
	
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f (1.0, 1.0, 1.0);
	
	// Following five lines of code is my own. Moves the camera forward and updates its lookat vector.
	glLoadIdentity ();
	cam.moveForward(speed);
	gluLookAt (cam.eye[0], cam.eye[1], cam.eye[2],
			 cam.eye[0] + cam.look[0], cam.eye[1] + cam.look[1], cam.eye[2] + cam.look[2],
			 cam.up[0], cam.up[1], cam.up[2]);
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, tanamb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, tandiff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tandiff);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);
	
	mountain(0.0,0.0,0.0, 1.0,0.0,0.0, 0.0,1.0,0.0, 1.0);
	mountain(1.0,1.0,0.0, 0.0,1.0,0.0, 1.0,0.0,0.0, 1.0);
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, seaamb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, seadiff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, seaspec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);
	
	glNormal3f(0.0,0.0,1.0);
	glBegin(GL_QUADS);
	glVertex3f(0.0,0.0,sealevel);
	glVertex3f(1.0,0.0,sealevel);
	glVertex3f(1.0,1.0,sealevel);
	glVertex3f(0.0,1.0,sealevel);
	glEnd();
	
	glutSwapBuffers();
	glFlush ();
		
	glutPostRedisplay();
}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(90.0,1.0,0.01,10.0);
	glMatrixMode (GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case '-':
			sealevel -= 0.01;
			break;
		case '+':
		case '=':
			sealevel += 0.01;
			break;
		case 'f':
			polysize *= 0.5;
			break;
		case 'c':
			polysize *= 2.0;
			break;
		case 27:
			exit(0);
			break;
		case 'w':
			cam.pitch(0.2);
			break;
		case 's':
			cam.pitch(-0.2);
			break;
		case 'a':
			cam.roll(-0.2);
			break;
		case 'd':
			cam.roll(0.2);
			break;
		case 'x':
			speed += .0001;
			break;
		case 'z':
			speed -= .0001;
			break;
	}
}

void arrowkeys(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_UP:
			cam.pitch(0.2);
			break;
		case GLUT_KEY_DOWN:
			cam.pitch(-0.2);
			break;
		case GLUT_KEY_LEFT:
			cam.roll(-0.2);
			break;
		case GLUT_KEY_RIGHT:
			cam.roll(0.2);
			break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (500, 500); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow (argv[0]);
	init ();
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(arrowkeys);
	glutMainLoop();
	return 0;
}