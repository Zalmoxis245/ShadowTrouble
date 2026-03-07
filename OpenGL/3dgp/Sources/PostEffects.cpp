#include "PostEffects.h"

PostEffects::PostEffects() {
	id = 0;
	tex_id = 0;
	fbo = 254;
	depth = 254;
	vbo = 254;
	prog = 254;
	fullscreen = true;
}

std::vector<PostEffects> PostEffects::Posts;
std::vector<GLuint> PostEffects::idFBO;
std::vector<GLuint> PostEffects::idDepthBuffer;
std::vector<GLuint> PostEffects::idVBO;

void PostEffects::AddNewPost(bool FBO, bool DepthBuffer, bool fullscreen, short width, short height) {
	//texture
	short tex_l = Textures::cnt_gl_texture_id_count;
	short l = static_cast<short>(Posts.size());
	glActiveTexture(GL_TEXTURE0 + tex_l);
	Textures::cnt_gl_texture_id_count++;
	Posts.emplace_back();
	glGenTextures(1, &Posts[l].id);
	glBindTexture(GL_TEXTURE_2D, Posts[l].id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	Posts[l].tex_id = tex_l;
	//FBO
	if (FBO) {
		short f = (short)idFBO.size();
		idFBO.push_back(GLuint());
		glGenFramebuffers(1, &idFBO[f]);
		glBindFramebuffer(GL_FRAMEBUFFER, idFBO[f]);
		Posts[l].fbo = (unsigned char)f;
	}
	else Posts[l].fbo = 254;
	//depth buffer
	if (DepthBuffer) {
		short d = (short)idDepthBuffer.size();
		idDepthBuffer.push_back(GLuint());
		glGenRenderbuffers(1, &idDepthBuffer[d]);
		glBindRenderbuffer(GL_RENDERBUFFER, idDepthBuffer[d]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, idDepthBuffer[d]);




		Posts[l].depth = (unsigned char)d;
	}
	else Posts[l].depth = 254;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Posts[l].id, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

	if (idVBO.empty()) {
		idVBO.push_back(GLuint());
		glGenBuffers(1, &idVBO[0]);
	}

	if (fullscreen) {
		Posts[l].vbo = 0;
		//temp
		float vertices[] = {
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 1.0f
		};
		// Bind the vertex buffer and send data
		glBindBuffer(GL_ARRAY_BUFFER, idVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	}
	else Posts[l].vbo = 254;
	Posts[l].fullscreen = fullscreen;

	if (Posts[l].fbo == 253) std::cout << COUT_RED << "ERROR 001: TOO MANY FBOs. INCREASE TO SHORT IN POST OR INVESTIGATE ISSUE." << RESET;
	if (Posts[l].fbo == 255) std::cout << COUT_RED << "ERROR 002: UNSIGNED UNDERFLOW IN FBOs. INVESTIGATE." << RESET;
	if (Posts[l].depth == 253) std::cout << COUT_RED << "ERROR 003: TOO MANY DBs. INCREASE TO SHORT IN POST OR INVESTIGATE ISSUE." << RESET;
	if (Posts[l].depth == 255) std::cout << COUT_RED << "ERROR 004: UNSIGNED UNDERFLOW IN DBs. INVESTIGATE." << RESET;
	if (Posts[l].vbo == 253) std::cout << COUT_RED << "ERROR 005: TOO MANY VBOs. INCREASE TO SHORT IN POST OR INVESTIGATE ISSUE." << RESET;
	if (Posts[l].vbo == 255) std::cout << COUT_RED << "ERROR 006: UNSIGNED UNDERFLOW IN VBOs. INVESTIGATE." << RESET;
}
void  PostEffects::AddNewPost() { // this assumes everything true as well as global win_w and win_h
	//texture
	short tex_l = Textures::cnt_gl_texture_id_count;
	short l = static_cast<short>(Posts.size());
	glActiveTexture(GL_TEXTURE0 + tex_l);
	Textures::cnt_gl_texture_id_count++;
	Posts.emplace_back();
	glGenTextures(1, &Posts[l].id);
	glBindTexture(GL_TEXTURE_2D, Posts[l].id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Globals::win_w, Globals::win_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	Posts[l].tex_id = tex_l;
	//FBO
	short f = (short)idFBO.size();
	idFBO.push_back(GLuint());
	glGenFramebuffers(1, &idFBO[f]);
	glBindFramebuffer(GL_FRAMEBUFFER, idFBO[f]);
	Posts[l].fbo = (unsigned char)f;
	
	//depth buffer
	short d = (short)idDepthBuffer.size();
	idDepthBuffer.push_back(GLuint());
	glGenRenderbuffers(1, &idDepthBuffer[d]);
	glBindRenderbuffer(GL_RENDERBUFFER, idDepthBuffer[d]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Globals::win_w, Globals::win_h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, idDepthBuffer[d]);
	Posts[l].depth = (unsigned char)d;
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Posts[l].id, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

	if (idVBO.empty()) {
		idVBO.push_back(GLuint());
		glGenBuffers(1, &idVBO[0]);
	}

	Posts[l].vbo = 0;
	//temp
	float vertices[] = {
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};
	// Bind the vertex buffer and send data
	glBindBuffer(GL_ARRAY_BUFFER, idVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	Posts[l].fullscreen = true;

	if (Posts[l].fbo == 253) std::cout << COUT_RED << "ERROR 001: TOO MANY FBOs. INCREASE TO SHORT IN POST OR INVESTIGATE ISSUE." << RESET;
	if (Posts[l].fbo == 255) std::cout << COUT_RED << "ERROR 002: UNSIGNED UNDERFLOW IN FBOs. INVESTIGATE." << RESET;
	if (Posts[l].depth == 253) std::cout << COUT_RED << "ERROR 003: TOO MANY DBs. INCREASE TO SHORT IN POST OR INVESTIGATE ISSUE." << RESET;
	if (Posts[l].depth == 255) std::cout << COUT_RED << "ERROR 004: UNSIGNED UNDERFLOW IN DBs. INVESTIGATE." << RESET;
	if (Posts[l].vbo == 253) std::cout << COUT_RED << "ERROR 005: TOO MANY VBOs. INCREASE TO SHORT IN POST OR INVESTIGATE ISSUE." << RESET;
	if (Posts[l].vbo == 255) std::cout << COUT_RED << "ERROR 006: UNSIGNED UNDERFLOW IN VBOs. INVESTIGATE." << RESET;
}
void PostEffects::DrawFullscreenQuad(GLuint attribVertex, GLuint attribTextCoord) {
	glBindBuffer(GL_ARRAY_BUFFER, idVBO[0]);

	glEnableVertexAttribArray(attribVertex);
	glEnableVertexAttribArray(attribTextCoord);

	glVertexAttribPointer(attribVertex, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glVertexAttribPointer(attribTextCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glDrawArrays(GL_QUADS, 0, 4);
	glDisableVertexAttribArray(attribVertex);
	glDisableVertexAttribArray(attribTextCoord);
}
void PostEffects::ApplyPosts() {
	Program::programs[POST0].sendUniform("matrixProjection", glm::ortho(0, 1, 0, 1, -1, 1));
	for (unsigned char i = 1; i < Posts.size(); i++) {

		if (!Posts[i].fullscreen) continue;
		Program::programs[POST0 + i].sendUniform("matrixProjection", glm::ortho(0, 1, 0, 1, -1, 1));
		glBindFramebuffer(GL_FRAMEBUFFER, idFBO[Posts[i].fbo]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Program::programs[POST0 + i].use(true);
		Program::programs[POST0 + i].sendUniform("texture0", Posts[i].tex_id);

		glActiveTexture(GL_TEXTURE0 + Posts[i].tex_id);
		glBindTexture(GL_TEXTURE_2D, Posts[i - 1].id);
		Program::programs[POST0 + i].sendUniform("matrixModelView", glm::mat4(1));
		DrawFullscreenQuad(Program::programs[POST0 + i].getAttribLocation("aVertex"), Program::programs[POST0 + i].getAttribLocation("aTexCoord"));
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Program::programs.back().use(true);
	Program::programs.back().sendUniform("texture0", Posts.back().tex_id);
	Program::programs.back().sendUniform("matrixModelView", glm::mat4(1));
	glActiveTexture(GL_TEXTURE0 + Posts.back().tex_id);
	glBindTexture(GL_TEXTURE_2D, Posts.empty() ? 0 : Posts.back().id);

	DrawFullscreenQuad(Program::programs.back().getAttribLocation("aVertex"), Program::programs.back().getAttribLocation("aTexCoord"));
}