#version 330

in vec3 vs_fs_pos;
in vec3 vs_fs_view_dir;

layout (location = 0) out vec4 color;

void main(void)
{
	vec3 view_dir = normalize( vs_fs_view_dir );

	// Plane -Z
	vec3 p_NZ = vec3(0.f, 0.f, -1.f);
	vec3 n_NZ = vec3(0.f, 0.f, 1.f);
	vec3 p_pos = vs_fs_pos - p_NZ;


    color = vec4(vs_fs_pos * 0.5 + 0.5,1.0);//vs_fs_color;
}
