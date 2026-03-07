#version 330 core

uniform mat4 matrixProjection;
uniform mat4 matrixModelView;
uniform mat4 matrixShadow[2];
uniform mat4 matrixView;
uniform bool shadowPass;
uniform float fogDensity; //fog
//animations
#define MAX_BONES 100
uniform mat4 bones[MAX_BONES];
uniform bool animated;

in vec3 aVertex;
in vec3 aNormal;
in vec2 aTexCoord;

//animations
in vec3 aTangent;
in vec3 aBiTangent;
in ivec4 aBoneId; // Bone Ids
in vec4 aBoneWeight; // Bone Weights



out vec3 vPos;      
out vec3 vNormal;  
out vec2 vTex;
out vec4 shadowCoord[2];
out float fogFactor; //fog
void main()
{
    

    //animations
    mat4 matrixBone;
    if(aBoneWeight[0] == 0.0 || !animated) matrixBone = mat4(1);
    else{
    matrixBone = (bones[aBoneId[0]] * aBoneWeight[0] +
                    bones[aBoneId[1]] * aBoneWeight[1] +
                    bones[aBoneId[2]] * aBoneWeight[2] +
                    bones[aBoneId[3]] * aBoneWeight[3]);
    }

    vec4 posView = matrixModelView * matrixBone * vec4(aVertex, 1.0);
    vPos = posView.xyz;
    vNormal = normalize(mat3(matrixModelView) * mat3(matrixBone) * aNormal);
    vTex = aTexCoord;

    //shadow
    mat4 matrixModel = inverse(matrixView) * matrixModelView;
    shadowCoord[0] = matrixShadow[0] * matrixModel * matrixBone * vec4(aVertex, 1);
    shadowCoord[1] = matrixShadow[1] * matrixModel * matrixBone * vec4(aVertex, 1);

    
    //fog

    fogFactor = exp2(-fogDensity * length(posView.xyz));
    
    //Position
    gl_Position = matrixProjection * posView;
    
    


    
}
