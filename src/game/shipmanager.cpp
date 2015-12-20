

#include "../fracstar.h"
#include "shipmanager.h"
#include "CoShip.h"
#include "engine/controller.h"
#include "engine/camera.h"
#include "gfx/gfxmanager.h"
#include "gfx/shader.h"
#include "gfx/rendercontext.h"
#include "system/profiler.h"


STATIC_MANAGER_CPP(ShipManager);

ShipManager::ShipManager()
	//m_BlockShader(nullptr),
{
	m_pStaticInstance = this;
}

ShipManager::~ShipManager()
{
	m_pStaticInstance = nullptr;
}

void ShipManager::Create()
{
	//m_BlockShader = GfxManager::GetStaticInstance()->LoadShader( "block" );
}

void ShipManager::Destroy()
{
	//m_BlockShader = nullptr;
}


void ShipManager::AddComponentToWorld( Component* pComponent )
{
	if( pComponent->IsA( CoShip::StaticClass() ) )
	{
		m_ships.push_back( reinterpret_cast<CoShip*>( pComponent ) );
	}
}

void ShipManager::RemoveComponentFromWorld( Component* pComponent )
{
	if( pComponent->IsA( CoShip::StaticClass() ) )
	{
		m_ships.remove( reinterpret_cast<CoShip*>( pComponent ) );
	}
}

void ShipManager::Tick( TickContext& TickCtxt )
{
	PROFILE_SCOPE( __FUNCTION__ );

	for( int32 i = 0; i < m_ships.size(); ++i )
	{
		m_ships[i]->Tick( TickCtxt );
	}
}

void ShipManager::_Render( RenderContext& RenderCtxt )
{
	PROFILE_SCOPE( __FUNCTION__ );


}

