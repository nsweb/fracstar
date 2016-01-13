#version 330

in vec3 vs_fs_view_dir;

layout (location = 0) out vec4 color;

void main(void)
{
    color = vec4(1.0,0.0,0.0,1.0);//vs_fs_color;
}
