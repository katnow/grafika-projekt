#include <iostream>
#include <fstream>
#include <vector>
#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "Positions.cpp"



int main() {
    // Inicjalizacja GLFW i GLEW
    glfwInit();
    // Konfiguracja kontekstu OpenGL (2.1 w tym przykładzie)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Terrain Rendering", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewInit();

    int width = 72;
    int height = 41;

    // Wczytaj dane z pliku HGT
    

    std::vector<float> vertices;
    float yScale = 64.0f / 256.0f, yShift = 16.0f;  // apply a scale+shift to the height data
    for (unsigned int i = 0; i < height; i++)
    {
        for (unsigned int j = 0; j < width; j++)
        {
            // retrieve texel for (i,j) tex coord
            short* texel = terrainPositions + (j + width * i) * 1;
            // raw height at coordinate
            unsigned char y = texel[0];

            // vertex
            vertices.push_back(-height / 2.0f + i);        // v.x
            vertices.push_back((int)y * yScale - yShift); // v.y
            vertices.push_back(-width / 2.0f + j);        // v.z
        }
    }

    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < height - 1; i++)       // for each row a.k.a. each strip
    {
        for (unsigned int j = 0; j < width; j++)      // for each column
        {
            for (unsigned int k = 0; k < 2; k++)      // for each side of the strip
            {
                indices.push_back(j + width * (i + k));
            }
        }
    }

    const unsigned int NUM_STRIPS = height - 1;
    const unsigned int NUM_VERTS_PER_STRIP = width * 2;

    GLuint terrainVAO, terrainVBO, terrainEBO;
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),       // size of vertices buffer
        &vertices[0],                          // pointer to first element
        GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &terrainEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int), // size of indices buffer
        &indices[0],                           // pointer to first element
        GL_STATIC_DRAW);

    // Tworzenie bufora wierzchołków

    glBindVertexArray(terrainVAO);
    // render the mesh triangle strip by triangle strip - each row at a time
    for (unsigned int strip = 0; strip < NUM_STRIPS; ++strip)
    {
        glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
            NUM_VERTS_PER_STRIP, // number of indices to render
            GL_UNSIGNED_INT,     // index data type
            (void*)(sizeof(unsigned int)
                * NUM_VERTS_PER_STRIP
                * strip)); // offset to starting index
    }


    // Pętla renderowania
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Renderowanie terenu
 

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            // Handle the error (log it, display an error message, etc.)
            fprintf(stderr, "OpenGL error %d occurred after glDrawArrays\n", error);
        }
        else {
            fprintf(stderr, "ok");
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Zwolnij zasoby
    glfwTerminate();

    return 0;
}