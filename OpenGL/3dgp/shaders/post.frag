#version 330


// Input Variables (received from Vertex Shader)

in vec2 texCoord0;


uniform sampler2D texture0;
uniform vec2 resolution = vec2(1280, 720);
//uniform float time;
//uniform int bools; //NULL | NULL | NULL | NULL || inverted color | enable color change | enable zoom | enable distortions

// Output Variable (sent down through the Pipeline)

out vec4 outColor;


void main(void)

{
   outColor = texture(texture0, texCoord0);
}