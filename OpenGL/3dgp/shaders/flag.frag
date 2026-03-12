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
//dematerializer
uniform sampler2D dematerializer;
uniform float t_dem;
uniform bool dematerializerActive;
//dematerializer

in vec4 color;
in vec4 position;
in vec3 normal;
in vec2 texCoord0;

out vec4 OutColor;

void main() 
{
    //Dematerializer
    if(dematerializerActive){
        vec2 noiseCoord0 = (texCoord0 == vec2(0,0)) ? position.xy * 2.0 : texCoord0;
        float noise = texture(dematerializer, noiseCoord0).r;
        if(noise < t_dem)
        discard;
    }

    //light & colour
    vec3 ambient = materialAmbient * lightAmbient.color;

    vec3 N = normalize(normal);
    vec3 L = normalize(light.position - position.xyz);
    float diff = max(dot(N, L), 0.0);
    vec3 lambertian = diff * light.colour;
    vec4 texColor = texture(texture0, texCoord0);
    
    OutColor = vec4((ambient + lambertian) * texColor.rgb, 1.0);
}