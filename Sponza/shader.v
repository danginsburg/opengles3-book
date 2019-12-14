R"(

#version 300 es
precision highp float;
uniform mat4 u_mvpMatrix;
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texcoord;
out vec4 v_pos;
out vec2 v_texcoord;

void main()
{
   gl_Position = u_mvpMatrix * a_position;
   v_pos = gl_Position;
   v_texcoord = a_texcoord;
}

)"
