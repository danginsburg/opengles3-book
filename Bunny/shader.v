R"(

#version 300 es
precision highp float;
uniform mat4 u_mvpMatrix;
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec4 a_lightpos;
out vec4 v_pos;
out vec4 v_lightpos;
out vec3 v_normal;

void main()
{
   v_lightpos = u_mvpMatrix * a_lightpos;
   gl_Position = u_mvpMatrix * a_position;
   v_pos = gl_Position;
   v_normal = normalize(mat3(u_mvpMatrix) * a_normal);   
}

)"
