

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


