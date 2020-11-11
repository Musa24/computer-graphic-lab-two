#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

#include"Draw.h"

bool textureMode = true;
bool lightMode = true;


int alpha = 0;

bool texChange = true;


class CustomCamera : public Camera
{
public:

	double camDististance;
	
	double fi1, fi2;
	CustomCamera()
	{
		camDististance = 15;
		fi1 = 1;
		fi2 = 1;
	}

	


	void SetUpCamera()
	{
		
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDististance *cos(fi2)*cos(fi1),
			camDististance *cos(fi2)*sin(fi1),
			camDististance *sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   



class CustomLight : public Light
{
public:
	CustomLight()
	{
		
		pos = Vector3(1, 1, 3);
	}

	
	
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0, 1, 0, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light; 




int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	

	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDististance <= 1)
		return;
	if (delta > 0 && camera.camDististance >= 100)
		return;

	camera.camDististance += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDististance = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}


	if (key == 'W') {
		alpha++;
		if (alpha == 3) {
			alpha = 0;
		}
	}
	
	if (key == 'Q') {
		texChange = !texChange;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}

vector<string> nametex_str = { "texture2.bmp", "Rabi.bmp" };
GLuint texId[2];


void initRender(OpenGL *ogl)
{
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	
	glEnable(GL_TEXTURE_2D);
	

	
	RGBTRIPLE *texarray;

	
	char *texCharArray;
	int texW, texH;

	for (int i = 0; i < nametex_str.size(); i++) {
		const char* nametex = nametex_str[i].c_str();
		OpenGL::LoadBMP(nametex, &texW, &texH, &texarray);
		OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

		
		glGenTextures(1, &texId[i]);
		
		glBindTexture(GL_TEXTURE_2D, texId[i]);

		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

		
		free(texCharArray);
		free(texarray);

		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}


	
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	
	glEnable(GL_NORMALIZE);

	
	glEnable(GL_LINE_SMOOTH); 


	

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL *ogl)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	

	GLfloat amb[] = { 167/256, 66/256, 245/256, 0 };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. }; 
	GLfloat sh = 0.1f * 256; 

	
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); 
	
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	
	glShadeModel(GL_SMOOTH);

	Draw::DrawABCDEFG(alpha, texChange, texId);
	
	
   
	
	glMatrixMode(GL_PROJECTION);	
	                              
	glPushMatrix();   			    
	glLoadIdentity();	 
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	
	
	glMatrixMode(GL_MODELVIEW);		
	glPushMatrix();			 
	glLoadIdentity();		 
	
	glDisable(GL_LIGHTING);
	
	
	
	GuiTextRectangle rec;		  
	rec.setSize(300, 200);
	rec.setPosition(10, ogl->getHeight() - 200 - 10);
	
	
	std::stringstream ss;
	ss << "Q - Change texture" << std::endl;
	ss << "W -  Change the type of speed" << std::endl;
	ss << "T - ON/OFF texture" << std::endl;
	ss << "L - ON/OFF illumination" << std::endl;
	ss << "F - Light from the camera" << std::endl;
	ss << "G - Move the light on the horizontal" << std::endl;
	ss << "G +  - Move the light on the Vertical" << std::endl;
	ss << "Light Coord: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Cam Coords: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Cam parameter: R="  << camera.camDististance << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();
	
	glMatrixMode(GL_PROJECTION);	  
	glPopMatrix();
	


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}