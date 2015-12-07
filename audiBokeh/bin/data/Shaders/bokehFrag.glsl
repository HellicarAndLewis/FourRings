
uniform sampler2DRect tex;  
//uniform sampler2DRect blurAreas;
uniform float max_radius;  

void main(void) {  

    vec4 finalColor = vec4(0.0,0.0,0.0,1.0);  
    float weight = 0.;//vec4(0.,0.,0.,0.);  
    int radius = int(max_radius);
    //vec2 triggerCoord = gl_TexCoord[0].xy;
    //if(texture2DRect(blurAreas, triggerCoord).r > 0.0) {
        for(int x=radius*-1;x<radius;x+=3) {  
            for(int y=radius*-1;y<radius;y+=3){  
                vec2 coord = gl_TexCoord[0].xy+vec2(x,y);  
                if(distance(coord, gl_TexCoord[0].xy) < float(radius)){  
                    vec4 texel = texture2DRect(tex, coord);  
                    float w = length(texel.rgb)+0.1;  
                    weight+=w;  
                    finalColor += texel*w;  
                }  
            }  
        }
        gl_FragColor = finalColor/weight;  
    //} else {
      //  gl_FragColor = texture2DRect(tex, triggerCoord);
    //}
}  