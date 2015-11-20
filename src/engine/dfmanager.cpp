

#include "../fracstar.h"
#include "dfmanager.h"
//#include "coblocks.h"
#include "engine/controller.h"
#include "engine/camera.h"
#include "gfx/gfxmanager.h"
#include "gfx/shader.h"
#include "gfx/rendercontext.h"
#include "system/profiler.h"


STATIC_MANAGER_CPP(DFManager);

DFManager::DFManager() :
	m_DFShader(nullptr),
	m_SampleShader(nullptr)
{
	m_pStaticInstance = this;
}

DFManager::~DFManager()
{
	m_pStaticInstance = nullptr;
}

void DFManager::Create()
{
	m_DFShader = GfxManager::GetStaticInstance()->LoadShader( "df" );
	m_SampleShader = GfxManager::GetStaticInstance()->LoadShader( "samples" );
}

void DFManager::Destroy()
{
	m_DFShader = nullptr;
	m_SampleShader = nullptr;
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

	//m_BlockShader->Bind();

	//ShaderUniform UniProj = m_BlockShader->GetUniformLocation("proj_mat");
	//m_BlockShader->SetUniform( UniProj, RenderCtxt.m_ProjMat );

	//vec3 SunDir( 1.f, 0.f, 0.f );
	//ShaderUniform UniSun = m_BlockShader->GetUniformLocation("sun_dir");
	//m_BlockShader->SetUniform( UniSun, SunDir );


	//for( int32 i = 0; i < m_Blocks.size(); ++i )
	//{
	//	m_Blocks[i]->_Render( RenderCtxt, m_BlockShader, m_SampleShader );
	//}

	//m_BlockShader->Unbind();
}

