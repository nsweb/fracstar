// engine.cpp
//

#include "../fracstar.h"
#include "engine/entity.h"
#include "engine/entitymanager.h"
#include "engine/controller.h"
//#include "coblocks.h"
//#include "blocksmanager.h"
//#include "../core/datastreammanager.h"
//#include "codposition.h"
#include "fscamera.h"
//#include "../cmd/cmdbuilddata.h"

FSEngine::FSEngine()
{

}

FSEngine::~FSEngine()
{

}

bool FSEngine::Init( bool bCreateWindow )
{
	bool bInit = Engine::Init( bCreateWindow );

	//////////////////////////////////////////////////////////////////////////
	// Scene description
	Entity* pPlanet = EntityManager::GetStaticInstance()->CreateEntityFromJson( "../data/planet.json", "Planet" );
	EntityManager::GetStaticInstance()->AddEntityToWorld( pPlanet );

#if 0
	Entity* pSun = EntityManager::GetStaticInstance()->CreateEntityFromJson( "../data/sun.json", "Sun" );
	EntityManager::GetStaticInstance()->AddEntityToWorld( pSun );
	CoDPosition* pSunCoPos = static_cast<CoDPosition*>( pSun->GetComponent( CoDPosition::StaticClass() ) );
	if( pSunCoPos )
	{
		pSunCoPos->SetPosition( dvec3( 0, -162e9, -81e9 ) );	// 230 million km
	}

	Entity* pEntCamera = EntityManager::GetStaticInstance()->CreateEntityFromJson( "../data/defaultcamera.json" );
	EntityManager::GetStaticInstance()->AddEntityToWorld( pEntCamera );
	if( pEntCamera->IsA( Camera::StaticClass() ) )
	{
		double CameraDistance = 1000.0;
		CoBlocks* pPlanetCoBlocks = static_cast<CoBlocks*>( pPlanet ? pPlanet->GetComponent( CoBlocks::StaticClass() ) : nullptr );
		if( pPlanetCoBlocks )
		{
			double OctreeSize = pPlanetCoBlocks->GetOctreeSize();
			CameraDistance = 2 * OctreeSize;
		}

		Camera* pCamera = static_cast<Camera*>( pEntCamera );
		pCamera->SetPosition( dvec3(0.f, CameraDistance, 0.f) );
		quat CamRot( quat::fromeuler_xyz( 90.0f, 180.0f, 0.0f ) );
		pCamera->SetRotation( CamRot );
	}

	// Link scene objects
	CameraCtrl_Base* pCamCtrl = Controller::GetStaticInstance()->GetCameraCtrl( MarsCameraCtrl_Fly::StaticClass() );
	if( pCamCtrl && pCamCtrl->IsA( MarsCameraCtrl_Fly::StaticClass() ) )
	{
		((MarsCameraCtrl_Fly*)pCamCtrl)->SetTargetPlanet( pPlanet );
	}
	
#endif

	return bInit;
}

void FSEngine::Shutdown()
{
	Engine::Shutdown();
}

void FSEngine::MainLoop()
{
	Engine::MainLoop();
}

void FSEngine::DeclareComponentsAndEntities()
{
	Super::DeclareComponentsAndEntities();

#if 0
	DECLARE_COMPONENT_MGR( CoBlocks, BlocksManager );
	DECLARE_COMPONENT( CoDPosition );
	DECLARE_ENTITYPATTERN( Planet, Entity, (2, "CoDPosition", "CoBlocks"), (0) );
	DECLARE_ENTITYPATTERN( Sun, Entity, (1, "CoDPosition"), (0) );
#endif
}

void FSEngine::CreateGameCameras()
{
#if 0
	Controller* pController = Controller::GetStaticInstance();
	pController->RegisterCameraCtrl( new MarsCameraCtrl_Fly() );
	pController->SetActiveCameraCtrl( MarsCameraCtrl_Fly::StaticClass() );
#endif
}

void FSEngine::InitManagers()
{
	Super::InitManagers();

#if 0
	BlocksManager* pBlocksManager = new BlocksManager();
	pBlocksManager->Create();
	m_Managers.push_back( pBlocksManager );

	DataStreamManager* pDataStreamManager = new DataStreamManager();
	pDataStreamManager->Create();
	m_Managers.push_back( pDataStreamManager );
#endif
}

void FSEngine::DestroyManagers()
{
	Super::DestroyManagers();
}

bool FSEngine::RunCommand( String const& CmdType, Array<String> const& Switches, Array<String> const& Tokens )
{
#if 0
	if( CmdType == "builddata" )
	{
		CmdBuildData Cmd;
		return Cmd.Run( Switches, Tokens );
	}
	else if( CmdType == "testksvd" )
	{
		CmdTestKSVD Cmd;
		return Cmd.Run( Switches, Tokens );
	}
	else if( CmdType == "testavi" )
	{
		CmdTestAVI Cmd;
		return Cmd.Run( Switches, Tokens );
	}
#endif

	return Engine::RunCommand( CmdType, Switches, Tokens );
}
