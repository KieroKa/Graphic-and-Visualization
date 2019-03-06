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
#include <iostream>
#include <string.h> //m
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "objloader.h"

//


using namespace glm;

const int liczba_obiektow = 7;

float speed_x = 0; // [radians/s]
float speed_y = 0; // [radians/s]

float aspect=1; //Ratio of width to height


    float angle_x = 3.14; //Object rotation angle
	float angle_y = 1.57; //Object rotation angle
	float angle_z = 3.14;

	float zebatki_rotacja = 0;
	float szybkosc_zebatka = 1;

	float godzina_rotacja = 0;
	float minuta_rotacja = 0;
	float sekunda_rotacja = 0;
	float wahadlo_rotacja = 0;

    float szybkosc_sekunda = 1;
    float szybkosc_minuta = szybkosc_sekunda/60;
    float szybkosc_godzina = szybkosc_minuta/12;
    float szybkosc_wahadla = szybkosc_sekunda/2;

    float granica_wahadla = 0.35f;
    int kierunek = 1;

    float zoom = 0.0f;


//Shader program object
//ShaderProgram *shaderProgram;
ShaderProgram* shaderPrograms[liczba_obiektow];
//VAO and VBO handles
GLuint vao[liczba_obiektow];
GLuint bufVertices; //handle for VBO buffer which stores vertex coordinates
GLuint bufColors;  //handle for VBO buffer which stores vertex colors
GLuint bufNormals; //handle for VBO buffer which stores vertex normals
GLuint bufTexCoords; //handle for VBO buffer which stores texturing coordinates

GLuint tex[liczba_obiektow];


//DO WCZYTANIA

std::vector < glm::vec4 >  vertices;
std::vector < glm::vec2 >  texCoords;
std::vector < glm::vec4 >  normals;
std::vector < glm::vec4 >  colors;
int vertexCount[liczba_obiektow];









//Error handling procedure
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

//Key event processing procedure
void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) speed_y = -3.14;
		if (key == GLFW_KEY_RIGHT) speed_y = 3.14;
		if (key == GLFW_KEY_UP) speed_x = -3.14;
		if (key == GLFW_KEY_DOWN) speed_x = 3.14;
	}


	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) speed_y = 0;
		if (key == GLFW_KEY_RIGHT) speed_y = 0;
		if (key == GLFW_KEY_UP) speed_x = 0;
		if (key == GLFW_KEY_DOWN) speed_x = 0;

		if (key == GLFW_KEY_PAGE_UP) zoom += 0.2f;       //zoom up
		if (key == GLFW_KEY_PAGE_DOWN) zoom -= 0.2f;   //zoom down
		if (key == GLFW_KEY_HOME) zoom = 0.0f;          //zoom reset



		if (key == GLFW_KEY_KP_ADD) {               //speed up
                 szybkosc_sekunda *= 1.1;
                 szybkosc_zebatka= szybkosc_sekunda;
     szybkosc_minuta = szybkosc_sekunda/60;
     szybkosc_godzina = szybkosc_minuta/12;
     szybkosc_wahadla = szybkosc_sekunda/2;
		}
        if (key == GLFW_KEY_KP_SUBTRACT){
                             szybkosc_sekunda *= 0.9;   //speed down
                 szybkosc_zebatka= szybkosc_sekunda;
     szybkosc_minuta = szybkosc_sekunda/60;
     szybkosc_godzina = szybkosc_minuta/12;
     szybkosc_wahadla = szybkosc_sekunda/2;
        }
        if(key == GLFW_KEY_KP_MULTIPLY){
                             szybkosc_sekunda = 1;      //speed reset
                             szybkosc_zebatka = 1;
     szybkosc_minuta = szybkosc_sekunda/60;
     szybkosc_godzina = szybkosc_minuta/12;
     szybkosc_wahadla = szybkosc_sekunda/2;
        }

		if (key == GLFW_KEY_R) {                       //reset all
     angle_x = 3.14; //Object rotation angle
	 angle_y = 1.57; //Object rotation angle
	 angle_z = 3.14;

	 godzina_rotacja = 0;
	 minuta_rotacja = 0;
	 sekunda_rotacja = 0;
	 wahadlo_rotacja = 0;

     szybkosc_sekunda = 1;
     szybkosc_minuta = szybkosc_sekunda/60;
     szybkosc_godzina = szybkosc_minuta/12;
     szybkosc_wahadla = szybkosc_sekunda/2;

	 zebatki_rotacja = 0;
	 szybkosc_zebatka = szybkosc_sekunda;


     granica_wahadla = 0.35f;
     kierunek = 1;
     zoom = 0.0f;

	glfwSetTime(0); //Zero time counter
	}

	}
}

