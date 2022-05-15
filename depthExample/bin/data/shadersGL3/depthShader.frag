#version 150

uniform sampler2DRect rgb;
uniform sampler2DRect depth;

in vec2 texCoordVarying;

out vec4 outputColor;

void main()
{
    vec4 texel0 = texture2DRect(depth, texCoordVarying);
    vec4 texel1 = texture2DRect(rgb, texCoordVarying);
    outputColor = vec4(mix(texel0.rgb, texel1.bgr, .9), 1.0);
}