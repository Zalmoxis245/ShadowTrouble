#version 330


// Uniforms: Transformation Matrices

uniform mat4 matrixProjection;
uniform mat4 matrixModelView;


in vec3 aVertex;
in vec2 aTexCoord;


out vec2 texCoord0;


void main(void)

{

vec4 position = matrixModelView * vec4(aVertex, 1.0);

gl_Position = matrixProjection * position;

texCoord0 = aTexCoord;

}