//Window resize event processing procedure
void windowResize(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height); //Window coordinates
    if (height!=0) {
        aspect=(float)width/(float)height; //Compute window size aspect ratio
    } else {
        aspect=1;
    }
}


//Creates a VBO buffer from an array
GLuint makeBuffer4(std::vector<glm::vec4> data, int vertexCount, int vertexSize) {
	GLuint handle;

	glGenBuffers(1,&handle);//Generate handle for VBO buffer
	glBindBuffer(GL_ARRAY_BUFFER,handle);  //Active the handle
	glBufferData(GL_ARRAY_BUFFER, vertexCount*vertexSize, &data[0], GL_STATIC_DRAW);//Copy the array to VBO

	return handle;
}
GLuint makeBuffer2(std::vector<glm::vec2> data, int vertexCount, int vertexSize) {
	GLuint handle;

	glGenBuffers(1,&handle);//Generate handle for VBO buffer
	glBindBuffer(GL_ARRAY_BUFFER,handle);  //Active the handle
	glBufferData(GL_ARRAY_BUFFER, vertexCount*vertexSize, &data[0], GL_STATIC_DRAW);//Copy the array to VBO

	return handle;
}

//Assigns VBO buffer handle to an attribute of a given name
void assignVBOtoAttribute(ShaderProgram *shaderProgram,const char* attributeName, GLuint bufVBO, int vertexSize) {
	GLuint location=shaderProgram->getAttribLocation(attributeName); //Get slot number for the attribute
	glBindBuffer(GL_ARRAY_BUFFER,bufVBO);  //Activate VBO handle
	glEnableVertexAttribArray(location); //Turn on using of an attribute of a number passed as an argument
	glVertexAttribPointer(location,vertexSize,GL_FLOAT, GL_FALSE, 0, NULL); //Data for the slot should be taken from the current VBO buffer
}


//Preparation for drawing of a single object
void prepareObject(ShaderProgram* shaderProgram, int obj_number) {  // obj_number od zera
	//Build VBO buffers with object data
	bufVertices=makeBuffer4(vertices, vertexCount[obj_number], sizeof(glm::vec4)); //VBO with vertex coordinates          // ZMIENIONE SIZEOF'y
	bufColors=makeBuffer4(colors, vertexCount[obj_number], sizeof(glm::vec4));//VBO with vertex colors
	bufNormals=makeBuffer4(normals, vertexCount[obj_number], sizeof(glm::vec4));//VBO with vertex normals
    bufTexCoords=makeBuffer2(texCoords, vertexCount[obj_number], sizeof(glm::vec2)); //VBO with texturing coordinates

	//Create VAO which associates VBO with attributes in shading program
	glGenVertexArrays(1,&vao[obj_number]); //Generate VAO handle and store it in the global variable

	glBindVertexArray(vao[obj_number]); //Activate newly created VAO

	assignVBOtoAttribute(shaderProgram,"vertex",bufVertices,4); //"vertex" refers to the declaration "in vec4 vertex;" in vertex shader
	assignVBOtoAttribute(shaderProgram,"color",bufColors,4); //"color" refers to the declaration "in vec4 color;" in vertex shader
	assignVBOtoAttribute(shaderProgram,"normal",bufNormals,4); //"normal" refers to the declaration "in vec4 normal;" w vertex shader
    assignVBOtoAttribute(shaderProgram,"texCoord0",bufTexCoords,2); //"texCoord0" refers to the declaration "in vec2 texCoord0;" w vertex shader

	glBindVertexArray(0); //Deactivate VAO
}

