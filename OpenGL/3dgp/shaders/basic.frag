#version 330 core

struct AMBIENT {
    vec3 color;
};

struct DIRECTIONAL {
    vec3 direction;
    vec3 diffuse;
};

struct POINT {
    vec3 position; 
    vec3 diffuse;
    vec3 specular;
};

struct SPOT {
    vec3 direction;
    float cutoff; 
    float exponent;
};

in float fogFactor; //fog
in vec3 vPos;
in vec3 vNormal;
in vec2 vTex;
in vec4 shadowCoord[2];

out vec4 outColor;

uniform sampler2D texture0;
//dematerializer
uniform sampler2D dematerializer;
uniform float t_dem;
uniform bool dematerializerActive;
//dematerializer
uniform sampler2DShadow shadowMap[2];
uniform bool hasTex;
uniform bool receiveShadows;

uniform vec3 fogColour; //fog
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;

uniform AMBIENT lightAmbient;
uniform DIRECTIONAL lightDir;
uniform POINT lightPoint0;
uniform POINT lightPoint1;
uniform SPOT  lightSpot0;
uniform SPOT  lightSpot1;
uniform float shadowLightIntensity[2];

uniform float att_quadratic;

vec3 PointLight(POINT light, vec3 N)
{
    vec3 L = normalize(light.position - vPos);
    vec3 V = normalize(-vPos);
    vec3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);

    float dist = length(light.position - vPos);
    float att  = 1.0 / (1.0 + att_quadratic * dist * dist);

    vec3 diffuse  = materialDiffuse * light.diffuse * NdotL;
    vec3 specular = materialSpecular * light.specular * pow(NdotH, shininess) /20;

    return (diffuse+specular) * att ;
}

vec3 Specular(POINT light, vec3 N, int light_index){
    vec3 L = normalize(light.position - vPos);
    vec3 V = normalize(-vPos);
    vec3 H = normalize(L + V);
    float NdotH = max(dot(N, H), 0.0);
    vec3 specular = materialSpecular * pow(NdotH, shininess) * shadowLightIntensity[light_index];


    return specular;
}

vec3 SpotLight(POINT light, SPOT spot)
{
    vec3 L = normalize(light.position - vPos);
    float spotFactor = dot(normalize(spot.direction), normalize(vPos - light.position));
    float intensity = smoothstep(spot.cutoff-0.1, spot.cutoff, spotFactor);
    if(intensity<=0.0) return vec3(0);
    return PointLight(light, normalize(vNormal)) * intensity * spot.exponent;

    /*
    if (spotFactor < spot.cutoff) 
    return vec3(0);
    float intensity = pow(spotFactor, spot.exponent);
    float smh = smoothstep(0.0, spot.cutoff, spotFactor);
    return max(PointLight(light, (normalize(vNormal)) / (spot.cutoff * spotFactor)*intensity) * smh, vec3(0.0));
    */
}

float lookup(vec2 offset, int shadow_index){

    float texelSz = 1.0/640;//texel of shadow map
    vec4 offset_f = vec4(offset * texelSz * shadowCoord[shadow_index].w, 0.0, 0.0);
    return textureProj(shadowMap[shadow_index], shadowCoord[shadow_index] + offset_f - 0.001 * vec4(vNormal, 0.0));

}

float Shadow(int shadow_index){
    float shadow = 1.0f;
    if (shadowCoord[shadow_index].w > 0.0){
        shadow = 0;
        float range = 2.5, step_ = 0.5;
        int samples = 0;
            for(float y = -range; y<=range ; y+=step_) //PCF
                for(float x = -range; x<=range; x+=step_){
                    shadow += lookup(vec2(x,y), shadow_index);
                    samples++;
                }
            shadow = (shadow / samples)+0.5;
     }
       // shadow = 0.5 + 0.5 * textureProj(shadowMap[shadow_index], shadowCoord[shadow_index] - 0.001 * vec4(vNormal, 0.0));
    return mix(1.0, shadow, shadowLightIntensity[shadow_index]);
}

void main()
{
    //Dematerializer
    if(dematerializerActive){
        vec2 noiseCoord0 = (vTex == vec2(0,0)) ? vPos.xy * 2.0 : vTex;
        float noise = texture(dematerializer, noiseCoord0).r;
        if(noise < t_dem)
        discard;
    }

    //shadow pre
    float shadow = 1.0;

    float shadow0 = Shadow(0);
    float shadow1 = Shadow(1);

    //outColor = vec4(0, 1.0f-shadow1, 0, 1);
    shadow = min(shadow0, shadow1);
    //color
    outColor = vec4(0.0);
    outColor+= vec4(materialAmbient * lightAmbient.color, 1.0);

    // directional
    outColor += vec4(materialDiffuse * lightDir.diffuse * max(dot(normalize(vNormal), normalize(-lightDir.direction)), 0.0), 1.0);

    // spotlight
    if(shadow0 > 0.5){
    outColor += vec4(SpotLight(lightPoint0, lightSpot0), 1.0);
    outColor += vec4(SpotLight(lightPoint1, lightSpot1), 1.0);
    }

    //GlobalSpecular
    outColor += vec4(Specular(lightPoint0, vNormal, 0) + Specular(lightPoint1, vNormal, 1), 0.0f); 

    //verify if it has texture
    vec4 tex = hasTex ? texture(texture0, vTex) : vec4(1.0);
    outColor *= tex;


    outColor = mix(vec4(fogColour, 1), outColor, fogFactor);//fog

    //shadow
    if(receiveShadows) outColor *= shadow;
    
}
