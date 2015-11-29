

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
#include "gfx/drawutils.h"
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

void LevelPath::InterpPath( float DistAlongPath, vec3& Pos, vec3& Tan ) const
{
    BB_ASSERT( m_Knots.size() >= 2 );
    
    float udist = bigball::clamp( (DistAlongPath / m_ClampedSumDistance) * m_ClampedKnotDistance, 0.f, m_ClampedKnotDistance );
    udist += m_Knots[1];
    
    int k = 1;
    for( ; k < m_Knots.size() - 1; k++ )
    {
        if( udist <= m_Knots[k + 1] )
            break;
    }
    
    BB_ASSERT( k - 1 < m_Splines.size() );
    udist = (udist - m_Knots[k]) / (m_Knots[k + 1] - m_Knots[k]);
    m_Splines[k - 1].Eval( udist, Pos, Tan );
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
    LPath.m_LevelName = "Level_0";
	
	const int32 nCP = 20;
	for( int i = 0; i < nCP; i++ )
	{
		float stime, ctime;
		bigball::sincos( i * 0.1f, &stime, &ctime );
		vec3 p = vec3( 3.0f*stime, 4.0f*ctime, -2.9f + 2.f*stime );
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

void CoPath::_DrawDebug( RenderContext& RenderCtxt )
{
    static float SegDist = 0.1f;
    static u8vec4 Color( 128, 0, 128, 255 );
    Array<vec3> SegmentList;
    vec3 Pos, Tan;
    
    for( int i = 0; i < m_LevelPaths.size(); i++ )
    {
        SegmentList.clear();
        
        LevelPath& LPath = m_LevelPaths[i];
        int SegCount = (int)(LPath.m_ClampedSumDistance / SegDist) + 1;
        for( int SegIdx = 0; SegIdx <= SegCount; SegIdx++ )
        {
            LPath.InterpPath( (SegIdx * LPath.m_ClampedSumDistance) / SegCount, Pos, Tan );
            SegmentList.push_back( Pos );
        }
        DrawUtils::GetStaticInstance()->PushSegmentList( SegmentList, Color );
    }
    
    // TEMP DEBUG
    /*SegmentList.clear();
    SegmentList.push_back(vec3(-10.f, -10.f, -10.f));
    SegmentList.push_back(vec3(-10.f, 10.f, -10.f));
    SegmentList.push_back(vec3(10.f, 10.f, -10.f));
    SegmentList.push_back(vec3(10.f, 10.f, -10.f));
    DrawUtils::GetStaticInstance()->PushSegmentList( SegmentList, u8vec4( 255, 255, 0, 255 ) );
    
    SegmentList.clear();
    SegmentList.push_back(vec3(-10.f, -10.f, 10.f));
    SegmentList.push_back(vec3(-10.f, 10.f, 10.f));
    SegmentList.push_back(vec3(10.f, 10.f, 10.f));
    SegmentList.push_back(vec3(10.f, 10.f, 10.f));
    DrawUtils::GetStaticInstance()->PushSegmentList( SegmentList, u8vec4( 0, 255, 255, 255 ) );
    
    DrawUtils::GetStaticInstance()->PushSegment(vec3(-10.f, -10.f, -10.f), vec3(-10.f, -10.f, 10.f), u8vec4( 255, 0, 0, 255 ), u8vec4( 0, 0, 255, 255 ));
    DrawUtils::GetStaticInstance()->PushSegment(vec3(-10.f, 10.f, -10.f), vec3(-10.f, 10.f, 10.f), u8vec4( 255, 0, 0, 255 ), u8vec4( 0, 0, 255, 255 ));
    DrawUtils::GetStaticInstance()->PushSegment(vec3(10.f, 10.f, -10.f), vec3(10.f, 10.f, 10.f), u8vec4( 255, 0, 0, 255 ), u8vec4( 0, 0, 255, 255 ));
    DrawUtils::GetStaticInstance()->PushSegment(vec3(10.f, -10.f, -10.f), vec3(10.f, -10.f, 10.f), u8vec4( 255, 0, 0, 255 ), u8vec4( 0, 0, 255, 255 ));
*/
}

LevelPath* CoPath::GetLevelPath( Name const& LevelName )
{
	for( int i = 0; i < m_LevelPaths.size(); i++ )
	{
		if( m_LevelPaths[i].m_LevelName == LevelName )
			return &m_LevelPaths[i];
	}
	return nullptr;
}