GLuint readTexture(char* filename) {
 GLuint tex;
 glActiveTexture(GL_TEXTURE0);

 //Load into computer's memory
 std::vector<unsigned char> image;   //Allocate a vector for image storage
 unsigned width, height;   //Variables for image size
 //Read image
 unsigned error = lodepng::decode(image, width, height, filename);

 //Import into graphics card's memory
 glGenTextures(1,&tex); //Initialize one handle
 glBindTexture(GL_TEXTURE_2D, tex); //Activate the handle
 //Import image into graphics card's memory associated with the handle
 glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
   GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image.data());

 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);

 return tex;
}

//Initialization procedure
void initOpenGLProgram(GLFWwindow* window) {
	//************Insert initialization code here************
	glClearColor(0, 0, 0, 1); //Clear the screen to black
	glEnable(GL_DEPTH_TEST); //Turn on Z-Buffer
	glfwSetKeyCallback(window, key_callback); //Register key event processing procedure
    glfwSetFramebufferSizeCallback(window,windowResize);


    for(int i=0;i<liczba_obiektow;i++)
	shaderPrograms[i]=new ShaderProgram("vshader.vert",NULL,"fshader.frag"); //Read, compile and link the shader program

	tex[0]=readTexture("tekstury/zegar_tekstura.png");
	tex[1]=readTexture("tekstury/metal.png");
    tex[2]=readTexture("tekstury/czarne.png");
    tex[3]=readTexture("tekstury/czarne.png");
    tex[4]=readTexture("tekstury/czarne.png");
    tex[5]=readTexture("tekstury/metal.png");
    tex[6]=readTexture("tekstury/metal.png");

	if(!loadOBJ(
         "modele/zegar.obj",
         vertices,
         texCoords,
         normals,
         colors,
         vertexCount[0]))
         std::cout<<"\nBlad czytania obiektu\n";

	prepareObject(shaderPrograms[0], 0);

	vertices.clear();
	texCoords.clear();
	normals.clear();
	colors.clear();


		if(!loadOBJ(
         "modele/wahadlo.obj",
         vertices,
         texCoords,
         normals,
         colors,
         vertexCount[1]))
         std::cout<<"\nBlad czytania obiektu\n";


	prepareObject(shaderPrograms[1], 1);

    vertices.clear();
	texCoords.clear();
	normals.clear();
	colors.clear();

	if(!loadOBJ(
         "modele/godzinowa.obj",
         vertices,
         texCoords,
         normals,
         colors,
         vertexCount[2]))
         std::cout<<"\nBlad czytania obiektu\n";

	prepareObject(shaderPrograms[2], 2);

	vertices.clear();
	texCoords.clear();
	normals.clear();
	colors.clear();

	if(!loadOBJ(
         "modele/minutowa.obj",
         vertices,
         texCoords,
         normals,
         colors,
         vertexCount[3]))
         std::cout<<"\nBlad czytania obiektu\n";

	prepareObject(shaderPrograms[3], 3);

	vertices.clear();
	texCoords.clear();
	normals.clear();
	colors.clear();

	if(!loadOBJ(
         "modele/sekundowa.obj",
         vertices,
         texCoords,
         normals,
         colors,
         vertexCount[4]))
         std::cout<<"\nBlad czytania obiektu\n";

	prepareObject(shaderPrograms[4], 4);

	vertices.clear();
	texCoords.clear();
	normals.clear();
	colors.clear();

	if(!loadOBJ(
         "modele/zebatka.obj",
         vertices,
         texCoords,
         normals,
         colors,
         vertexCount[5]))
         std::cout<<"\nBlad czytania obiektu\n";
    vertexCount[6]=vertexCount[5];
	prepareObject(shaderPrograms[5], 5);
	prepareObject(shaderPrograms[6], 6);

	vertices.clear();
	texCoords.clear();
	normals.clear();
	colors.clear();







}

