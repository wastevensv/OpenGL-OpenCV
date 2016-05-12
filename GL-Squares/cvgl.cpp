#include <GL/glew.h>
#include <gl/GL.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cstdio>
#include <string>
#include <cmath>
#include <cstring>
#include <fstream>
#include <streambuf>

#include "objhelper.hpp"
#include "cvhelper.hpp"
#include "shaders.h"

float backdrop_vert[] = {
//  Position             Texture
     1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // Bottom Right
     1.0f,  1.0f,  0.0f, 1.0f, 1.0f, // Top Right
     0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // Top Left
     
     1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // Bottom Right
     0.0f,  0.0f,  0.0f, 0.0f, 0.0f, // Bottom Left
     0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // Top Left
};

float axis_vertices[] = {
// Position              Color
     0.0f,  0.0f,  0.0f, 1.0f, 1.0f,  1.0f, // Origin
     1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f, // X Line
    
     0.0f,  0.0f,  0.0f, 1.0f, 1.0f,  1.0f, // Origin
     0.0f,  1.0f,  0.0f, 0.0f, 1.0f,  0.0f, // Y Line
    
     0.0f,  0.0f,  0.0f, 1.0f, 1.0f,  1.0f, // Origin
     0.0f,  0.0f,  1.0f, 0.0f, 0.0f,  1.0f, // Z Line
};

