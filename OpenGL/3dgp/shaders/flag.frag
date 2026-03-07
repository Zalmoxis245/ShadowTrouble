#version 330

struct POINT {
    vec3 position;
    vec3 colour;
};

struct AMBIENT {    
    vec3 color;
};

uniform sampler2D texture0;
uniform vec3 materialAmbient;
uniform AMBIENT lightAmbient;
uniform POINT light;

in vec4 color;
in vec4 position;
in vec3 normal;
in vec2 texCoord0;

out vec4 OutColor;

void main() 
{
    vec3 ambient = materialAmbient * lightAmbient.color;

    vec3 N = normalize(normal);
    vec3 L = normalize(light.position - position.xyz);
    float diff = max(dot(N, L), 0.0);
    vec3 lambertian = diff * light.colour;
    vec4 texColor = texture(texture0, texCoord0);
    
    OutColor = vec4((ambient + lambertian) * texColor.rgb, 1.0);
}