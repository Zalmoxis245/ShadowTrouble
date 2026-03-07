#include "Shadow.h"

bool Shadow::ShadowPassing =false;

void Shadow::Init(unsigned char program, unsigned char _shadow_id) {

    tex_id = (unsigned char)Textures::cnt_gl_texture_id_count++;
    prog = program;
    shadow_id = _shadow_id;
    glEnable(GL_CULL_FACE);
    Program::programs[prog].use(true);
    glActiveTexture(GL_TEXTURE0 + tex_id);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);


    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int w = viewport[2], h = viewport[3];
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w * 2, h * 2, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    Program::programs[prog].sendUniform("shadowMap[" + std::to_string((int)shadow_id) + "]", tex_id);
    glActiveTexture(GL_TEXTURE0);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER_EXT, fbo);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, tex, 0);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void Shadow::createShadowMap(glm::mat4 lightTransform, float time, float deltaTime, void (*renderScene)(glm::mat4&, float, float), void (*onReshape)(int, int)) {
    Program::programs[prog].use(true);
    glActiveTexture(GL_TEXTURE0 + tex_id);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    // Store the current viewport in a safe place
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int w = viewport[2], h = viewport[3];
    // setup the viewport to 2x2 the original and wide (120 degrees) FoV (Field of View)
    glViewport(0, 0, w * 2, h * 2);
    glm::mat4 matrixProjection = glm::perspective(glm::radians(160.f), (float)w / (float)h, 0.5f, 50.0f);
    Program::programs[prog].sendUniform("matrixProjection", matrixProjection);
    // prepare the camera
    glm::mat4 matrixView = lightTransform;
    // send the View Matrix
    Program::programs[prog].sendUniform("matrixView", matrixView);
    // Bind the Framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
    // OFF-SCREEN RENDERING FROM NOW!
    // Clear previous frame values - depth buffer only!
    glClear(GL_DEPTH_BUFFER_BIT);
    // Disable color rendering, we only want to write to the Z-Buffer (this is to speed-up)
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    // Prepare and send the Shadow Matrix - this is matrix transform every coordinate x,y,z
    // x = x* 0.5 + 0.5
    // y = y* 0.5 + 0.5
    // z = z* 0.5 + 0.5
    // Moving from unit cube [-1,1] to [0,1]
    const glm::mat4 bias = {
    { 0.5, 0.0, 0.0, 0.0 },
    { 0.0, 0.5, 0.0, 0.0 },
    { 0.0, 0.0, 0.5, 0.0 },
    { 0.5, 0.5, 0.5, 1.0 }
    };
    Program::programs[prog].sendUniform("matrixShadow[" + std::to_string((int)shadow_id) + "]", bias * matrixProjection * matrixView);
    // Render all objects in the scene
    renderScene(matrixView, time, deltaTime);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    onReshape(w, h);
    glActiveTexture(GL_TEXTURE0);
}
