/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include "myTeapot.h"
#include "Kostka.h"
#include "cube.h"
#include "cylinder.h"
#include "wal1.h"
#include "wal2.h"
#include "piston.h"
#include "wajcha.h"
#include "blok.h"
#include "head.h"
#include "walek.h"
#include "valve.h"
#include "pokrywka.h"
#include "pas.h"
#include "malyzeb.h"
#include "duzyzeb.h"

float speed_x=0;
float speed_y=0;
float speed_z=0;
float zoom = -15;
float speed_zoom = 0;
float aspectRatio=1;
ShaderProgram *sp;

bool blok = 1;
bool head = 1;
bool pokrywka = 1;

GLuint tex0;
GLuint tex1;
GLuint tex2;
GLuint tex3;
GLuint tex4;
GLuint tex5;
GLuint tex6;
GLuint tex7;
GLuint tex8;

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
    if (action==GLFW_PRESS) {
        if (key==GLFW_KEY_LEFT) speed_x=-PI/2;
        if (key==GLFW_KEY_SPACE) speed_z=((int)speed_z+2)%4;
        if (key==GLFW_KEY_RIGHT) speed_x=PI/2;
        if (key==GLFW_KEY_UP) speed_y=PI/2;
        if (key==GLFW_KEY_DOWN) speed_y=-PI/2;
        if (key==GLFW_KEY_KP_ADD) speed_zoom=5;
        if (key==GLFW_KEY_KP_SUBTRACT) speed_zoom=-5;
        if (key==GLFW_KEY_Q) blok = (blok + 1) % 2;
        if (key==GLFW_KEY_W) head = (head + 1) % 2;
        if (key==GLFW_KEY_E) pokrywka = (pokrywka + 1) % 2;
    }
    if (action==GLFW_RELEASE) {
        if (key==GLFW_KEY_LEFT) speed_x=0;
        if (key==GLFW_KEY_RIGHT) speed_x=0;
        if (key==GLFW_KEY_UP) speed_y=0;
        if (key==GLFW_KEY_DOWN) speed_y=0;
        if (key==GLFW_KEY_KP_ADD) speed_zoom=0;
        if (key==GLFW_KEY_KP_SUBTRACT) speed_zoom=0;
    }
}

void windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}

GLuint readTexture(const char* filename) {
  GLuint tex;
  glActiveTexture(GL_TEXTURE0);

  //Read into computers memory
  std::vector<unsigned char> image;   //Allocate data structure for the image
  unsigned width, height;   //Variables, which will be set to image dimensions
  //Read image
  unsigned error = lodepng::decode(image, width, height, filename);

  //Import to graphics card memory
  glGenTextures(1,&tex); //Initialize a handle
  glBindTexture(GL_TEXTURE_2D, tex); //Activate the handle
  //Copy image from computers memory to graphics cards memory
  glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image.data());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return tex;
}

void initOpenGLProgram(GLFWwindow* window) {
	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);
	sp=new ShaderProgram("vertex.glsl",NULL,"fragment.glsl");

    tex0=readTexture("metal.png");
    tex1=readTexture("sky.png");
    tex2=readTexture("metal_spec.png");
    tex3=readTexture("metal_jajca.png");
    tex4=readTexture("amelinium.png");
    tex5=readTexture("szary.png");
    tex6=readTexture("matowy.png");
    tex7=readTexture("czerwony.png");
    tex8=readTexture("czarny.png");
}

void freeOpenGLProgram(GLFWwindow* window) {
    glDeleteTextures(1,&tex0);
    glDeleteTextures(1,&tex1);
    glDeleteTextures(1,&tex2);
    glDeleteTextures(1,&tex3);
    glDeleteTextures(1,&tex4);
    glDeleteTextures(1,&tex5);
    glDeleteTextures(1,&tex6);
    glDeleteTextures(1,&tex7);
    glDeleteTextures(1,&tex8);

    delete sp;
}

