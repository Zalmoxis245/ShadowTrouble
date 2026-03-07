#version 330


in vec2 texCoord0;


uniform sampler2D texture0;
uniform vec2 resolution = vec2(1280, 720);
uniform float time;
uniform bool blur;

out vec4 outColor;


void main(void)

{
    outColor = texture(texture0, texCoord0);
    if(!blur) return;

    float pulse = (sin(time*4) + 1.0) * 2.5;
    vec2 off = (vec2(1.0, 0) / resolution) * pulse;
    
    vec4 color = texture(texture0, texCoord0) * 0.2270270270;
    
    color += texture(texture0, texCoord0 + (off * 1.3846153846)) * 0.3162162162;
    color += texture(texture0, texCoord0 - (off * 1.3846153846)) * 0.3162162162;
    
    color += texture(texture0, texCoord0 + (off * 3.2307692308)) * 0.0702702703;
    color += texture(texture0, texCoord0 - (off * 3.2307692308)) * 0.0702702703;

    outColor = vec4(color.rgb, 1.0);
}