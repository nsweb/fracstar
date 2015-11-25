

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

CLASS_EQUIP_CPP(CoPath);

LevelPath::LevelPath() :
    m_SumDistance(0.f)
{
    
}

vec3 LevelPath::InterpPath( float tnorm )
{
    BB_ASSERT( m_Knots.size() >= 2 );
    
    float t = bigball::clamp( tnorm * m_SumDistance, 0.f, m_SumDistance );
    
    int k = 0;
    for( ; k < m_Knots.size() - 1; k++ )
    {
        if( t <= m_Knots[k + 1] )
            break;
    }
    
    float t0, t1 = m_Knots[k], t2 = m_Knots[k + 1], t3;
    vec3 A1, A2, A3;
    if( k == 0 )
        A1 = m_CPoints[0];
    else
    {
        t0 = m_Knots[k - 1];
        A1 = (t1 - t) / (t1 - t0) * m_CPoints[k - 1] + (t - t0) / (t1 - t0) * m_CPoints[k];
    }
    
    A2 = (t2 - t) / (t2 - t1) * m_CPoints[k] + (t - t1) / (t2 - t1) * m_CPoints[k + 1];
    
    if( k == m_Knots.size() - 2 )
        A3 = m_CPoints[m_Knots.size() - 1];
    else
    {
        t3 = m_Knots[k + 2];
        A3 = (t3 - t) / (t3 - t2) * m_CPoints[k - 1] + (t - t2) / (t3 - t2) * m_CPoints[k];
    }
    
    vec3 B1, B2;
    
}

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
		bigball::sincos( i * 0.1f, &stime, &ctime );
		vec3 p = vec3( 3.0f*stime, 4.0f*ctime, -2.9f + 0.0f*stime );
		LPath.m_CPoints.push_back( p );
	}

	float lastt = 0.f, newt;
    LPath.m_SumDistance = 0.f;
	LPath.m_Knots.push_back( 0.f );
	for( int i = 1; i < nCP; i++ )
	{
		float dist = bigball::distance( LPath.m_CPoints[i], LPath.m_CPoints[i-1] );
        LPath.m_SumDistance += dist;
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
	//CoPosition* CoPos = static_cast<CoPosition*>( GetEntity()->GetComponent( CoPosition::StaticClass() ) );

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
