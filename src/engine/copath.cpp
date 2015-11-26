

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


void CubicSpline::InitNonuniformCatmullRom( const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3, float dt0, float dt1, float dt2 )
{
	// compute tangents when parameterized in [t1,t2]
	vec3 t1 = (p1 - p0) / dt0 - (p2 - p0) / (dt0 + dt1) + (p2 - p1) / dt1;
	vec3 t2 = (p2 - p1) / dt1 - (p3 - p1) / (dt1 + dt2) + (p3 - p2) / dt2;

	// rescale tangents for parametrization in [0,1]
	t1 *= dt1;
	t2 *= dt1;

	InitCubicSpline( p1, p2, t1, t2 );
}

/*
 * Compute coefficients for a cubic polynomial
 *   p(s) = c0 + c1*s + c2*s^2 + c3*s^3
 * such that
 *   p(0) = x0, p(1) = x1
 *  and
 *   p'(0) = t0, p'(1) = t1.
 */
void CubicSpline::InitCubicSpline( const vec3& p0, const vec3& p1, const vec3& t0, const vec3& t1 )
{
	c0 = p0;
	c1 = t0;
	c2 = -3.f*p0 + 3.f*p1 - 2.f*t0 - t1;
	c3 = 2.f*p0 - 2.f*p1 + t0 + t1;
}


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
    return A3;
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
	LPath.m_ClampedSumDistance = 0.f;
	LPath.m_Knots.push_back( 0.f );
	for( int i = 1; i < nCP; i++ )
	{
		float dist = bigball::distance( LPath.m_CPoints[i], LPath.m_CPoints[i-1] );
        LPath.m_SumDistance += dist;
		if( i > 1 && i < nCP-1 )
			LPath.m_ClampedSumDistance += dist;
		newt = lastt + bigball::sqrt( dist );
		LPath.m_Knots.push_back( newt );
		lastt = newt;
	}
	LPath.m_ClampedKnotDistance = LPath.m_Knots[nCP-2] - LPath.m_Knots[1];

	// Compute cubic splines
	LPath.m_Splines.resize(nCP - 3);
	for( int i = 0; i < nCP-3; i++ )
	{
		float dt0 = LPath.m_Knots[i+1] - LPath.m_Knots[i];
		float dt1 = LPath.m_Knots[i+2] - LPath.m_Knots[i+1];
		float dt2 = LPath.m_Knots[i+3] - LPath.m_Knots[i+2];
		LPath.m_Splines[i].InitNonuniformCatmullRom( LPath.m_CPoints[i], LPath.m_CPoints[i+1], LPath.m_CPoints[i+2], LPath.m_CPoints[i+3], dt0, dt1, dt2 );
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

LevelPath* CoPath::GetLevelPath( Name const& LevelName )
{
	for( int i = 0; i < m_LevelPaths.size(); i++ )
	{
		if( m_LevelPaths[i].m_LevelName == LevelName )
			return &m_LevelPaths[i];
	}
	return nullptr;
}