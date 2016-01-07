

#include "../fracstar.h"
#include "dfmanager.h"
//#include "coblocks.h"
#include "engine/controller.h"
#include "engine/camera.h"
#include "gfx/gfxmanager.h"
#include "gfx/shader.h"
#include "gfx/rendercontext.h"
#include "system/profiler.h"

#include "../game/fsworld.h"
#include "../game/colevel.h"
#include "../game/shipmanager.h"
#include "../game/coship.h"


STATIC_MANAGER_CPP(DFManager);

DFManager::DFManager() :
	m_df_vao(0),
	m_df_vbo(0)
	//m_df_shader(nullptr)
{
	m_pStaticInstance = this;
}

DFManager::~DFManager()
{
	m_pStaticInstance = nullptr;
}

void DFManager::Create()
{
	//m_df_shader = GfxManager::GetStaticInstance()->LoadShader( "df" );

	DFVertex ScreenVertices[] = { { vec2(-1.f,-1.f), vec2(-1.f,-1.f) }, { vec2(-1.f,1.f), vec2(-1.f,1.f) }, { vec2(1.f,1.f), vec2(1.f,1.f) },
								  { vec2(-1.f,-1.f), vec2(-1.f,-1.f) }, { vec2(1.f,1.f), vec2(1.f,1.f) },	{ vec2(1.f,-1.f), vec2(1.f,-1.f) } };

	glGenVertexArrays( 1, &m_df_vao);
	glBindVertexArray( m_df_vao);

	glGenBuffers(1, &m_df_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_df_vbo);
	glBufferData( GL_ARRAY_BUFFER, sizeof(ScreenVertices), ScreenVertices, GL_STATIC_DRAW );

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(DFVertex) /*stride*/, (void*)0 /*offset*/	);
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(DFVertex) /*stride*/, (void*)8 /*offset*/	);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void DFManager::Destroy()
{
	//m_df_shader = nullptr;
}


void DFManager::AddComponentToWorld( Component* pComponent )
{
	//if( pComponent->IsA( CoBlocks::StaticClass() ) )
	//{
	//	m_Blocks.push_back( reinterpret_cast<CoBlocks*>( pComponent ) );
	//}
}

void DFManager::RemoveComponentFromWorld( Component* pComponent )
{
	//if( pComponent->IsA( CoBlocks::StaticClass() ) )
	//{
	//	m_Blocks.remove( reinterpret_cast<CoBlocks*>( pComponent ) );
	//}
}

void DFManager::Tick( TickContext& TickCtxt )
{
	PROFILE_SCOPE( __FUNCTION__ );

	//for( int32 i = 0; i < m_Blocks.size(); ++i )
	//{
	//	m_Blocks[i]->Tick( TickCtxt );
	//}
}

void DFManager::_Render( RenderContext& RenderCtxt )
{
	PROFILE_SCOPE( __FUNCTION__ );
   
    CoLevel* level = FSWorld::GetStaticInstance()->GetCurrentLevel();
    if( !level )
        return;

	CoShip* ship = ShipManager::GetStaticInstance()->_GetShip();
	float ship_time = ship ? ship->m_path_knot_dist_level : 0.f;

	static float global_time = 0.f;
	global_time += RenderCtxt.m_delta_seconds;
    
    vec3 cam_pos = RenderCtxt.m_view.m_Transform.GetTranslation();
    mat4 view_inv_mat( RenderCtxt.m_view.m_Transform.GetRotation(), RenderCtxt.m_view.m_Transform.GetTranslation(), (float)RenderCtxt.m_view.m_Transform.GetScale() );
	
	const float fov_y = RenderCtxt.m_view.m_fParameters[eCP_FOV] * (F_PI / 180.0f);
	const float z_near = RenderCtxt.m_view.m_fParameters[eCP_NEAR];
	const float z_far = RenderCtxt.m_view.m_fParameters[eCP_FAR];

	vec2 screen_res;
	screen_res.y = bigball::tan( fov_y * 0.5f );
	screen_res.x = screen_res.y * RenderCtxt.m_view.m_fParameters[eCP_ASPECT];

	vec2 z_var;	
	z_var.x = (z_far + z_near) / (z_far - z_near);
	z_var.y = 2.0f*z_far*z_near / (z_far - z_near);

    Shader*	df_shader = level->m_df_shader;
    
	df_shader->Bind();
	ShaderUniform uni_gtime = df_shader->GetUniformLocation("global_time");
	df_shader->SetUniform( uni_gtime, global_time );
    ShaderUniform uni_camera = df_shader->GetUniformLocation("camera_pos");
    df_shader->SetUniform( uni_camera, cam_pos );
    ShaderUniform uni_view_inv = df_shader->GetUniformLocation("viewinv_mat");
    df_shader->SetUniform( uni_view_inv, view_inv_mat );
    ShaderUniform uni_proj = df_shader->GetUniformLocation("proj_mat");
    df_shader->SetUniform( uni_proj, RenderCtxt.m_proj_mat );

	ShaderUniform uni_sr = df_shader->GetUniformLocation("screen_res");
	df_shader->SetUniform( uni_sr, screen_res );
	ShaderUniform uni_zvar = df_shader->GetUniformLocation("z_var");
	df_shader->SetUniform( uni_zvar, z_var );

	level->InterpAndSetUniforms( ship_time );

	glBindVertexArray( m_df_vao );

	glDrawArrays( GL_TRIANGLES, 0, 8 );

	glBindVertexArray(0);

	df_shader->Unbind();
}

