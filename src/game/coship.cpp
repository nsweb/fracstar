

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
	m_pCurrentLevel(nullptr),
    m_State(eShipState::Run),
    m_Speed(1.f),
	m_PathDistLevel(0.f)

{

}

CoShip::~CoShip()
{

}

void CoShip::Create( Entity* Owner, class json::Object* Proto )
{
	Super::Create( Owner, Proto );

	json::TokenIdx EntTok = Proto->GetToken( "entity", json::OBJECT );
	json::TokenIdx ShipTok = Proto->GetToken( "Ship", json::OBJECT, EntTok );
	if( ShipTok != INDEX_NONE )
	{
		json::TokenIdx ParamTok = Proto->GetToken( "speed", json::PRIMITIVE, ShipTok );
		if( ParamTok != INDEX_NONE )
			m_Speed = Proto->GetFloatValue( ParamTok, m_Speed );
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

void CoShip::Tick( TickContext& TickCtxt )
{
	if( !m_pCurrentLevel )
		return;

	//Name const& LevelName = m_pCurrentLevel->GetName();
	CoPath* CoP = static_cast<CoPath*>( m_pCurrentLevel->GetComponent( CoPath::StaticClass() ) );
    
    if( m_State == eShipState::Run )
    {
        m_PathDistLevel += TickCtxt.m_DeltaSeconds * m_Speed;
        if( m_PathDistLevel > CoP->m_ClampedSumDistance )
            m_PathDistLevel -= CoP->m_ClampedSumDistance;
    }
    
    vec3 Pos, Tan;
    CoP->InterpPath( m_PathDistLevel, Pos, Tan );
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

void CoShip::ChangeState( eShipState eNewState )
{
    m_State = eNewState;
}
