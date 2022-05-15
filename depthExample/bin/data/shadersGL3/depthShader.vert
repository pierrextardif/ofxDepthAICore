#version 150

// these are for the programmable pipeline system
uniform mat4 modelViewProjectionMatrix;

uniform sampler2DRect depth;
uniform float u_disparity;

in vec4 position;
in vec2 texcoord;

out vec2 texCoordVarying;



void main()
{
    texCoordVarying = texcoord;
    vec4 depthRGBA = texture2DRect(depth, texcoord);
	float z = u_disparity * (depthRGBA.r);
	gl_Position = modelViewProjectionMatrix * vec4(position.xy, z, position.w);
}