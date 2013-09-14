#version 300 es

precision mediump float;                

in vec2 v_texCoord;                     

layout(location = 0) out vec4 outColor;                     

uniform sampler2D s_baseMap;            
uniform sampler2D s_lightMap;           

void main()                             
{                                       
	vec4 baseColor;                       
  	vec4 lightColor;                      
                                        
  	baseColor = texture( s_baseMap, v_texCoord ); 
  	lightColor = texture( s_lightMap, v_texCoord );

  	outColor = baseColor * (lightColor + 0.25);   
}