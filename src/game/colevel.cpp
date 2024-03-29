

#include "../fracstar.h"
#include "colevel.h"
#include "levelshader.h"
#include "../engine/copath.h"
#include "core/json.h"
#include "system/file.h"
#include "engine/coposition.h"
#include "engine/controller.h"
#include "engine/camera.h"
#include "engine/tickcontext.h"
#include "engine/entitymanager.h"
#include "gfx/gfxmanager.h"
#include "gfx/shader.h"
#include "gfx/rendercontext.h"

CLASS_EQUIP_CPP(CoLevel);

CoLevel::CoLevel() :
    m_df_program(BGFX_INVALID_HANDLE),
	m_hit_program(BGFX_INVALID_HANDLE)
{

}

CoLevel::~CoLevel()
{

}

void CoLevel::Create( Entity* owner, class json::Object* proto )
{
	Super::Create( owner, proto );
    
    m_level_name = GetEntity()->GetName();

	/*json::TokenIdx EntTok = proto->GetToken( "entity", json::OBJECT );
	json::TokenIdx ShipTok = proto->GetToken( "Ship", json::OBJECT, EntTok );
	if( ShipTok != INDEX_NONE )
	{
		json::TokenIdx ParamTok = proto->GetToken( "speed", json::PRIMITIVE, ShipTok );
		if( ParamTok != INDEX_NONE )
			m_speed = proto->GetFloatValue( ParamTok, m_speed );
	}*/
    
    LoadShaders();
    
    CreateUniformVariables();
}
    
bool CoLevel::LoadShaders()
{
	// Load distance field shader
    m_df_vsh = loadShader("df_vs");
    m_df_fsh = loadShader("df_fs");

    // Create program from shaders.
    m_df_program = bgfx::createProgram(m_df_vsh, m_df_fsh, true /* destroy shaders when program is destroyed */);

	// Load hit test shader
#if 0
	String cs_name = String::Printf( "../data/level/%s/hittest.cs.glsl", m_level_name.c_str() );
	String cs_src;

	if( !shader_file.Open( cs_name.c_str(), false /*bWrite*/) )
		return false;

	shader_file.SerializeString( cs_src );
	shader_file.Close();
	src_buffers[Shader::Vertex] = nullptr;
	src_buffers[Shader::Fragment] = nullptr;
	src_buffers[Shader::Compute] = cs_src.c_str();

	String hit_shader_name = String::Printf( "%s_hit", m_level_name.c_str() );
	m_hit_shader = GfxManager::GetStaticInstance()->LoadShaderFromMemory( hit_shader_name.c_str(), src_buffers );
#endif

    return isValid(m_df_program) /*&& m_hit_shader*/ ? true : false;
}

void CoLevel::CreateUniformVariables()
{
    // (REBIND)
#if 0
    Array<ShaderUniformDetail> uniforms;
    m_df_shader->GetActiveUniforms( uniforms );

    String str_lvl("u_lvl_");
    for( int uni_idx = 0; uni_idx < uniforms.size(); uni_idx++ )
    {
        ShaderUniformDetail const& uni = uniforms[uni_idx];
        String str_name = uni.m_name.c_str();
        if( str_name.StartsWith( str_lvl ) )
        {
            LevelVariableTrackBase* new_track = nullptr;
            //m_var_tracks
            switch (uni.m_type)
            {
                case GL_FLOAT:      new_track = new LevelVariableTrack<float>(); break;
                case GL_FLOAT_VEC2: new_track = new LevelVariableTrack<vec2>(); break;
                case GL_FLOAT_VEC3: new_track = new LevelVariableTrack<vec3>(); break;
                case GL_FLOAT_VEC4: new_track = new LevelVariableTrack<vec4>(); break;
                //case GL_INT:        new_track = new LevelVariableTrack<int>(); break;
                //case GL_INT_VEC2:   new_track = new LevelVariableTrack<ivec2>(); break;
                //case GL_INT_VEC3:   new_track = new LevelVariableTrack<ivec3>(); break;
                //case GL_INT_VEC4:   new_track = new LevelVariableTrack<ivec4>(); break;
                default:
                    break;
            }
            
            if( new_track )
            {
				new_track->m_var_name = str_name.c_str();
                m_var_tracks.push_back(new_track);
                new_track->m_var_index = m_shader_variables.size();
                
                const int req_size = new_track->GetVarSize();
                const int old_size = m_uniform_buffer.size();
                
                UniformVariable new_var;
                ShaderUniformDetail* new_var_detail = &new_var;
                *new_var_detail = uni;
                new_var.m_buffer_offset = old_size;
                m_shader_variables.push_back(new_var);
                m_uniform_buffer.resize(old_size + req_size);
                
                // Get default value
                new_track->RetrieveShaderUniformValue( m_df_shader, uni, &m_uniform_buffer[old_size] );
            }
#define TEST_UNI_TRACK	1
#if TEST_UNI_TRACK
			if( str_name == "lvl_mb_scale" )
			{
				LevelVariableTrack<float>* test_track = static_cast<LevelVariableTrack<float>*>( new_track );
				float* default_value = GetUniformBufferValue<float>(new_track->m_var_index);
				const int key_count = 40;
				for( int key_idx = 0; key_idx < key_count; key_idx++ )
				{
					float curve_val = bigfx::sin( 8.f * F_PI * (float)key_idx / key_count );
					LevelVariableTrack<float>::Key new_key = { (float)key_idx * 0.5f, *default_value + 0.15f*curve_val };
					test_track->m_keys.push_back( new_key );
				}
				
			}
			else if( str_name == "lvl_min_radius2" )
			{
				LevelVariableTrack<float>* test_track = static_cast<LevelVariableTrack<float>*>( new_track );
				float* default_value = GetUniformBufferValue<float>(new_track->m_var_index);
				const int key_count = 40;
				for( int key_idx = 0; key_idx < key_count; key_idx++ )
				{
					float curve_val = bigfx::cos( 8.f * F_PI * (float)key_idx / key_count );
					LevelVariableTrack<float>::Key new_key = { (float)key_idx * 0.5f, *default_value + 0.15f*curve_val };
					test_track->m_keys.push_back( new_key );
				}

			}
#endif // TEST_UNI_TRACK
        }
    }
#endif
}

void CoLevel::Destroy()
{
    if (isValid(m_df_program))
        bgfx::destroy(m_df_program);

    if (isValid(m_hit_program))
        bgfx::destroy(m_hit_program);
    
	Super::Destroy();
}

void CoLevel::AddToWorld()
{
    
	Super::AddToWorld();
}

void CoLevel::RemoveFromWorld()
{
	Super::RemoveFromWorld();
}

void CoLevel::Tick( TickContext& tick_ctxt )
{

}

void CoLevel::InterpAndSetUniforms( float time )
{
	for( int track_idx = 0; track_idx < m_var_tracks.size(); track_idx++ )
	{
		m_var_tracks[track_idx]->InterpUniformValue( time, this );
		m_var_tracks[track_idx]->SetShaderUniformValue( m_df_fsh, this );

		LevelShader::GetStaticInstance()->SetCppUniformValue(this, m_var_tracks[track_idx]->m_var_name, m_var_tracks[track_idx]->m_var_index);
	}
}


