

#include "../fracstar.h"
#include "coship.h"
#include "levelshader.h"

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
#include "gfx/drawutils.h"
#include "../engine/copath.h"
#include "../engine/dfmanager.h"

CLASS_EQUIP_CPP(CoShip);

CoShip::CoShip() :
	m_pcurrent_level(nullptr),
    m_state(eShipState::Run),
    m_speed(1.f),
    m_speed_lateral(0.1f),
    m_cam_zoffset(0.05f),
    m_ship_scale(0.005f),
    m_move_range(0.03f),
	m_path_dist_level(0.f),
	m_current_collision_dist(1e8f)
{

}

CoShip::~CoShip()
{

}

void CoShip::Create( Entity* owner, class json::Object* proto )
{
	Super::Create( owner, proto );

	json::TokenIdx ent_tok = proto->GetToken( "entity", json::OBJECT );
	json::TokenIdx ship_tok = proto->GetToken( "Ship", json::OBJECT, ent_tok );
	if( ship_tok != INDEX_NONE )
	{
		json::TokenIdx param_tok = proto->GetToken( "speed", json::PRIMITIVE, ship_tok );
		if( param_tok != INDEX_NONE )
			m_speed = proto->GetFloatValue( param_tok, m_speed );
        param_tok = proto->GetToken( "cam_zoffset", json::PRIMITIVE, ship_tok );
        if( param_tok != INDEX_NONE )
            m_cam_zoffset = proto->GetFloatValue( param_tok, m_cam_zoffset );
        param_tok = proto->GetToken( "ship_scale", json::PRIMITIVE, ship_tok );
        if( param_tok != INDEX_NONE )
            m_ship_scale = proto->GetFloatValue( param_tok, m_ship_scale );
        param_tok = proto->GetToken( "move_range", json::PRIMITIVE, ship_tok );
        if( param_tok != INDEX_NONE )
            m_move_range = proto->GetFloatValue( param_tok, m_move_range );
	}
    
    m_ship_cam_pos = vec3( 0.f, 0.f, -m_cam_zoffset);

	m_ship_shader = GfxManager::GetStaticInstance()->LoadShader( "ship" );
}

void CoShip::Destroy()
{
	m_ship_shader = nullptr;

	Super::Destroy();
}

void CoShip::AddToWorld()
{
	Super::AddToWorld();
}

void CoShip::RemoveFromWorld()
{
	Super::RemoveFromWorld();
}

void CoShip::Tick( TickContext& tick_ctxt )
{
	if( !m_pcurrent_level )
		return;

	CoPath* ppath = static_cast<CoPath*>( m_pcurrent_level->GetComponent( CoPath::StaticClass() ) );
    
    if( m_state == eShipState::Run )
    {
        m_path_dist_level += tick_ctxt.m_delta_seconds * m_speed;
        if( m_path_dist_level > ppath->m_sum_distance )
            m_path_dist_level -= ppath->m_sum_distance;
        
        m_path_dist_level = bigball::clamp( m_path_dist_level, 0.f, ppath->m_sum_distance );
        m_path_knot_dist_level = ppath->ConvertDistanceToKnot(m_path_dist_level);
        
    }
    else
    {
        // Ensure path dist in sync with knot dist
        m_path_knot_dist_level = bigball::clamp( m_path_knot_dist_level, 0.f, ppath->m_sum_knot_distance );
        m_path_dist_level = ppath->ConvertKnotToDistance(m_path_knot_dist_level);
    }
    
    ppath->InterpPathKnotDist( m_path_knot_dist_level, m_path_frame );

	// Ship position is relative to cam
    CoPosition* ppos = static_cast<CoPosition*>( GetEntityComponent( CoPosition::StaticClass() ) );
    ppos->SetPosition( m_ship_cam_pos );
	ppos->SetRotation(quat(1.f) /*m_path_frame.GetRotation()*/);

	// Check collisions
	{
		vec3 ship_world_pos = m_path_frame.TransformPosition(m_ship_cam_pos);
		m_current_collision_dist = LevelShader::GetStaticInstance()->EstimateLevelDistance(m_pcurrent_level->GetName(), ship_world_pos);
        //DrawUtils::GetStaticInstance()->PushAABB(ship_world_pos, level_dist*0.5f, u8vec4(255,0,0,124));
        
        static float cube_size = 0.01f;
        vec3 front_dir = m_path_frame.TransformVector( vec3(0.f,0.f,-1.f) );
        vec3 right_dir = m_path_frame.TransformVector( vec3(1.f,0.f,0.f) );
        vec3 up_dir = m_path_frame.TransformVector( vec3(0.f,1.f,0.f) );
        {
            vec3 ray_pos = ship_world_pos;// + vec3 (0.f, 0.m_ship_scale
            vec3 front_pos = LevelShader::GetStaticInstance()->RayMarch(m_pcurrent_level->GetName(), ray_pos, front_dir, 10.f);
            DrawUtils::GetStaticInstance()->PushAABB(front_pos, cube_size, u8vec4(255,0,0,124));
        }
        {
            vec3 ray_pos = ship_world_pos;// + vec3 (0.f, 0.m_ship_scale
            vec3 right_pos = LevelShader::GetStaticInstance()->RayMarch(m_pcurrent_level->GetName(), ray_pos, right_dir, 10.f);
            DrawUtils::GetStaticInstance()->PushAABB(right_pos, cube_size, u8vec4(0,255,0,124));
        }
        {
            vec3 ray_pos = ship_world_pos;// + vec3 (0.f, 0.m_ship_scale
            vec3 down_pos = LevelShader::GetStaticInstance()->RayMarch(m_pcurrent_level->GetName(), ray_pos, -up_dir, 10.f);
            DrawUtils::GetStaticInstance()->PushAABB(down_pos, cube_size, u8vec4(0,0,255,124));
        }
	}
}

