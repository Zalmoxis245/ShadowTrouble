#version 330

struct POINT {
    vec3 position;
    vec3 colour;
};

// Uniforms: Transformation Matrices
uniform mat4 matrixProjection;
uniform mat4 matrixModelView;

uniform vec3 materialAmbient;
uniform float t;
uniform float freq;  
uniform float amp; 
uniform float speed; 
uniform POINT light;
uniform sampler2D texture0;

in vec3 aVertex;
in vec3 aNormal;
in vec2 aTexCoord;

out vec4 color;
out vec4 position;
out vec3 normal;
out vec2 texCoord0;

struct AMBIENT
{    
    vec3 color;
}; 
uniform AMBIENT lightAmbient;

vec4 AmbientLight(AMBIENT light)
{
    return vec4(materialAmbient * light.color, 1);
}

float wave(vec3 p, vec2 uv){
    float mainWave = sin( p.z * freq - speed * t)  * amp;
    float verticalMask = 1 - uv.y;

    return mainWave * verticalMask;
}
//Comment to chekc version I am on
void main(void) 
{   
    //pos
    float d = 0.01; // Changed to 0.01 because d=1 makes the flag look flat
    float zDisp = wave(aVertex, aTexCoord);

    float dydx = (wave(aVertex + vec3(d,0,0), aTexCoord) - wave(aVertex - vec3(d,0,0), aTexCoord)) / 2.0 / d;
    float dydz = (wave(aVertex + vec3(0,0,d), aTexCoord) - wave(aVertex - vec3(0,0,d), aTexCoord)) / 2.0 / d;
    vec3 newNormal = normalize(vec3(-dydx, 1.0, -dydz));
    vec3 newPos = aVertex + vec3(0.0, zDisp, 0.0);

    position = matrixModelView * vec4(newPos, 1.0);
    normal = normalize(mat3(matrixModelView) * newNormal);
    texCoord0 = vec2(aTexCoord.y, 1.0 - aTexCoord.x);

    gl_Position = matrixProjection * position;

    //color
    color = vec4(0, 0, 0, 1);
    color += AmbientLight(lightAmbient);

    vec3 L = normalize(light.position - position.xyz);
    float diff = max(dot(normalize(normal), L), 0.0);
    vec3 lambertian = diff * light.colour;
    
    vec4 texColor = texture(texture0, aTexCoord);
    
    color = vec4((color.rgb + lambertian) * texColor.rgb, texColor.a);
}