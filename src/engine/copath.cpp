

#include "../fracstar.h"
#include "copath.h"
#include "engine/coposition.h"
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

#define PRIORITY_LOAD 1

CLASS_EQUIP_CPP(CoPath);

CoPath::CoPath() 
{

}

CoPath::~CoPath()
{

}

void CoPath::Create( Entity* Owner, class json::Object* Proto )
{
	Super::Create( Owner, Proto );

	// Temp : creating a test path procedurally
	m_LevelPaths.push_back( LevelPath() );
	LevelPath& LPath = m_LevelPaths.Last();
	
	const int32 nCP = 10;
	for( int i = 0; i < nCP; i++ )
	{
		float stime, ctime;
		bigball::sincos( nCP * 0.1f, &stime, &ctime );
		vec3 p = vec3( 3.0f*stime, 4.0f*ctime, -2.9f + 0.0f*stime );
		LPath.m_CPoints.push_back( p );
	}

	float lastt = 0.f, newt; 
	LPath.m_Knots.push_back( 0.f );
	for( int i = 1; i < nCP; i++ )
	{
		float dist = bigball::distance( LPath.m_CPoints[i], LPath.m_CPoints[i-1] );
		newt = lastt + bigball::sqrt( dist );
		LPath.m_Knots.push_back( newt );
		lastt = newt;
	}
}

void CoPath::Destroy()
{
	Super::Destroy();
}

void CoPath::AddToWorld()
{
	Super::AddToWorld();
}

void CoPath::RemoveFromWorld()
{
	Super::RemoveFromWorld();
}

void CoPath::Tick( TickContext& TickCtxt )
{
	CoPosition* CoPos = static_cast<CoPosition*>( GetEntity()->GetComponent( CoPosition::StaticClass() ) );

	// Retrieve current camp pos from path interpolation (centripetal catmull-rom spline)


	//CameraView const& CamView = Controller::GetStaticInstance()->GetRenderView();
	//dvec3 CamPos = CamView.m_Transform.GetTranslation();
	//dtransform const& EntityT = CoPos->GetTransform();
	//dvec3 CamPosBlock = EntityT.TransformPositionInverse( CamPos );
}

//void CoPath::_Render( RenderContext& RenderCtxt, Shader* BlockShader )
//{
//	
//}