bool CoShip::OnControllerInput( Camera* pcamera, ControllerInput const& input )
{
    m_ship_cam_pos.x = bigball::clamp( m_ship_cam_pos.x + input.m_delta.x * m_speed_lateral, -m_move_range, m_move_range );
    m_ship_cam_pos.y = bigball::clamp( m_ship_cam_pos.y + input.m_delta.y * m_speed_lateral, -m_move_range, m_move_range );
    
    return true;
}

void CoShip::_Render( RenderContext& render_ctxt )
{
	static float global_time = 0.f;
	global_time += render_ctxt.m_delta_seconds;

	transform cam2world_transform( render_ctxt.m_view.m_Transform.GetRotation(), render_ctxt.m_view.m_Transform.GetTranslation(), (float)render_ctxt.m_view.m_Transform.GetScale() );

	//vec3 cam_pos = cam2world_transform.GetTranslation();
	mat4 view_inv_mat( cam2world_transform.GetRotation(), cam2world_transform.GetTranslation(), cam2world_transform.GetScale() );

	CoPosition* ppos = static_cast<CoPosition*>( GetEntityComponent( CoPosition::StaticClass() ) );
	transform ship2cam_transform = ppos->GetTransform();	// relative to cam
	transform ship_transform = cam2world_transform * ship2cam_transform;
	mat4 world_mat( ship_transform.GetRotation(), ship_transform.GetTranslation(), (float)ship_transform.GetScale() * m_ship_scale );
	mat4 view_mat = bigball::inverse(view_inv_mat);
	mat4 world_view_mat = view_mat * world_mat;
	static float coll_threshold = 0.05f;
	float collision_dist = clamp(m_current_collision_dist / coll_threshold, 0.0f, 1.0f);

	// Draw reverse faces, so that we can still display something inside cube
	glCullFace(GL_FRONT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_ship_shader->Bind();
	{
		ShaderUniform uni_world = m_ship_shader->GetUniformLocation("world_mat");
		m_ship_shader->SetUniform( uni_world, world_mat );
		ShaderUniform uni_view = m_ship_shader->GetUniformLocation("view_mat");
		m_ship_shader->SetUniform( uni_view, view_mat );
		ShaderUniform uni_proj = m_ship_shader->GetUniformLocation("proj_mat");
		m_ship_shader->SetUniform( uni_proj, render_ctxt.m_proj_mat );
		ShaderUniform uni_vtb = m_ship_shader->GetUniformLocation("viewtobox_mat");
		m_ship_shader->SetUniform( uni_vtb, bigball::inverse(world_view_mat) );
		ShaderUniform uni_cdist = m_ship_shader->GetUniformLocation("collision_dist");
		m_ship_shader->SetUniform(uni_cdist, collision_dist);
		
		DFManager::GetStaticInstance()->DrawCube();
	}
	m_ship_shader->Unbind();

	glCullFace(GL_BACK);
}

void CoShip::ChangeState( eShipState new_state )
{
    m_state = new_state;
}

void CoShip::SetCurrentLevel( Entity* pcurrent_level )
{
    m_pcurrent_level = pcurrent_level;
    
    m_path_dist_level = 0.f;
    m_path_knot_dist_level = 0.f;
}
