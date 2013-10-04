#version 300 es

precision mediump float;
           
in vec2 v_texCoord;

layout(location = 0) out vec4 outColor;

uniform sampler2D s_texture;

void main()
{
	outColor = texture( s_texture, v_texCoord );
}