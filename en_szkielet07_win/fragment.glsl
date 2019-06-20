#version 330


//Varying variables
in vec4 l;
in vec4 l2;
in vec4 n;
in vec4 v;
in vec2 iTexCoord0;
in vec2 iTexCoord1;

uniform sampler2D textureMap0;
uniform sampler2D textureMap1;
uniform sampler2D textureMap2;

out vec4 pixelColor; //Fragment shader output variable

void main(void) {
    vec4 kd=vec4(0,0,0,1);
    vec4 ld=vec4(1,1,1,1); //Color of the diffused light
    vec4 ks=vec4(1,1,1,1); //Material color for a reflected light
    vec4 ls=vec4(1,1,1,1); //Color of the reflected light

    vec4 texColor=texture(textureMap0,iTexCoord0);
    vec4 texColor2=texture(textureMap1,iTexCoord1);
    vec4 texColor3=texture(textureMap2,iTexCoord0);
    kd = mix(texColor , mix(texColor2 , texColor3 , 0.75) , 0.25);
    ks = texColor3;

    vec4 ml=normalize(l);
    vec4 ml2=normalize(l2);
    vec4 mn=normalize(n);
    vec4 mv=normalize(v);
    vec4 mr=reflect(-ml,mn); //Reflection direction vector in the eye space
    vec4 mr2=reflect(-ml2,mn); //Reflection direction vector in the eye space

    float nl=clamp(dot(mn,ml),0,1); //cosine of the angle between n and l vectors
    float nl2=clamp(dot(mn,ml2),0,1); //cosine of the angle between n and l vectors
    float rv=pow(clamp(dot(mr,mv),0,1),25); //cosine of the angle between r and v vectors to the power of Phong exponent
    float rv2=pow(clamp(dot(mr2,mv),0,1),25); //cosine of the angle between r and v vectors to the power of Phong exponent

	vec4 co=vec4(kd.rgb*ld.rgb*nl+ks.rgb*ls.rgb*rv,kd.a);
    vec4 co2=vec4(kd.rgb*ld.rgb*nl2+ks.rgb*ls.rgb*rv2,kd.a);
	pixelColor=co*0.7+co2*0.7;
}
