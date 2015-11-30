#version 330

//layout (std140, binding = 0) uniform WVP
//{
	//uniform mat4 world_mat;
	uniform mat4 view_mat;
	uniform mat4 proj_mat;
//	uniform vec3 sun_dir;
//	uniform vec3 back_dir;
//};


layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

out vec4 vs_fs_color;

void main(void)
{
    gl_Position = proj_mat * (view_mat * /*world_mat **/ position);

	//vec2 vertexPosition_homoneneousspace = position.xy - vec2(400,300); // [0..800][0..600] -> [-400..400][-300..300]
 //   vertexPosition_homoneneousspace /= vec2(400,300);
 //   gl_Position =  vec4(vertexPosition_homoneneousspace,0,1);

    vs_fs_color = color;
}