glm::mat4 drawSth(glm::mat4 M , float *verts , float *normals , float *texCoords ,\
                  unsigned int vertexCount , GLuint tex0 , GLuint tex1 , GLuint tex2){

    glUniform1i(sp->u("textureMap0"),0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,tex0);

    glUniform1i(sp->u("textureMap1"),1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,tex1);

    glUniform1i(sp->u("textureMap2"),2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,tex2);

	glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M));

    glEnableVertexAttribArray(sp->a("vertex"));
    glVertexAttribPointer(sp->a("vertex"),4,GL_FLOAT,false,0,verts);

    glEnableVertexAttribArray(sp->a("normal"));
    glVertexAttribPointer(sp->a("normal"),4,GL_FLOAT,false,0,normals);

    glEnableVertexAttribArray(sp->a("texCoord0"));
    glVertexAttribPointer(sp->a("texCoord0"),2,GL_FLOAT,false,0,texCoords);

    glDrawArrays(GL_TRIANGLES,0,vertexCount);

    glDisableVertexAttribArray(sp->a("vertex"));
    glDisableVertexAttribArray(sp->a("normal"));
    glDisableVertexAttribArray(sp->a("texCoord0"));

    return M;
}

double r = 0.1 , R = 0.4;
double f = atan(r*sqrt(2)/(R+r*sqrt(2)));
double h(double fi) {return sqrt(pow(r,2) + pow(R+r,2) - 2*r*(R+r)*cos(fi-PI/2));}

double wychylenie(double fi){
    fi /= 2;
    if(fi < 5*PI/4)
        return R;
    if(fi < 3*PI/2-f){
        double a1 = (R-h(3*PI/2-f))/(f-PI/4);
        return a1*fi + (R-5*PI*a1/4);
    }
    if(fi < 3*PI/2+f)
        return h(fi);
    if(fi < 7*PI/4){
        double a2 = (h(3*PI/2+f)-R)/(f-PI/4);
        return a2*fi + (R-7*PI*a2/4);
    }
    return R;
}

float wych(float fi){
    if(fi > 4 * PI) fi -= 4*PI;
    return -1*(float)wychylenie((double)fi);
}

