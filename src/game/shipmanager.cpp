

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
		m_Ships.push_back( reinterpret_cast<CoShip*>( pComponent ) );
	}
}

void ShipManager::RemoveComponentFromWorld( Component* pComponent )
{
	if( pComponent->IsA( CoShip::StaticClass() ) )
	{
		m_Ships.remove( reinterpret_cast<CoShip*>( pComponent ) );
	}
}

void ShipManager::Tick( TickContext& TickCtxt )
{
	PROFILE_SCOPE( __FUNCTION__ );

	for( int32 i = 0; i < m_Ships.size(); ++i )
	{
		m_Ships[i]->Tick( TickCtxt );
	}
}

void ShipManager::_Render( RenderContext& RenderCtxt )
{
	PROFILE_SCOPE( __FUNCTION__ );

	//m_BlockShader->Bind();

	//ShaderUniform UniProj = m_BlockShader->GetUniformLocation("proj_mat");
	//m_BlockShader->SetUniform( UniProj, RenderCtxt.m_ProjMat );

	//vec3 SunDir( 1.f, 0.f, 0.f );
	//ShaderUniform UniSun = m_BlockShader->GetUniformLocation("sun_dir");
	//m_BlockShader->SetUniform( UniSun, SunDir );


	//for( int32 i = 0; i < m_Paths.size(); ++i )
	//{
	//	m_Blocks[i]->_Render( RenderCtxt, m_BlockShader, m_SampleShader );
	//}

	//m_BlockShader->Unbind();
}