int main()
{
    
    // --- OpenCV Init ---
    //0 is the id of video device. 0 if you have only one camera.
    cv::VideoCapture stream1(1);

    if (!stream1.isOpened()) { //check if video device has been initialised
            std::cerr << "cannot open camera" << std::endl;
    }
         
    // --- GLFW Init ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // --- Create window ---
    GLFWwindow* window = glfwCreateWindow(800, 600,
                                      "OpenGL", nullptr, nullptr); // Windowed
    if(window == NULL) return 2;
    glfwMakeContextCurrent(window);
    
    // --- GLEW/GL Init ---
    glewExperimental = GL_TRUE;
    glewInit();

    // --- Application Specific Setup ---
    
    
    // Create FG Vertex Array Object
    GLuint bd_vao;
    glGenVertexArrays(1, &bd_vao);
    glBindVertexArray(bd_vao);
    
    // Create FG Vertex Buffer Object
    GLuint bd_vbo;
    glGenBuffers(1, &bd_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, bd_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backdrop_vert),
                                  backdrop_vert, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    
    // Create BG Vertex Array Object
    GLuint bb_vao;
    glGenVertexArrays(1, &bb_vao);
    glBindVertexArray(bb_vao);

    // Create BG Vertex Buffer Object
    GLuint bb_vbo;
    glGenBuffers(1, &bb_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, bb_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backdrop_vert),
                                  backdrop_vert, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    // Create Axis Vertex Array Object
    GLuint axis_vao;
    glGenVertexArrays(1, &axis_vao);
    glBindVertexArray(axis_vao);

    // Create Axis Vertex Buffer Object
    GLuint axis_vbo;
    glGenBuffers(1, &axis_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, axis_vbo);

//    vector<glm::vec3> objVertices;
//    bool res = loadOBJ("cube.obj", objVertices, vector<glm::vec2>(),
//                                                vector<glm::vec3>());
//    if(res) {
//      glBufferData(GL_ARRAY_BUFFER, objVertices.size() * sizeof(glm::vec3),
//                                    &objVertices[0], GL_STATIC_DRAW);
//    } else {
      glBufferData(GL_ARRAY_BUFFER, sizeof(axis_vertices),
                                    axis_vertices, GL_STATIC_DRAW);
//    }
    glBindVertexArray(0);

    // Compile color vertex shader
    GLuint colorVert = glCreateShader(GL_VERTEX_SHADER);
    //const char *cvSource = readFile("3dvert.glsl").c_str();
    const char *cvSource = SHADER_VERT3D_COLOR;

    glShaderSource(colorVert, 1, &cvSource, NULL);
    glCompileShader(colorVert);
    GLint cvStatus;
    glGetShaderiv(colorVert, GL_COMPILE_STATUS, &cvStatus);
    if(cvStatus != GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(colorVert, 512, NULL, buffer);
        std::cerr << buffer << std::endl;
        return 1;
    }

    // Compile color fragment shader
    GLuint colorFrag = glCreateShader(GL_FRAGMENT_SHADER);
    //const char* cfSource = readFile("colorfrag.glsl").c_str();
    const char *cfSource = SHADER_FRAG_COLOR;

    glShaderSource(colorFrag, 1, &cfSource, NULL);
    glCompileShader(colorFrag);
    GLint cfStatus;
    glGetShaderiv(colorFrag, GL_COMPILE_STATUS, &cfStatus);
    if(cfStatus != GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(colorFrag, 512, NULL, buffer);
        std::cerr << buffer << std::endl;
        return 2;
    }

    // Compile pretty vertex shader
    GLuint prettyVert = glCreateShader(GL_VERTEX_SHADER);
    //const char *pvSource = readFile("3dvert.glsl").c_str();
    const char *pvSource = SHADER_VERT3D;

    glShaderSource(prettyVert, 1, &pvSource, NULL);
    glCompileShader(prettyVert);
    GLint pvStatus;
    glGetShaderiv(prettyVert, GL_COMPILE_STATUS, &pvStatus);
    if(pvStatus != GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(prettyVert, 512, NULL, buffer);
        std::cerr << buffer << std::endl;
        return 3;
    }

    // Compile pretty fragment shader
    GLuint prettyFrag = glCreateShader(GL_FRAGMENT_SHADER);
    //const char* pfSource = readFile("prettyfrag.glsl").c_str();
    const char* pfSource = SHADER_FRAG_PRETTY;

    glShaderSource(prettyFrag, 1, &pfSource, NULL);
    glCompileShader(prettyFrag);
    GLint pfStatus;
    glGetShaderiv(prettyFrag, GL_COMPILE_STATUS, &pfStatus);
    if(pfStatus != GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(prettyFrag, 512, NULL, buffer);
        std::cerr << buffer << std::endl;
        return 4;
    }

    // Compile blank vertex shader
    GLuint blankVert = glCreateShader(GL_VERTEX_SHADER);
    //const char *bvSource = readFile("3dvert.glsl").c_str();
    const char *bvSource = SHADER_VERT3D;

    glShaderSource(blankVert, 1, &bvSource, NULL);
    glCompileShader(blankVert);
    GLint bvStatus;
    glGetShaderiv(blankVert, GL_COMPILE_STATUS, &bvStatus);
    if(bvStatus != GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(blankVert, 512, NULL, buffer);
        std::cerr << buffer << std::endl;
        return 5;
    }

    // Compile blank fragment shader
    GLuint blankFrag = glCreateShader(GL_FRAGMENT_SHADER);
    //const char* bfSource = readFile("simplefrag.glsl").c_str();
    const char* bfSource = SHADER_FRAG_SIMPLE;

    glShaderSource(blankFrag, 1, &bfSource, NULL);
    glCompileShader(blankFrag);
    GLint bfStatus;
    glGetShaderiv(blankFrag, GL_COMPILE_STATUS, &bfStatus);
    if(bfStatus != GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(blankFrag, 512, NULL, buffer);
        std::cerr << buffer << std::endl;
        return 6;
    }

    // Make color shader program.
    GLuint colorShaderProgram = glCreateProgram();
    glAttachShader(colorShaderProgram, colorVert);
    glAttachShader(colorShaderProgram, colorFrag);
    glBindFragDataLocation(colorShaderProgram, 0, "outColor");
    glLinkProgram(colorShaderProgram);
    glUseProgram(colorShaderProgram);
    

    //GLint ctAttrib = glGetAttribLocation(colorShaderProgram, "texcoord`");
    //glEnableVertexAttribArray(ctAttrib);
    //glVertexAttribPointer(ctAttrib, 3, GL_FLOAT, GL_FALSE,
    //            6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    
    glBindVertexArray(axis_vao);
    glBindBuffer(GL_ARRAY_BUFFER, axis_vbo);
    GLint cvpAttrib = glGetAttribLocation(colorShaderProgram, "position");
    glEnableVertexAttribArray(cvpAttrib);
    glVertexAttribPointer(cvpAttrib, 3, GL_FLOAT, GL_FALSE,
                 6*sizeof(GLfloat), 0);

    GLint cvcAttrib = glGetAttribLocation(colorShaderProgram, "v_color");
    glEnableVertexAttribArray(cvcAttrib);
    glVertexAttribPointer(cvcAttrib, 3, GL_FLOAT, GL_FALSE,
                 6*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));

    // Make pretty shader program.
    GLuint prettyShaderProgram = glCreateProgram();
    glAttachShader(prettyShaderProgram, prettyVert);
    glAttachShader(prettyShaderProgram, prettyFrag);
    glBindFragDataLocation(prettyShaderProgram, 0, "outColor");
    glLinkProgram(prettyShaderProgram);
    glUseProgram(prettyShaderProgram);
    

    glBindVertexArray(bd_vao);
    glBindBuffer(GL_ARRAY_BUFFER, bd_vbo);
    GLint pvpAttrib = glGetAttribLocation(prettyShaderProgram, "position");
    glEnableVertexAttribArray(pvpAttrib);
    glVertexAttribPointer(pvpAttrib, 3, GL_FLOAT, GL_FALSE,
                 5*sizeof(GLfloat), 0);

    GLint pvtAttrib = glGetAttribLocation(prettyShaderProgram, "texcoord");
    glEnableVertexAttribArray(pvtAttrib);
    glVertexAttribPointer(pvtAttrib, 2, GL_FLOAT, GL_FALSE,
                5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    // Make blank shader program.
    GLuint blankShaderProgram = glCreateProgram();
    glAttachShader(blankShaderProgram, blankVert);
    glAttachShader(blankShaderProgram, blankFrag);
    glBindFragDataLocation(blankShaderProgram, 0, "outColor");
    glLinkProgram(blankShaderProgram);
    glUseProgram(blankShaderProgram);

    glBindVertexArray(bb_vao);
    glBindBuffer(GL_ARRAY_BUFFER, bb_vbo);
    GLint bvpAttrib = glGetAttribLocation(blankShaderProgram, "position");
    glEnableVertexAttribArray(bvpAttrib);
    glVertexAttribPointer(bvpAttrib, 3, GL_FLOAT, GL_FALSE,
                 5*sizeof(GLfloat), 0);

    GLint bvtAttrib = glGetAttribLocation(blankShaderProgram, "texcoord");
    glEnableVertexAttribArray(bvtAttrib);
    glVertexAttribPointer(bvtAttrib, 2, GL_FLOAT, GL_FALSE,
                5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    // Setup view
    glm::mat4 view = glm::lookAt(
                   glm::vec3( 0.5f,  2.0f,  1.5f),
                   glm::vec3( 0.5f,  0.0f,  0.0f),
                   glm::vec3( 0.0f,  0.0f,  1.0f)
                   );
    glm::mat4 proj = 
          glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 10.0f);
    glm::mat4 model = glm::mat4();

    glUniformMatrix4fv(glGetUniformLocation(colorShaderProgram, "view" ),
                                            1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(colorShaderProgram, "proj" ),
                                            1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(colorShaderProgram, "model"),
                                            1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix4fv(glGetUniformLocation(prettyShaderProgram, "view" ),
                                            1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(prettyShaderProgram, "proj" ),
                                            1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(prettyShaderProgram, "model"),
                                            1, GL_FALSE, glm::value_ptr(model));
    
    glUniformMatrix4fv(glGetUniformLocation(blankShaderProgram, "view" ),
                                            1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(blankShaderProgram, "proj" ),
                                            1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(blankShaderProgram, "model"),
                                            1, GL_FALSE, glm::value_ptr(model));

    // Create texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);


    glEnable(GL_DEPTH_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    float xco = 0.0f;
    float yco = 0.0f;
    float angle = 0.0f;
    float scale = 0.2f;
    // --- Main Loop ---
    while(!glfwWindowShouldClose(window))
    {
        // Reset
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Move camera
        //GLfloat radius = 2.0f;
        //GLfloat camX = sin(glfwGetTime()) * radius;
        //GLfloat camY = cos(glfwGetTime()) * radius;
        //view = glm::lookAt(glm::vec3(camX,camY,1.5f),
        //        glm::vec3(0.0f, 0.0f, 0.0f),
        //        glm::vec3(0.0f, 0.0f, 1.0f));

        // Capture Image
        cv::Mat cameraFrame;
        stream1 >> cameraFrame;

        // Process Image
        cv::Mat processedFrame = cameraFrame.clone();
        vector<vector<cv::Point>> anchors;

        // Find object.
        findObjects(processedFrame, anchors, 0);
        if (anchors.size() != 0) {
            float dx, dy;
            
            xco = ((float)anchors[0][0].x / processedFrame.cols);
            yco = ((float)anchors[0][0].y / processedFrame.rows);

            if(anchors[0][1].y > anchors[0][2].y) {
              dx = (anchors[0][1].x - anchors[0][0].x);
              dy = (anchors[0][1].y - anchors[0][0].y);
            } else {
              dx = (anchors[0][2].x - anchors[0][0].x);
              dy = (anchors[0][2].y - anchors[0][0].y);
            }

            scale = 2*(dy/processedFrame.rows);
            angle = -atan(dx/dy);

            cerr << "#" << anchors[0][0].x << ", \t" << anchors[0][0].y << endl;
            cerr << "!" << xco << ", \t" << yco << ", \t" << endl;
            cerr << " " << angle << ", \t" << scale <<endl;
        }

        // Draw Baseboard
        model = glm::mat4();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                      processedFrame.cols, processedFrame.rows, 0, GL_BGR,
                      GL_UNSIGNED_BYTE, processedFrame.data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glUseProgram(blankShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(prettyShaderProgram, "view"),
          1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(prettyShaderProgram, "proj"),
          1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(prettyShaderProgram, "model"),
          1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(bd_vao);
          glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Draw Backboard
        model = glm::rotate(
                  model,
                  glm::radians(90.0f),
                  glm::vec3(1.0f, 0.0f, 0.0f)
                );
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                      cameraFrame.cols, cameraFrame.rows, 0, GL_BGR,
                      GL_UNSIGNED_BYTE, cameraFrame.data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glUseProgram(blankShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(blankShaderProgram, "view"),
          1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(blankShaderProgram, "proj"),
          1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(blankShaderProgram, "model"),
          1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(bb_vao);
          glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Draw Axis
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(xco, yco, 0.0f));
        model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(scale));

        glUseProgram(colorShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(colorShaderProgram, "view"),
                1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(colorShaderProgram, "proj"),
                1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(colorShaderProgram, "model"),
                1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(axis_vao);
          glDrawArrays(GL_LINES, 0, 6);
        glBindVertexArray(0);

        //Display
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- Cleanup/Shutdown ---
    glDeleteProgram(colorShaderProgram);
    glDeleteProgram(prettyShaderProgram);
    glDeleteProgram(blankShaderProgram);
    
    glDeleteShader(colorFrag);
    glDeleteShader(colorVert);

    glDeleteShader(prettyFrag);
    glDeleteShader(prettyVert);

    glDeleteShader(blankVert);
    glDeleteShader(blankFrag);

    glDeleteBuffers(1, &axis_vbo);
    glDeleteBuffers(1, &bd_vbo);
    glDeleteBuffers(1, &bb_vbo);

    glDeleteVertexArrays(1, &axis_vao);
    glDeleteVertexArrays(1, &bd_vao);
    glDeleteVertexArrays(1, &bb_vao);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
