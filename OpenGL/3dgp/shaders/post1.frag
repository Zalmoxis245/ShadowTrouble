#version 330


// Input Variables (received from Vertex Shader)

in vec2 texCoord0;


uniform sampler2D texture0;
uniform vec2 resolution = vec2(1280, 720);
uniform float time;
uniform int bools; //NULL | NULL | NULL | NULL || inverted color | enable color change | enable zoom | enable distortions

// Output Variable (sent down through the Pipeline)

out vec4 outColor;


void main(void)

{
    vec2 uv = texCoord0 * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;

    float r = length(uv);

    float distortionStrength = -0.1;

    // wave
    float phase = r * 5.0 - time * 5.0;
    float wave = sin(phase) * 0.2;

    float distortion = 1.0 + distortionStrength * r * r + wave;

    uv *= ( (bools&0x1)==0x1?distortion: 1.0f);

    //zoom
    float zoom = 1.4;
    uv/= ( (bools&0x2)==0x2?zoom: 1.0f);


    uv.x /= resolution.x / resolution.y;
    vec2 finalUV = uv * 0.5 + 0.5;

    outColor = texture(texture0, finalUV);
    
    //gradients
    float g = r+time*0.2;

    vec3 gradient = vec3(
        sin(phase)*0.5+0.5,
        sin(phase+1)*0.5+0.5,
        sin(phase+2)*0.5+0.5
    );
    gradient *= 0.8 + 0.2 * sin(time);
    if((bools&0x4)==0x4)
        if((bools&0x8)==0x8)
            outColor =  vec4(1) - vec4(mix(outColor.rgb, gradient, 0.7), outColor.a);
        else
            outColor =  vec4(mix(outColor.rgb, gradient, 0.7), 1.0);

}