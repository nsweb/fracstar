
$input a_position
$output v_pos, v_viewdir

#include <bgfx_shader.sh>

void main()
{
	vec4 view_pos = mul(u_modelView, vec4(a_position, 1.0)); //mul(u_view, mul(u_model[0], vec4(a_position, 1.0)));
    gl_Position = mul(u_proj, view_pos);
	// Transform view vector back to local box
	//vec4 view_dir = vec4( view_pos.xyz, 0.f );
	//vec3 view_dir_local = mul(u_viewtobox_mat, view_dir).xyz;
	vec3 view_dir_local = mul(view_pos, ((mat3)u_modelView));

	v_pos = a_position.xyz;
	v_viewdir = normalize( view_dir_local );
}
