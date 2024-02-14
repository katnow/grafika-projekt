#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Texture.h"

#include "SOIL/SOIL.h"

#include "Positions.cpp"


const int width = 3601;
const int length = 3601;

const float yScale = 1.f/8.f;
const float yShift = 32.f;

std::vector<float> vertices;
std::vector<unsigned int> indices;

GLuint terrainVAO, terrainVBO, terrainEBO;

GLuint shaderProgram;
Core::Shader_Loader shaderLoader;

Core::RenderContext terrainContext;

glm::vec3 cameraPos = glm::vec3(-4.f, 0, 0);
glm::vec3 cameraDir = glm::vec3(1.f, 0.f, 0.f);

glm::vec3 lightColor = glm::vec3(1.f, 1.f, 1.f);

float aspectRatio = 8.f / 6.f;

unsigned char* terrainTexture;

namespace texture {
    GLuint terrain; 
}

glm::mat4 createCameraMatrix()
{
    glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir, glm::vec3(0.f, 1.f, 0.f)));
    glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide, cameraDir));

    glm::mat4 cameraRotrationMatrix = glm::mat4({
        cameraSide.x,cameraSide.y,cameraSide.z,0,
        cameraUp.x,cameraUp.y,cameraUp.z ,0,
        -cameraDir.x,-cameraDir.y,-cameraDir.z,0,
        0.,0.,0.,1.,
    });

    cameraRotrationMatrix = glm::transpose(cameraRotrationMatrix);
    glm::mat4 cameraMatrix = cameraRotrationMatrix * glm::translate(-cameraPos);

    return cameraMatrix;
}

glm::mat4 createPerspectiveMatrix()
{
    glm::mat4 perspectiveMatrix;
    float n = 0.05;
    float f = 4096.f;
    float a1 = glm::min(aspectRatio, 1.f);
    float a2 = glm::min(1 / aspectRatio, 1.f);

    perspectiveMatrix = glm::mat4({
        1,0.,0.,0.,
        0.,aspectRatio,0.,0.,
        0.,0.,(f + n) / (n - f),2 * f * n / (n - f),
        0.,0.,-1.,0.,
     });

    perspectiveMatrix = glm::transpose(perspectiveMatrix);

    return perspectiveMatrix;
}

void renderTerrain() {
    Core::SetActiveTexture(texture::terrain, "colorTexture", shaderProgram, 5);
    glUseProgram(shaderProgram);
    
    glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    //modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 transformation = viewProjectionMatrix * modelMatrix;

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transformation"), 1, GL_FALSE, (float*)&transformation);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 1200, 500, 1200);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

    glBindVertexArray(terrainVAO);

    // Draw the terrain using 
    // GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP,
    // GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY, GL_TRIANGLES_ADJACENCY, GL_PATCHES
    glDrawElements(
        GL_TRIANGLES,
        (width - 1) * (length - 1) * 6,
        GL_UNSIGNED_INT, 
        0
    );

    glBindVertexArray(0);
}

void renderScene(GLFWwindow* window)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderTerrain();

    glfwSwapBuffers(window);

    glfwPollEvents();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    aspectRatio = width / float(height);
    glViewport(0, 0, width, height);
}

void init(GLFWwindow* window)
{
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glEnable(GL_DEPTH_TEST);

    shaderProgram = shaderLoader.CreateProgram("shaders/edges_shader.vert", "shaders/edges_shader.frag");
    texture::terrain = Core::LoadTexture("img/mapa_highres2.jpg");

    for (unsigned int x = 0; x < length; x++)
    {
        for (unsigned int z = 0; z < width; z++)
        {
            // retrieve texel for (x,z) tex coord
            short texel = terrainPositions[x][z];
            // raw height at coordinate
            unsigned int y = texel;
            
            // vertex
            vertices.push_back(1.0f * x);        // v.x
            vertices.push_back(1.0f * z);        // v.z
            vertices.push_back(1.0f * y * yScale - yShift); // v.y
        }
    }

    for (unsigned int x = 0; x < width - 1; x++) {
        for (unsigned int z = 0; z < length - 1; z++) {
            // Pierwszy tr�jk�t w pasie
            indices.push_back(z + length * x);
            indices.push_back(z + length * (x + 1));
            indices.push_back(z + length * (x + 1) + 1);

            // Drugi tr�jk�t w pasie
            indices.push_back(z + length * x);
            indices.push_back(z + length * (x + 1) + 1);
            indices.push_back(z + length * x + 1);
        }
    }

    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        &vertices[0],
        GL_STATIC_DRAW
    );


    // vertexPosition
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * (sizeof(float)), (void*)0);

    // vertexNormal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * (sizeof(float)), (void*)0);
    
    // vertexTexCoord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 3 * (sizeof(float)), (void*)0);

    glGenBuffers(1, &terrainEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        &indices[0],
        GL_STATIC_DRAW
    );
}

void shutdown(GLFWwindow* window)
{
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
    glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir, glm::vec3(0.f, 1.f, 0.f)));
    glm::vec3 cameraUp = glm::vec3(0.f, 1.f, 0.f);
    float angleSpeed = 0.02f;
    float moveSpeed = 5.0f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraDir * moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraDir * moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        cameraPos += cameraSide * moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        cameraPos -= cameraSide * moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += cameraUp * moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos -= cameraUp * moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(cameraDir, 0));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(cameraDir, 0));
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        glm::vec3 axis = glm::normalize(glm::cross(cameraDir, cameraUp));
        cameraDir = glm::normalize(glm::angleAxis(-angleSpeed, axis) * cameraDir);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        glm::vec3 axis = glm::normalize(glm::cross(cameraDir, cameraUp));
        cameraDir = glm::normalize(glm::angleAxis(angleSpeed, axis) * cameraDir);
    }
}

void renderLoop(GLFWwindow* window) {
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        renderScene(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &terrainVAO);
    glDeleteBuffers(1, &terrainVBO);
    glDeleteBuffers(1, &terrainEBO);
}