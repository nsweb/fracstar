

#include "../fracstar.h"
#include "dfmanager.h"
//#include "coblocks.h"
#include "engine/controller.h"
#include "engine/camera.h"
#include "gfx/rendercontext.h"

#include "../game/fsworld.h"
#include "../game/colevel.h"
#include "../game/shipmanager.h"
#include "../game/coship.h"

bgfx::VertexLayout DFManager::DFVertex::ms_layout;

STATIC_MANAGER_CPP(DFManager);

DFManager::DFManager()
{
	m_pStaticInstance = this;
}

DFManager::~DFManager()
{
	m_pStaticInstance = nullptr;
}

void DFManager::Create()
{
	static DFVertex screen_vertices[] = { { vec2(-1.f,-1.f), vec2(-1.f,-1.f) }, { vec2(1.f,1.f), vec2(1.f,1.f) }, { vec2(-1.f,1.f), vec2(-1.f,1.f) },
								  { vec2(-1.f,-1.f), vec2(-1.f,-1.f) },	{ vec2(1.f,-1.f), vec2(1.f,-1.f) }, { vec2(1.f,1.f), vec2(1.f,1.f) } };

	// Create vertex stream declaration.
	DFVertex::init();

	// Create static vertex buffer.
	m_vbh_fullscreen_quad = bgfx::createVertexBuffer(bgfx::makeRef(screen_vertices, sizeof(screen_vertices)),
		DFVertex::ms_layout);

	u_z_var_screen_res = bgfx::createUniform("u_z_var_screen_res", bgfx::UniformType::Vec4);
	u_camera_pos = bgfx::createUniform("u_camera_pos", bgfx::UniformType::Vec4);
}

void DFManager::Destroy()
{
	bgfx::destroy(u_z_var_screen_res);
	bgfx::destroy(u_camera_pos);

	// Cleanup.
	bgfx::destroy(m_vbh_fullscreen_quad);
}

void DFManager::AddComponentToWorld( Component* pComponent )
{

}

void DFManager::RemoveComponentFromWorld( Component* pComponent )
{

}

void DFManager::Tick( TickContext& tick_ctxt )
{
	// (REBIND) BGFX_PROFILER_SCOPE is only for bgfx code https://github.com/bkaradzic/bgfx/issues/566
	//BGFX_PROFILER_SCOPE( __FUNCTION__, 0xffffffff );
}

void DFManager::_Render( RenderContext& render_ctxt )
{
	// (REBIND)
	//BGFX_PROFILER_SCOPE(__FUNCTION__, 0xffffffff);

    CoLevel* level = FSWorld::GetStaticInstance()->GetCurrentLevel();
    if( !level )
        return;

	CoShip* ship = ShipManager::GetStaticInstance()->_GetShip();
	float ship_time = ship ? ship->m_path_knot_dist_level : 0.f;

	static float global_time = 0.f;
	global_time += render_ctxt.m_delta_seconds;
    
    vec3 cam_pos = render_ctxt.m_view.m_transform.GetTranslation();
    mat4 view_inv_mat( render_ctxt.m_view.m_transform.GetRotation(), render_ctxt.m_view.m_transform.GetTranslation(), (float)render_ctxt.m_view.m_transform.GetScale() );
	
	const float fov_y = render_ctxt.m_view.m_parameters[eCP_FOV] * (F_PI / 180.0f);
	const float z_near = render_ctxt.m_view.m_parameters[eCP_NEAR];
	const float z_far = render_ctxt.m_view.m_parameters[eCP_FAR];

	vec2 screen_res;
	screen_res.y = bigfx::tan( fov_y * 0.5f );
	screen_res.x = screen_res.y * render_ctxt.m_view.m_parameters[eCP_ASPECT];

	vec2 z_var;	
	z_var.x = (z_far + z_near) / (z_far - z_near);
	z_var.y = 2.0f*z_far*z_near / (z_far - z_near);

	bgfx::ProgramHandle	df_program = level->m_df_program;
	//bgfx::ShaderHandle	df_vsh = level->m_df_fsh;

	// Set view and projection matrix for view 0
	bgfx::setViewTransform(0, &render_ctxt.m_view_mat.v0, &render_ctxt.m_proj_mat.v0);

	float z_var_screen_res[4] = { z_var.x, z_var.y, screen_res.x, screen_res.y };
	bgfx::setUniform(u_z_var_screen_res, z_var_screen_res);
	float cam_pos_4[4] = { cam_pos.x, cam_pos.y, cam_pos.z, 0.f };
	bgfx::setUniform(u_camera_pos, cam_pos_4);

	level->InterpAndSetUniforms( ship_time );

	bgfx::setState(BGFX_STATE_DEFAULT);
	bgfx::setVertexBuffer(0, m_vbh_fullscreen_quad);
	bgfx::submit(0, df_program);
}


