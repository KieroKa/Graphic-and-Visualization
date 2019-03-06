#ifndef LOADOBJ_H
#define LOADOBJ_H


#include <vector>
#include <fstream>
#include <string.h>
#include <iostream>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "constants.h"

bool loadOBJ(
    const char * path,
    std::vector < glm::vec4 > & out_vertices,
    std::vector < glm::vec2 > & out_uvs,
    std::vector < glm::vec4 > & out_normals,
    std::vector < glm::vec4 > & out_colors,
    int & vertexCount
);


#endif
