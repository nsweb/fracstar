

#include "../fracstar.h"
#include "coship.h"
#include "engine/coposition.h"
#include "../engine/copath.h"
//#include "core/sort.h"
#include "engine/controller.h"
#include "engine/camera.h"
#include "engine/tickcontext.h"
//#include "engine/entity.h"
#include "engine/entitymanager.h"
#include "gfx/gfxmanager.h"
#include "gfx/shader.h"
#include "gfx/rendercontext.h"
//#include "math/frustum.h"
//#include "math/intersections.h"
#include "system/file.h"


CLASS_EQUIP_CPP(CoShip);

CoShip::CoShip() :
	m_pCurrentLevel(nullptr),
	m_PathDistLevel(0.f)
{

}

CoShip::~CoShip()
{

}

void CoShip::Create( Entity* Owner, class json::Object* Proto )
{
	Super::Create( Owner, Proto );

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

	Name const& LevelName = m_pCurrentLevel->GetName();
	CoPath* pCoPath = static_cast<CoPath*>( m_pCurrentLevel->GetComponent( CoPath::StaticClass() ) );
	LevelPath* LPath = pCoPath->GetLevelPath( LevelName );
	if( ! LPath )
		return;

	CoPosition* CoPos = static_cast<CoPosition*>( GetEntity()->GetComponent( CoPosition::StaticClass() ) );

	// Retrieve current camp pos from path interpolation (centripetal catmull-rom spline)


	//CameraView const& CamView = Controller::GetStaticInstance()->GetRenderView();
	//dvec3 CamPos = CamView.m_Transform.GetTranslation();
	//dtransform const& EntityT = CoPos->GetTransform();
	//dvec3 CamPosBlock = EntityT.TransformPositionInverse( CamPos );
}

//void CoShip::_Render( RenderContext& RenderCtxt, Shader* BlockShader )
//{
//	
//}
