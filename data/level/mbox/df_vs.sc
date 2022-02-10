
$input a_position, a_texcoord0
$output v_texcoord0

void main()
{
    gl_Position = a_position;
	v_texcoord0 = a_texcoord0;
}
