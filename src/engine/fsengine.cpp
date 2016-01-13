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
#include "../game/colevel.h"
#include "../game/fsworld.h"
#include "../engine/dfmanager.h"
#include "../engine/fscamera.h"
#include "../editor/fseditor.h"

FSEngine* FSEngine::ms_pengine = nullptr;

FSEngine::FSEngine()
{
	ms_pengine = this;
}

FSEngine::~FSEngine()
{
	ms_pengine = nullptr;
}

bool FSEngine::Init( bool bCreateWindow )
{
	bool bInit = Engine::Init( bCreateWindow );
    
    //////////////////////////////////////////////////////////////////////////
	m_peditor = new FSEditor();
	m_peditor->Init();


	//////////////////////////////////////////////////////////////////////////
	// Scene description
    FSWorld::GetStaticInstance()->InitLevels( "../data/world.json" );
    
	Entity* pship = EntityManager::GetStaticInstance()->CreateEntityFromJson( "../data/ship.json", "Ship" );
	EntityManager::GetStaticInstance()->AddEntityToWorld( pship );

	//Entity* plevel = EntityManager::GetStaticInstance()->CreateEntityFromJson( "../data/level_0.json", "Level_0" );
	//EntityManager::GetStaticInstance()->AddEntityToWorld( plevel );

#if 0
	Entity* pSun = EntityManager::GetStaticInstance()->CreateEntityFromJson( "../data/sun.json", "Sun" );
	EntityManager::GetStaticInstance()->AddEntityToWorld( pSun );
	CoDPosition* pSunCoPos = static_cast<CoDPosition*>( pSun->GetComponent( CoDPosition::StaticClass() ) );
	if( pSunCoPos )
	{
		pSunCoPos->SetPosition( dvec3( 0, -162e9, -81e9 ) );	// 230 million km
	}
#endif
	Entity* pent_camera = EntityManager::GetStaticInstance()->CreateEntityFromJson( "../data/defaultcamera.json" );
	EntityManager::GetStaticInstance()->AddEntityToWorld( pent_camera );
	if( pent_camera->IsA( Camera::StaticClass() ) )
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
	CameraCtrl_Base* cam_ctrl = Controller::GetStaticInstance()->GetCameraCtrl( FSCameraCtrl_Fly::StaticClass() );
	if( cam_ctrl && cam_ctrl->IsA( FSCameraCtrl_Fly::StaticClass() ) )
		((FSCameraCtrl_Fly*)cam_ctrl)->SetTarget( pship );

    CoLevel* level = FSWorld::GetStaticInstance()->GetCurrentLevel();
	CoShip* pcoship = static_cast<CoShip*>( pship->GetComponent( CoShip::StaticClass() ) );
	if( pcoship )
		pcoship->SetCurrentLevel( level->GetEntity() );

	return bInit;
}

void FSEngine::Shutdown()
{
	m_peditor->Shutdown();
	BB_DELETE( m_peditor );

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
    DECLARE_COMPONENT_MGR( CoLevel, FSWorld );
	//DECLARE_COMPONENT( CoPath );
	DECLARE_ENTITYPATTERN( Ship, Entity, (2, "CoPosition", "CoShip"), (0) );
	DECLARE_ENTITYPATTERN( Level, Entity, (3, "CoPosition", "CoPath", "CoLevel"), (0) );
}

void FSEngine::CreateGameCameras()
{
	Controller* pController = Controller::GetStaticInstance();
	pController->RegisterCameraCtrl( new FSCameraCtrl_Fly() );
	pController->RegisterCameraCtrl( new FSCameraCtrl_EditPath() );
	pController->SetActiveCameraCtrl( FSCameraCtrl_Fly::StaticClass() );
}

void FSEngine::InitManagers()
{
	Super::InitManagers();

    FSWorld* pworld = new FSWorld();
    pworld->Create();
    m_managers.push_back( pworld );
    
	ShipManager* pship_manager = new ShipManager();
	pship_manager->Create();
	m_managers.push_back( pship_manager );

	PathManager* ppath_manager = new PathManager();
	ppath_manager->Create();
	m_managers.push_back( ppath_manager );

	DFManager* pdf_manager = new DFManager();
	pdf_manager->Create();
	m_managers.push_back( pdf_manager );
}

void FSEngine::DestroyManagers()
{
	Super::DestroyManagers();
}

bool FSEngine::RunCommand( String const& cmd_type, Array<String> const& switches, Array<String> const& tokens )
{
#if 0
	if( cmd_type == "builddata" )
	{
		CmdBuildData cmd;
		return cmd.Run( switches, tokens );
	}
#endif

	return Engine::RunCommand( cmd_type, switches, tokens );
}
