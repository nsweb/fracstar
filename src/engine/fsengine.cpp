// fsengine.cpp
//

#include "../fracstar.h"
#include "engine/entity.h"
#include "engine/entitymanager.h"
#include "engine/controller.h"
#include "engine/coposition.h"
#include "ui/uimanager.h"
#include "../engine/copath.h"
#include "../engine/pathmanager.h"
#include "../game/coship.h"
#include "../game/shipmanager.h"
#include "../engine/dfmanager.h"
#include "../engine/fscamera.h"
#include "../editor/fseditor.h"

FSEngine* FSEngine::ms_pEngine = nullptr;

FSEngine::FSEngine()
{
	ms_pEngine = this;
}

FSEngine::~FSEngine()
{
	ms_pEngine = nullptr;
}

bool FSEngine::Init( bool bCreateWindow )
{
	bool bInit = Engine::Init( bCreateWindow );
    
    //////////////////////////////////////////////////////////////////////////
	m_pEditor = new FSEditor();
	m_pEditor->Init();


	//////////////////////////////////////////////////////////////////////////
	// Scene description
	Entity* pShip = EntityManager::GetStaticInstance()->CreateEntityFromJson( "../data/ship.json", "Ship" );
	EntityManager::GetStaticInstance()->AddEntityToWorld( pShip );

	Entity* pLevel = EntityManager::GetStaticInstance()->CreateEntityFromJson( "../data/level_0.json", "Level_0" );
	EntityManager::GetStaticInstance()->AddEntityToWorld( pLevel );

#if 0
	Entity* pSun = EntityManager::GetStaticInstance()->CreateEntityFromJson( "../data/sun.json", "Sun" );
	EntityManager::GetStaticInstance()->AddEntityToWorld( pSun );
	CoDPosition* pSunCoPos = static_cast<CoDPosition*>( pSun->GetComponent( CoDPosition::StaticClass() ) );
	if( pSunCoPos )
	{
		pSunCoPos->SetPosition( dvec3( 0, -162e9, -81e9 ) );	// 230 million km
	}
#endif
	Entity* pEntCamera = EntityManager::GetStaticInstance()->CreateEntityFromJson( "../data/defaultcamera.json" );
	EntityManager::GetStaticInstance()->AddEntityToWorld( pEntCamera );
	if( pEntCamera->IsA( Camera::StaticClass() ) )
	{
#if 0
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
#endif
	}


	// Link scene objects
	CameraCtrl_Base* pCamCtrl = Controller::GetStaticInstance()->GetCameraCtrl( FSCameraCtrl_Fly::StaticClass() );
	if( pCamCtrl && pCamCtrl->IsA( FSCameraCtrl_Fly::StaticClass() ) )
	{
		((FSCameraCtrl_Fly*)pCamCtrl)->SetTarget( pShip );
	}

	CoShip* pCoShip = static_cast<CoShip*>( pShip->GetComponent( CoShip::StaticClass() ) );
	if( pCoShip )
		pCoShip->SetCurrentLevel( pLevel );

	return bInit;
}

void FSEngine::Shutdown()
{
	m_pEditor->Shutdown();
	BB_DELETE( m_pEditor );

	Engine::Shutdown();
}

void FSEngine::MainLoop()
{
	Engine::MainLoop();
}

void FSEngine::DeclareComponentsAndEntities()
{
	Super::DeclareComponentsAndEntities();

	DECLARE_COMPONENT_MGR( CoPath, PathManager );
	DECLARE_COMPONENT_MGR( CoShip, ShipManager );
	//DECLARE_COMPONENT( CoPath );
	DECLARE_ENTITYPATTERN( Ship, Entity, (2, "CoPosition", "CoShip"), (0) );
	DECLARE_ENTITYPATTERN( Level, Entity, (2, "CoPosition", "CoPath"), (0) );
}

void FSEngine::CreateGameCameras()
{
	Controller* pController = Controller::GetStaticInstance();
	pController->RegisterCameraCtrl( new FSCameraCtrl_Fly() );
	pController->SetActiveCameraCtrl( FSCameraCtrl_Fly::StaticClass() );
}

void FSEngine::InitManagers()
{
	Super::InitManagers();

	ShipManager* pShipManager = new ShipManager();
	pShipManager->Create();
	m_Managers.push_back( pShipManager );

	PathManager* pPathManager = new PathManager();
	pPathManager->Create();
	m_Managers.push_back( pPathManager );

	DFManager* pDFManager = new DFManager();
	pDFManager->Create();
	m_Managers.push_back( pDFManager );
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
