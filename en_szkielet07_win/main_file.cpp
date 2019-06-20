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

float speed_x=0; //angular speed in radians
float speed_y=0; //angular speed in radians
float speed_z=0; //angular speed in radians
float zoom = -15;
float speed_zoom = 0;
float aspectRatio=1;
ShaderProgram *sp; //Pointer to the shader program

GLuint tex0;
GLuint tex1;

//Error processing callback procedure
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


//Function for reading texture files
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

//Initialization code procedure
void initOpenGLProgram(GLFWwindow* window) {
	//************Place any code here that needs to be executed once, at the program start************
	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);
	sp=new ShaderProgram("vertex.glsl",NULL,"fragment.glsl");

    tex0=readTexture("metal.png");
    tex1=readTexture("sky.png");
}

//Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Place any code here that needs to be executed once, after the main loop ends************

    glDeleteTextures(1,&tex0);
    glDeleteTextures(1,&tex1);

    delete sp;
}

glm::mat4 drawSth(glm::mat4 M , float *verts , float *normals , \
                   float *texCoords , unsigned int vertexCount){

	glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M));

    glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
    glVertexAttribPointer(sp->a("vertex"),4,GL_FLOAT,false,0,verts); //Specify source of the data for the attribute vertex

    glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute normal
    glVertexAttribPointer(sp->a("normal"),4,GL_FLOAT,false,0,normals); //Specify source of the data for the attribute normal

    glEnableVertexAttribArray(sp->a("texCoord0")); //Enable sending data to the attribute color
    glVertexAttribPointer(sp->a("texCoord0"),2,GL_FLOAT,false,0,texCoords); //Specify source of the data for the attribute color

    glDrawArrays(GL_TRIANGLES,0,vertexCount); //Draw the object

    glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
    glDisableVertexAttribArray(sp->a("normal")); //Disable sending data to the attribute normal
    glDisableVertexAttribArray(sp->a("texCoord0")); //Disable sending data to the attribute color

    return M;
}