//Freeing of resources
void freeOpenGLProgram() {    // Trzeba tu dodac usuwanie jak dodamy nowe modele
	 //Delete shader program

	for(int i=0;i<liczba_obiektow;i++) {
            glDeleteVertexArrays(1,&vao[i]);
            delete shaderPrograms[i];
            glDeleteTextures(1,&tex[i]);
	}
	//glDeleteVertexArrays(1,&vao); //Delete VAO
	//Delete VBOs
	glDeleteBuffers(1,&bufVertices);
	glDeleteBuffers(1,&bufColors);
	glDeleteBuffers(1,&bufNormals);
    glDeleteBuffers(1,&bufTexCoords);
    //Delete textures

}

void drawObject(int obj_n, ShaderProgram *shaderProgram, mat4 mP, mat4 mV, mat4 mM) {
	//Turn on the shading program that will be used for drawing.
	//While in this program it would be perfectly correct to execute this once in the initOpenGLProgram,
	//in most cases more than one shading program is used and hence, it should be switched between drawing of objects
	//while we render a single scene.
	shaderProgram->use();

	//Set uniform variables P,V and M in the vertex shader by assigning the appropriate matrices
	//In the lines below, expression:
	//  shaderProgram->getUniformLocation("P")
	//Retrieves the slot number corresponding to a uniform variable of a given name.
	//WARNING! "P" in the instruction above corresponds to the declaration "uniform mat4 P;" in the vertex shader,
	//while mP in glm::value_ptr(mP) corresponds to the argument "mat4 mP;" in THIS file.
	//The whole line below copies data from variable mP to the uniform variable P in the vertex shader. The rest of the instructions work similarly.
	glUniformMatrix4fv(shaderProgram->getUniformLocation("P"),1, false, glm::value_ptr(mP));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("V"),1, false, glm::value_ptr(mV));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("M"),1, false, glm::value_ptr(mM));
    glUniform1i(shaderProgram->getUniformLocation("textureMap0"),0); //Bind textureMap0 in fragment shader with texturing unit 0
	glUniform1i(shaderProgram->getUniformLocation("textureMap1"),1); //Bind textureMap1 in fragment shader with texturing unit 1

	//Bind texture from tex0 with 0th texturing unit
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,tex[obj_n]);
	//Bind texture from tex1 with 1st texturing unit
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,tex[obj_n]);

	//Activation of VAO and therefore making all associations of VBOs and attributes current
	glBindVertexArray(vao[obj_n]);

	//Drawing of an object
	glDrawArrays(GL_TRIANGLES,0,vertexCount[obj_n]);

	//Tidying up after ourselves (not needed if we use VAO for every object)
	glBindVertexArray(0);
}

//Procedure which draws the scene
void drawScene(GLFWwindow* window, float angle_x, float angle_y, float angle_z, float zebatki_rotacja, float godzina_rotacja, float minuta_rotacja, float sekunda_rotacja, float wahadlo_rotacja) {
	//************Place the drawing code here******************l

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //Clear color and depth buffers

	glm::mat4 P = glm::perspective(50 * PI / 180,aspect, 1.0f, 50.0f); //Compute projection matrix

	glm::mat4 V = glm::lookAt( //Compute view matrix
    glm::vec3(0.0f, 0.0f, -8.0f+zoom),
    glm::vec3(0.0f, 2.5f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));


	//Compute model matrix
	glm::mat4 M = glm::mat4(1.0f);
	M = glm::rotate(M, angle_x, glm::vec3(1, 0, 0));
	M = glm::rotate(M, angle_y, glm::vec3(0, 1, 0));
	M = glm::rotate(M, angle_z, glm::vec3(0, 0, 1));

	//Draw object
	drawObject(0,shaderPrograms[0],P,V,M);

	glm::mat4 M2=glm::translate(M, glm::vec3(0.8f,2.7f,0.0f));
    M2 = glm::rotate(M2, wahadlo_rotacja, glm::vec3(-1, 0, 0));


    drawObject(1,shaderPrograms[1],P,V,M2);

    glm::mat4 M3=glm::translate(M, glm::vec3(1.05f,3.85f,-0.02f));
    M3 = glm::rotate(M3, godzina_rotacja, glm::vec3(-1, 0, 0));

    drawObject(2,shaderPrograms[2],P,V,M3);

    glm::mat4 M4=glm::translate(M, glm::vec3(1.08f,3.85f,-0.02f));
    M4 = glm::rotate(M4, minuta_rotacja, glm::vec3(-1, 0, 0));

    drawObject(3,shaderPrograms[3],P,V,M4);

    glm::mat4 M5=glm::translate(M, glm::vec3(1.11f,3.85f,-0.02f));
    M5 = glm::rotate(M5, sekunda_rotacja, glm::vec3(-1, 0, 0));

    drawObject(4,shaderPrograms[4],P,V,M5);

    glm::mat4 M6=glm::translate(M, glm::vec3(-0.25f,1.30f,-0.30f));
    M6 = glm::rotate(M6, zebatki_rotacja, glm::vec3(-1, 0, 0));

    drawObject(5,shaderPrograms[5],P,V,M6);

    glm::mat4 M7=glm::translate(M, glm::vec3(-0.25f,1.70f,0.30f));
    M7 = glm::rotate(M7, zebatki_rotacja, glm::vec3(1, 0, 0));

    drawObject(6,shaderPrograms[6],P,V,M7);
	//Swap front and back buffers
	glfwSwapBuffers(window);

}



