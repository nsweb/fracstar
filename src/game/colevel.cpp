

#include "../fracstar.h"
#include "colevel.h"
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
    m_df_shader(nullptr)
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
    
    LoadShader();
    
    CreateUniformVariables();
}
    
bool CoLevel::LoadShader()
{
    char const* src_buffers[Shader::MAX] = { nullptr };
    File shader_file;
    String vs_name = String::Printf( "../data/level/%s/df.vs.glsl", m_level_name.c_str() );
    String fs_name = String::Printf( "../data/level/%s/df.fs.glsl", m_level_name.c_str() );
    String vs_src, fs_src;
    
    if( !shader_file.Open( vs_name.c_str(), false /*bWrite*/) )
        return false;
    
    shader_file.SerializeString( vs_src );
    shader_file.Close();
    
    if( !shader_file.Open( fs_name.c_str(), false /*bWrite*/) )
        return false;
    
    shader_file.SerializeString( fs_src );
    shader_file.Close();
    
    src_buffers[Shader::Vertex] = vs_src.c_str();
    src_buffers[Shader::Fragment] = fs_src.c_str();
    
    m_df_shader = GfxManager::GetStaticInstance()->LoadShaderFromMemory( m_level_name.c_str(), src_buffers );
    
    return m_df_shader ? true : false;
}

void CoLevel::CreateUniformVariables()
{
    Array<ShaderUniformDetail> uniforms;
    m_df_shader->GetActiveUniforms( uniforms );

    String str_lvl("lvl_");
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
                case GL_INT:        new_track = new LevelVariableTrack<int>(); break;
                case GL_INT_VEC2:   new_track = new LevelVariableTrack<ivec2>(); break;
                case GL_INT_VEC3:   new_track = new LevelVariableTrack<ivec3>(); break;
                case GL_INT_VEC4:   new_track = new LevelVariableTrack<ivec4>(); break;
                default:
                    break;
            }
            
            if( new_track )
            {
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
                new_track->GetShaderUniformValue( m_df_shader, uni, &m_uniform_buffer[old_size] );
            }
        }
    }
}

void CoLevel::Destroy()
{
    m_df_shader = nullptr;
    
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