//Drawing procedure
void drawScene(GLFWwindow* window,float angle_x,float angle_y,float angle_z) {
	//************Place any code here that draws something inside the window******************l

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V=glm::lookAt(
        glm::vec3(0.0f,5.0f,zoom),
        glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3(0.0f,1.0f,0.0f)); //compute view matrix
    glm::mat4 P=glm::perspective(50.0f*PI/180.0f, aspectRatio, 1.0f, 50.0f); //compute projection matrix


	//Teapot
	/*float *verts=myTeapotVertices;
	float *normals=myTeapotVertexNormals;
	float *texCoords=myTeapotTexCoords;
	unsigned int vertexCount=myTeapotVertexCount;*/


    sp->use();//activate shading program
    //Send parameters to graphics card
    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));

    glUniform4f(sp->u("lp"),0,8,-2,1); //Light coordinates in the world space
    glUniform4f(sp->u("lp2"),0,0,-8,1); //Light coordinates in the world space

    glUniform1i(sp->u("textureMap0"),0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,tex0);

    glUniform1i(sp->u("textureMap1"),1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,tex1);

    glm::mat4 M=glm::mat4(1.0f);
	M=glm::rotate(M,angle_y,glm::vec3(1.0f,0.0f,0.0f)); //Compute model matrix
	M=glm::rotate(M,angle_x,glm::vec3(0.0f,1.0f,0.0f)); //Compute model matrix
	 //Compute model matrix

    M=glm::translate(M,glm::vec3(0.0f,-4.0f,0.0f));
    //M2=glm::translate(M2,glm::vec3(0.0f,-4.0f,0.0f));
    glm::mat4 M2=glm::rotate(M,angle_z,glm::vec3(0.0f,0.0f,1.0f));

    drawSth(M2 , wal1Vertices , wal1Normals , wal1TexCoords , wal1VertexCount);
    drawSth(M2 , wal2Vertices , wal2Normals , wal2TexCoords , wal2VertexCount);

    glm::mat4 M4=glm::translate(M,glm::vec3(cos(angle_z+PI/2)*0.95,sin(angle_z+PI/2)*0.95,0.9f));
    if(sin(angle_z)>0)
        M4=glm::rotate(M4,(float)asin(sqrt(1-pow(sin(angle_z)*0.95,2)/16))-PI/2,glm::vec3(0.0f,0.0f,1.0f));
    else
        M4=glm::rotate(M4,-((float)asin(sqrt(1-pow(sin(angle_z)*0.95,2)/16))-PI/2),glm::vec3(0.0f,0.0f,1.0f));

    drawSth(M4 , wajchaVertices , wajchaNormals , wajchaTexCoords , wajchaVertexCount);
    M4=glm::translate(M4,glm::vec3(0.0f,0.0f,-1.8f));
    drawSth(M4 , wajchaVertices , wajchaNormals , wajchaTexCoords , wajchaVertexCount);

    glm::mat4 M5=glm::translate(M,glm::vec3(cos(angle_z-PI/2)*0.95,sin(angle_z-PI/2)*0.95,2.7f));
    if(sin(angle_z+PI)>0)
        M5=glm::rotate(M5,(float)asin(sqrt(1-pow(sin(angle_z+PI)*0.95,2)/16))-PI/2,glm::vec3(0.0f,0.0f,1.0f));
    else
        M5=glm::rotate(M5,-((float)asin(sqrt(1-pow(sin(angle_z+PI)*0.95,2)/16))-PI/2),glm::vec3(0.0f,0.0f,1.0f));

    drawSth(M5 , wajchaVertices , wajchaNormals , wajchaTexCoords , wajchaVertexCount);
    M5=glm::translate(M5,glm::vec3(0.0f,0.0f,-5.4f));
    drawSth(M5 , wajchaVertices , wajchaNormals , wajchaTexCoords , wajchaVertexCount);

    float z = sqrt(16-pow(0.95*sin(angle_z),2));
    glm::mat4 M3=glm::translate(M,glm::vec3(0.0f,z+sin(angle_z+PI/2)*0.95,0.9f));
    drawSth(M3 , pistonVertices , pistonNormals , pistonTexCoords , pistonVertexCount);
    M3=glm::translate(M3,glm::vec3(0.0f,0.0f,-1.8f));
    drawSth(M3 , pistonVertices , pistonNormals , pistonTexCoords , pistonVertexCount);

    z = sqrt(16-pow(0.95*sin(angle_z+PI),2));
    M=glm::translate(M,glm::vec3(0.0f,z+sin(angle_z-PI/2)*0.95,2.7f));
    drawSth(M , pistonVertices , pistonNormals , pistonTexCoords , pistonVertexCount);
    M=glm::translate(M,glm::vec3(0.0f,0.0f,-5.4f));
    drawSth(M , pistonVertices , pistonNormals , pistonTexCoords , pistonVertexCount);

    glfwSwapBuffers(window); //Copy back buffer to front buffer
}

int main(void)
{
	GLFWwindow* window; //Pointer to object that represents the application window

	glfwSetErrorCallback(error_callback);//Register error processing callback procedure

	if (!glfwInit()) { //Initialize GLFW library
		fprintf(stderr, "Can't initialize GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Create a window 500pxx500px titled "OpenGL" and an OpenGL context associated with it.

	if (!window) //If no window is opened then close the program
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Since this moment OpenGL context corresponding to the window is active and all OpenGL calls will refer to this context.
	glfwSwapInterval(1); //During vsync wait for the first refresh

	GLenum err;
	if ((err=glewInit()) != GLEW_OK) { //Initialize GLEW library
		fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Call initialization procedure


	float angle_x=0; //current rotation angle of the object, x axis
	float angle_y=0; //current rotation angle of the object, y axis
	float angle_z=0; //current rotation angle of the object, y axis
	glfwSetTime(0); //Zero the timer
	//Main application loop
	while (!glfwWindowShouldClose(window)) //As long as the window shouldnt be closed yet...
	{
        angle_x+=speed_x*glfwGetTime(); //Add angle by which the object was rotated in the previous iteration
		angle_y+=speed_y*glfwGetTime(); //Add angle by which the object was rotated in the previous iteration
		angle_z+=speed_z*glfwGetTime(); //Add angle by which the object was rotated in the previous iteration
		zoom += speed_zoom*glfwGetTime();
        glfwSetTime(0); //Zero the timer
		drawScene(window,angle_x,angle_y,angle_z); //Execute drawing procedure
		glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
