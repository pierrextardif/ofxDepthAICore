#version 150

uniform sampler2DRect rgb;
uniform sampler2DRect mask;

uniform vec2 u_cropResize;
uniform vec2 u_nnSize;
uniform vec2 u_previewSize;

in vec2 texCoordVarying;

out vec4 outputColor;


float remap(float xIn, vec2 scale1, vec2 scale2)
{
	return scale2.x + (scale2.y - scale2.x) * ((xIn - scale1.x) / (scale1.y - scale1.x));
}



void main()
{
    vec2 c = texCoordVarying;
    float human = 1.0;
    if(c.x >= u_cropResize.x && c.x <= u_cropResize.y){
		vec2 coords = vec2(remap(c.x, u_cropResize, vec2(0., 1.)) , c.y);
        human = texture2DRect(mask, coords * u_nnSize).r * 255.;
    }
    //vec4 texel0 = texture2DRect(mask, texCoordVarying / vec2(1920., 1080.) * vec2(256.));
    vec4 texel = texture2DRect(rgb, texCoordVarying * u_previewSize);
    outputColor = vec4(texel.bgr * human, 1.0);
}