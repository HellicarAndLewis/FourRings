#extension GL_ARB_texture_rectangle : enable

//uniform vec2 resolution;
//uniform sampler2DRect gpuCompute0;
//uniform sampler2DRect tex;
uniform sampler2DRect texture0;
uniform sampler2DRect revealImg;
uniform vec3 color1;
uniform vec3 color2;
//uniform vec4 originalCol;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {
    float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
    return outVal;
}


void main(void) {
    
    float pct = texture2DRect(texture0, gl_TexCoord[0].xy).x;
    //if(pct > 0.5) {
        pct = map(pct, 0.5, 1.0, 0.0, 1.0);
    //}
    vec3 colReveal = texture2DRect(revealImg, gl_TexCoord[0].xy).xyz;
    vec3 color = (1.0-pct)*color1+pct*colReveal;
	gl_FragColor = vec4(color, 1.0);

}
