

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
        if( m_path_dist_level > CoP->m_sum_distance )
            m_path_dist_level -= CoP->m_sum_distance;
    }
	m_path_dist_level = bigball::clamp( m_path_dist_level, 0.f, CoP->m_sum_distance );
    
    transform tf;
    CoP->InterpPathDist( m_path_dist_level, tf );

    CoPosition* CoPos = static_cast<CoPosition*>( GetEntity()->GetComponent( CoPosition::StaticClass() ) );
    CoPos->SetPosition( tf.GetTranslation() );
    CoPos->SetRotation( tf.GetRotation() );
}

void CoShip::ChangeState( eShipState new_state )
{
    m_state = new_state;
}
