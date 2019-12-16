R"(

#version 320 es
precision mediump float;
in vec4 v_pos;
in vec2 v_texcoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D s_texture;
void main()
{
    //outColor = vec4(15.0f, 15.0f, 15.0f, 15.0f) * light_factor / (4.0f * 3.1415926 * dis * dis);
    outColor = texture( s_texture, v_texcoord );
}

)"
