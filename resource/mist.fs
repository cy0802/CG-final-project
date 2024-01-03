#version 330 core
out vec4 FragColor;
vec2 iResolution = vec2(1300.0, 1000.0);
uniform float iTime;

float n21 (vec3 uvw)
{
    return fract(sin(uvw.x*23.35661 + uvw.y*6560.65 + uvw.z*4624.165)*2459.452);
}

float smoothNoise (vec3 uvw)
{
    float fbl = n21(floor(uvw));
    float fbr = n21(vec3(1.0,0.0,0.0)+floor(uvw));
    float ful = n21(vec3(0.0,1.0,0.0)+floor(uvw));
    float fur = n21(vec3(1.0,1.0,0.0)+floor(uvw));
    
    float bbl = n21(vec3(0.0,0.0,1.0)+floor(uvw));
    float bbr = n21(vec3(1.0,0.0,1.0)+floor(uvw));
    float bul = n21(vec3(0.0,1.0,1.0)+floor(uvw));
    float bur = n21(vec3(1.0,1.0,1.0)+floor(uvw));
    
    uvw = fract(uvw);
    vec3 blend = uvw;
    blend = (blend*blend*(3.0 -2.0*blend)); // cheap smoothstep
    
    return mix(	mix(mix(fbl, fbr, blend.x), mix(ful, fur, blend.x), blend.y),
        		mix(mix(bbl, bbr, blend.x), mix(bul, bur, blend.x), blend.y),
               	blend.z);
}

float perlinNoise (vec3 uvw)
{
    float blended = smoothNoise(uvw*4.0);
    blended += smoothNoise(uvw*8.0)*0.5;
    blended += smoothNoise(uvw*16.0)*0.25;
    blended += smoothNoise(uvw*32.0)*0.125;
    blended += smoothNoise(uvw*64.0)*0.0625;
    
    blended /= 2.0;
    //blended = fract(blended*2.0)*0.5+0.5;
    blended *= pow(0.8-abs(uvw.y),2.0);
    return blended;
}

void main()
{
    vec2 uv = gl_FragCoord.xy/iResolution.x-0.5*vec2(1.0,(iResolution.y/iResolution.x));
	
    
    vec3 uvw = vec3(uv, iTime*0.14);
    
    vec3 result = vec3(0.0);
    float moreDepth = 0.0;
    
    for(int i=0;i<20;i++)
    {
        moreDepth += 0.008;
        result += vec3(perlinNoise(uvw*vec3(vec2(moreDepth*12.0+1.0),1.0) + vec3(0.0,0.0, moreDepth)));
    }
    result /= 14.0;
    
    result *= 2.4*(0.6-length(uvw.xy));
    
    // Modify the alpha component based on iTime
    // float alphaFactor = pow(abs(sin(iTime * 0.5)), 2.0); // Adjust the power for the fade effect
    float alphaFactor = 0.5;
    FragColor = vec4(result * vec3(0.8, 0.9, 1.0) * 1.0 + vec3(0.2, 0.3, 0.4) * (0.9 - abs(uv.y)), 0.6 * alphaFactor);
}