#version 330

in vec4 vs_fs_color;

layout (location = 0) out vec4 color;

void main(void)
{
    //color = vec4(0.5, 0.5, 0.8, 1.0 );//vs_fs_color;
    color = vs_fs_color;
}
