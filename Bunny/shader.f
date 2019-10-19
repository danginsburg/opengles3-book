R"(

#version 300 es
precision mediump float;
in vec4 v_pos;
in vec4 v_lightpos;
in vec3 v_normal;
layout(location = 0) out vec4 outColor;
void main()
{
    float dis = distance(v_lightpos, v_pos);
    vec3 tolight = normalize(vec3(v_lightpos) - vec3(v_pos));
    float light_factor = dot(tolight, v_normal);
    outColor = vec4(15.0f, 15.0f, 15.0f, 15.0f) * light_factor / (4.0f * 3.1415926 * dis * dis);
}

)"
