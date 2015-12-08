#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DRect texture0;
uniform sampler2DRect background;
uniform vec3 foregroundColor;

void main() {
    float pct = texture2DRect(texture0, gl_TexCoord[0].xy).x;
    vec3 colForeground = vec3(foregroundColor.r/255, foregroundColor.g/255, foregroundColor.b/255); //texture2DRect(texture0, gl_TexCoord[0].xy).xyz;
    vec3 colBackground = texture2DRect(background, gl_TexCoord[0].xy).xyz;
    vec3 color = (1.0-pct)*colBackground + pct*colForeground;
    gl_FragColor = vec4(color, 1.0);
}