//Drawing procedure
void drawScene(GLFWwindow* window,float angle_x,float angle_y,float angle_z) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V=glm::lookAt(
        glm::vec3(0.0f,5.0f,zoom),
        glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3(0.0f,1.0f,0.0f)); //compute view matrix
    glm::mat4 P=glm::perspective(50.0f*PI/180.0f, aspectRatio, 1.0f, 50.0f); //compute projection matrix

    sp->use();

    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));

    //Light coordinates in the world space
    glUniform4f(sp->u("lp"),0,8,-2,1);
    glUniform4f(sp->u("lp2"),0,0,-8,1);



    glm::mat4 M=glm::mat4(1.0f);
	M=glm::rotate(M,angle_y,glm::vec3(1.0f,0.0f,0.0f));
	M=glm::rotate(M,angle_x,glm::vec3(0.0f,1.0f,0.0f));
    M=glm::translate(M,glm::vec3(0.0f,-4.0f,0.0f));

    glm::mat4 M2=glm::rotate(M,angle_z,glm::vec3(0.0f,0.0f,1.0f));

    drawSth(glm::translate(M2,glm::vec3(0.0f,0.0f,-4.0f)) , malyzebVertices , malyzebNormals , malyzebTexCoords , malyzebVertexCount , tex4 , tex1 , tex3);

    glm::mat4 Mp=glm::translate(M,glm::vec3(0.0f,0.0f,-4.0f));
    drawSth(Mp , pasVertices , pasNormals , pasTexCoords , pasVertexCount , tex8 , tex5 , tex3);

    glm::mat4 Mv = glm::translate(M , glm::vec3(0.0f,6.6f,0.5f));
    drawSth(glm::translate(Mv,glm::vec3(0.0f,wych(angle_z+5*PI/2),0.0f)) , valveVertices , valveNormals , valveTexCoords , valveVertexCount , tex4 , tex1 , tex3);

    Mv = glm::translate(Mv , glm::vec3(0.0f,0.0f,1.8f));
    drawSth(glm::translate(Mv,glm::vec3(0.0f,wych(angle_z+3*PI/2),0.0f)) , valveVertices , valveNormals , valveTexCoords , valveVertexCount , tex4 , tex1 , tex3);

    Mv = glm::translate(Mv , glm::vec3(0.0f,0.0f,0.8f));
    drawSth(glm::translate(Mv,glm::vec3(0.0f,wych(angle_z+5*PI/2),0.0f)) , valveVertices , valveNormals , valveTexCoords , valveVertexCount , tex4 , tex1 , tex3);

    Mv = glm::translate(Mv , glm::vec3(0.0f,0.0f,-1.8f));
    drawSth(glm::translate(Mv,glm::vec3(0.0f,wych(angle_z+7*PI/2),0.0f)) , valveVertices , valveNormals , valveTexCoords , valveVertexCount , tex4 , tex1 , tex3);

    Mv = glm::translate(Mv , glm::vec3(0.0f,0.0f,-1.8f));
    drawSth(glm::translate(Mv,glm::vec3(0.0f,wych(angle_z+3*PI/2),0.0f)) , valveVertices , valveNormals , valveTexCoords , valveVertexCount , tex4 , tex1 , tex3);

    Mv = glm::translate(Mv , glm::vec3(0.0f,0.0f,-1.8f));
    drawSth(glm::translate(Mv,glm::vec3(0.0f,wych(angle_z+9*PI/2),0.0f)) , valveVertices , valveNormals , valveTexCoords , valveVertexCount , tex4 , tex1 , tex3);

    Mv = glm::translate(Mv , glm::vec3(0.0f,0.0f,-0.8f));
    drawSth(glm::translate(Mv,glm::vec3(0.0f,wych(angle_z+7*PI/2),0.0f)) , valveVertices , valveNormals , valveTexCoords , valveVertexCount , tex4 , tex1 , tex3);

    Mv = glm::translate(Mv , glm::vec3(0.0f,0.0f,1.8f));
    drawSth(glm::translate(Mv,glm::vec3(0.0f,wych(angle_z+9*PI/2),0.0f)) , valveVertices , valveNormals , valveTexCoords , valveVertexCount , tex4 , tex1 , tex3);

    if(blok)
        drawSth(M , blokVertices , blokNormals , blokTexCoords , blokVertexCount , tex5 , tex5 , tex6);

    if(head)
        drawSth(M , headVertices , headNormals , headTexCoords , headVertexCount , tex4 , tex5 , tex5);

    if(pokrywka)
        drawSth(M , pokrywkaVertices , pokrywkaNormals , pokrywkaTexCoords , pokrywkaVertexCount , tex7 , tex5 , tex3);

    glm::mat4 Mw = glm::translate(M , glm::vec3(0.0f,6.9f,0.0f));
    Mw = glm::rotate(Mw , angle_z/2-PI/4 , glm::vec3(0.0f,0.0f,1.0f));

    drawSth(Mw , walekVertices , walekNormals , walekTexCoords , walekVertexCount , tex4 , tex1 , tex3);

    drawSth(glm::translate(Mw,glm::vec3(0.0f,0.0f,-4.0f)) , duzyzebVertices , duzyzebNormals , duzyzebTexCoords , duzyzebVertexCount , tex4 , tex1 , tex3);

    drawSth(M2 , wal1Vertices , wal1Normals , wal1TexCoords , wal1VertexCount , tex4 , tex1 , tex3);
    drawSth(M2 , wal2Vertices , wal2Normals , wal2TexCoords , wal2VertexCount , tex0 , tex1 , tex2);

    glm::mat4 M4=glm::translate(M,glm::vec3(cos(angle_z+PI/2)*0.95,sin(angle_z+PI/2)*0.95,0.9f));
    if(sin(angle_z)>0)
        M4=glm::rotate(M4,(float)asin(sqrt(1-pow(sin(angle_z)*0.95,2)/16))-PI/2,glm::vec3(0.0f,0.0f,1.0f));
    else
        M4=glm::rotate(M4,-((float)asin(sqrt(1-pow(sin(angle_z)*0.95,2)/16))-PI/2),glm::vec3(0.0f,0.0f,1.0f));

    drawSth(M4 , wajchaVertices , wajchaNormals , wajchaTexCoords , wajchaVertexCount , tex0 , tex1 , tex2);
    M4=glm::translate(M4,glm::vec3(0.0f,0.0f,-1.8f));
    drawSth(M4 , wajchaVertices , wajchaNormals , wajchaTexCoords , wajchaVertexCount , tex0 , tex1 , tex2);

    glm::mat4 M5=glm::translate(M,glm::vec3(cos(angle_z-PI/2)*0.95,sin(angle_z-PI/2)*0.95,2.7f));
    if(sin(angle_z+PI)>0)
        M5=glm::rotate(M5,(float)asin(sqrt(1-pow(sin(angle_z+PI)*0.95,2)/16))-PI/2,glm::vec3(0.0f,0.0f,1.0f));
    else
        M5=glm::rotate(M5,-((float)asin(sqrt(1-pow(sin(angle_z+PI)*0.95,2)/16))-PI/2),glm::vec3(0.0f,0.0f,1.0f));

    drawSth(M5 , wajchaVertices , wajchaNormals , wajchaTexCoords , wajchaVertexCount , tex0 , tex1 , tex2);
    M5=glm::translate(M5,glm::vec3(0.0f,0.0f,-5.4f));
    drawSth(M5 , wajchaVertices , wajchaNormals , wajchaTexCoords , wajchaVertexCount , tex0 , tex1 , tex2);

    float z = sqrt(16-pow(0.95*sin(angle_z),2));
    glm::mat4 M3=glm::translate(M,glm::vec3(0.0f,z+sin(angle_z+PI/2)*0.95,0.9f));
    drawSth(M3 , pistonVertices , pistonNormals , pistonTexCoords , pistonVertexCount , tex4 , tex1 , tex3);
    M3=glm::translate(M3,glm::vec3(0.0f,0.0f,-1.8f));
    drawSth(M3 , pistonVertices , pistonNormals , pistonTexCoords , pistonVertexCount , tex4 , tex1 , tex3);

    z = sqrt(16-pow(0.95*sin(angle_z+PI),2));
    M=glm::translate(M,glm::vec3(0.0f,z+sin(angle_z-PI/2)*0.95,2.7f));
    drawSth(M , pistonVertices , pistonNormals , pistonTexCoords , pistonVertexCount , tex4 , tex1 , tex3);
    M=glm::translate(M,glm::vec3(0.0f,0.0f,-5.4f));
    drawSth(M , pistonVertices , pistonNormals , pistonTexCoords , pistonVertexCount , tex4 , tex1 , tex3);


    glfwSwapBuffers(window);
}

int main(void)
{
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		fprintf(stderr, "Can't initialize GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	GLenum err;
	if ((err=glewInit()) != GLEW_OK) {
		fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window);


	float angle_x=0;
	float angle_y=0;
	float angle_z=0;
	glfwSetTime(0);

	while (!glfwWindowShouldClose(window))
	{
        angle_x+=speed_x*glfwGetTime();
		angle_y+=speed_y*glfwGetTime();
		angle_z+=speed_z*glfwGetTime();
		if(angle_z > 4*PI) angle_z -= 4*PI;
		zoom += speed_zoom*glfwGetTime();
        glfwSetTime(0);
		drawScene(window,angle_x,angle_y,angle_z);
		glfwPollEvents();
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
