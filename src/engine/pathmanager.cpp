

#include "../fracstar.h"
#include "pathmanager.h"
#include "copath.h"
#include "engine/controller.h"
#include "engine/camera.h"
#include "gfx/gfxmanager.h"
#include "gfx/shader.h"
#include "gfx/rendercontext.h"
#include "system/profiler.h"


STATIC_MANAGER_CPP(PathManager);

PathManager::PathManager()
	//m_BlockShader(nullptr),
{
	m_pStaticInstance = this;
}

PathManager::~PathManager()
{
	m_pStaticInstance = nullptr;
}

void PathManager::Create()
{
	//m_BlockShader = GfxManager::GetStaticInstance()->LoadShader( "block" );
}

void PathManager::Destroy()
{
	//m_BlockShader = nullptr;
}


void PathManager::AddComponentToWorld( Component* pComponent )
{
	if( pComponent->IsA( CoPath::StaticClass() ) )
	{
		m_paths.push_back( reinterpret_cast<CoPath*>( pComponent ) );
	}
}

void PathManager::RemoveComponentFromWorld( Component* pComponent )
{
	if( pComponent->IsA( CoPath::StaticClass() ) )
	{
		m_paths.remove( reinterpret_cast<CoPath*>( pComponent ) );
	}
}

void PathManager::Tick( TickContext& TickCtxt )
{
	PROFILE_SCOPE( __FUNCTION__ );

	for( int32 i = 0; i < m_paths.size(); ++i )
	{
		m_paths[i]->Tick( TickCtxt );
	}
}

void PathManager::_Render( RenderContext& RenderCtxt )
{
	PROFILE_SCOPE( __FUNCTION__ );
    
    for( int32 i = 0; i < m_paths.size(); ++i )
    {
        m_paths[i]->_DrawDebug( RenderCtxt );
    }

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

