

#include "../fracstar.h"
#include "coship.h"
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

CLASS_EQUIP_CPP(CoShip);

CoShip::CoShip() :
	m_pcurrent_level(nullptr),
    m_state(eShipState::Run),
    m_speed(1.f),
	m_path_dist_level(0.f)

{

}

CoShip::~CoShip()
{

}

void CoShip::Create( Entity* owner, class json::Object* proto )
{
	Super::Create( owner, proto );

	json::TokenIdx EntTok = proto->GetToken( "entity", json::OBJECT );
	json::TokenIdx ShipTok = proto->GetToken( "Ship", json::OBJECT, EntTok );
	if( ShipTok != INDEX_NONE )
	{
		json::TokenIdx ParamTok = proto->GetToken( "speed", json::PRIMITIVE, ShipTok );
		if( ParamTok != INDEX_NONE )
			m_speed = proto->GetFloatValue( ParamTok, m_speed );
	}
}

void CoShip::Destroy()
{
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

	//Name const& LevelName = m_pCurrentLevel->GetName();
	CoPath* CoP = static_cast<CoPath*>( m_pcurrent_level->GetComponent( CoPath::StaticClass() ) );
    
    if( m_state == eShipState::Run )
    {
        m_path_dist_level += tick_ctxt.m_DeltaSeconds * m_speed;
        if( m_path_dist_level > CoP->m_clamped_sum_distance )
            m_path_dist_level -= CoP->m_clamped_sum_distance;
    }
    
    vec3 Pos, Tan;
    CoP->InterpPath( m_path_dist_level, Pos, Tan );
    Tan = bigball::normalize( Tan );
    
    static vec3 GUp( 0.f, 0.f, 1.f );
    mat3 CamRot;

    // Back
    CamRot.v2 = -Tan;
    // Right
    CamRot.v0 = bigball::normalize( bigball::cross(Tan, GUp) );
    // Up
    CamRot.v1 = bigball::cross(CamRot.v0, Tan);
    
    CoPosition* CoPos = static_cast<CoPosition*>( GetEntity()->GetComponent( CoPosition::StaticClass() ) );
    CoPos->SetPosition( Pos );
    quat Rot( CamRot );
    CoPos->SetRotation( Rot );

	// Retrieve current camp pos from path interpolation (centripetal catmull-rom spline)


	//CameraView const& CamView = Controller::GetStaticInstance()->GetRenderView();
	//dvec3 CamPos = CamView.m_Transform.GetTranslation();
	//dtransform const& EntityT = CoPos->GetTransform();
	//dvec3 CamPosBlock = EntityT.TransformPositionInverse( CamPos );
}

void CoShip::ChangeState( eShipState new_state )
{
    m_state = new_state;
}