int main(void)
{
	GLFWwindow* window; //Pointer to window object

	glfwSetErrorCallback(error_callback);//Register error callback procedure

	if (!glfwInit()) { //Initialize GLFW procedure
		fprintf(stderr, "Can't initialize GLFW GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Create 500x500 window with "OpenGL" as well as OpenGL context.

	if (!window) //If window could not be created, then end the program
	{
		fprintf(stderr, "Can't create window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Since this moment, the window context is active and OpenGL commands will work with it.
	glfwSwapInterval(1); //Synchronize with the first VBLANK signal

	if (glewInit() != GLEW_OK) { //Initialize GLEW library
		fprintf(stderr, "Can't initialize GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Initialization procedure

	 angle_x = 3.14; //Object rotation angle
	 angle_y = 1.57; //Object rotation angle
	 angle_z = 3.14;

     szybkosc_sekunda = 1;
     szybkosc_minuta = szybkosc_sekunda/60;
     szybkosc_godzina = szybkosc_minuta/12;
     szybkosc_wahadla = szybkosc_sekunda/2;

	 zebatki_rotacja = 0;
	 szybkosc_zebatka = szybkosc_sekunda;

	 godzina_rotacja = 0;
	 minuta_rotacja = 0;
	 sekunda_rotacja = 0;
	 wahadlo_rotacja = 0;


     granica_wahadla = 0.35f;
     kierunek = 1;


	glfwSetTime(0); //Zero time counter

	//Main loop
	while (!glfwWindowShouldClose(window)) //As long as window shouldnt be closed...
	{
		angle_x += speed_x*glfwGetTime(); //Increase angle by the angle speed times the time passed since the previous frame
		angle_y += speed_y*glfwGetTime(); //Increase angle by the angle speed times the time passed since the previous frame

		zebatki_rotacja += szybkosc_zebatka*glfwGetTime();
		godzina_rotacja += szybkosc_godzina*glfwGetTime();
		minuta_rotacja += szybkosc_minuta*glfwGetTime();
		sekunda_rotacja += szybkosc_sekunda*glfwGetTime();

		if(wahadlo_rotacja >= granica_wahadla) kierunek = -1;
		if(wahadlo_rotacja <= -granica_wahadla) kierunek = 1;
		if(wahadlo_rotacja >= 2*granica_wahadla || wahadlo_rotacja <= 2*granica_wahadla*(-1)) wahadlo_rotacja = 0;

		wahadlo_rotacja += szybkosc_wahadla*glfwGetTime()*kierunek;


		glfwSetTime(0); //Zero time counter
		drawScene(window,angle_x,angle_y,angle_z,zebatki_rotacja,godzina_rotacja,minuta_rotacja,sekunda_rotacja,wahadlo_rotacja); //Execute drawing procedure
		glfwPollEvents(); //Execute callback procedures which process events
	}

	freeOpenGLProgram(); //Free resources

	glfwDestroyWindow(window); //Delete OpenGL context and window
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
