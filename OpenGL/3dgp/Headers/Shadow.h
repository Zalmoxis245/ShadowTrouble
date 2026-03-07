#pragma once
#include "Globals.h"
#include <GL/glew.h>
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include <vector>
#include <iostream>
#include "Textures.h"

#include "Program.h"

struct Shadow {
    GLuint tex = 0, fbo = 0;
    unsigned char tex_id = 0, prog = 0, shadow_id = 0;
    static bool ShadowPassing;

    void Init(unsigned char program, unsigned char _shadow_id);
    void createShadowMap(glm::mat4 lightTransform, float time, float deltaTime, void (*renderScene)(glm::mat4&, float, float), void (*onReshape)(int, int));
};