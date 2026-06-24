#version 420 core
in vec3 v_Color;
out vec4 FragColor;



void main() {
    //vec3 col = vec3(1,0,0);
    //FragColor = vec4(col, 1.0);
    FragColor = vec4(v_Color, 1.0);
}
