#version 330

//layout (std140, binding = 0) uniform WVP
//{
	uniform mat4 world_mat;
	uniform mat4 view_mat;
	uniform mat4 proj_mat;
	uniform mat4 viewtobox_mat;
	//uniform mat4 viewinv_mat;
//};


layout (location = 0) in vec4 position;
//layout (location = 2) in mat4 world_mat;

out vec3 vs_fs_view_dir;

void main(void)
{
	vec4 view_pos = (view_mat * (world_mat * position));
    gl_Position = proj_mat * view_pos;
	// Transform view vector back to local box
	vec4 view_dir = vec4( view_pos.xyz, 0.f );
	vec3 view_dir_local = (viewtobox_mat * view_dir).xyz;

	//vec3 camPos     = viewinv_mat[3].xyz;
 //   vec3 camRight   = viewinv_mat[0].xyz;
 //   vec3 camUp      = viewinv_mat[1].xyz;
 //   vec3 camDir     = -viewinv_mat[2].xyz;
	
	vs_fs_view_dir = normalize( view_dir_local );
		//vs_fs_texcoord.xy * screen_res * 1.0;
	
	// Get direction for this pixel
	//vec3 rayDir = normalize(camDir + screen_coord.x*camRight + screen_coord.y*camUp);

}
