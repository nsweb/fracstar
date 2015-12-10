

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



CoPath::CoPath()
{

}

CoPath::~CoPath()
{

}

void CoPath::Create( Entity* owner, class json::Object* proto )
{
	Super::Create( owner, proto );

	// Temp : creating a test path procedurally
	//m_LevelPaths.push_back( LevelPath() );
	//LevelPath& LPath = m_LevelPaths.Last();
    m_level_name = "Level_0";
	
	const int32 cp_count = 10;
	m_ctrl_points.resize( cp_count );
	for( int i = 0; i < cp_count; i++ )
	{
		float stime, ctime;
		bigball::sincos( i * 0.6f, &stime, &ctime );
		vec3 p = vec3( 3.0f*stime, 4.0f*ctime, -2.9f + 2.f*stime );
        m_ctrl_points[i].m_pos = p;
	}

    ComputeKnotDistances( 0 );

	// Compute cubic splines
	m_splines.resize(cp_count - 3);
	for( int i = 0; i < cp_count-3; i++ )
	{
		float dt0 = m_ctrl_points[i+1].m_knot - m_ctrl_points[i].m_knot;
		float dt1 = m_ctrl_points[i+2].m_knot - m_ctrl_points[i+1].m_knot;
		float dt2 = m_ctrl_points[i+3].m_knot - m_ctrl_points[i+2].m_knot;
		m_splines[i].InitNonuniformCatmullRom( m_ctrl_points[i].m_pos, m_ctrl_points[i+1].m_pos, m_ctrl_points[i+2].m_pos, m_ctrl_points[i+3].m_pos, dt0, dt1, dt2 );
	}
}

void CoPath::ComputeKnotDistances( int from_cp_idx )
{
    const int cp_count = m_ctrl_points.size();
    
    float lastt = 0.f, newt, lastd = 0.f, newd;
    m_clamped_sum_distance = 0.f;
    m_ctrl_points[0].m_knot = 0.f;
    m_ctrl_points[0].m_dist = 0.f;
    for( int i = bigball::max(1,from_cp_idx); i < cp_count; i++ )
    {
        newd = bigball::distance( m_ctrl_points[i].m_pos, m_ctrl_points[i-1].m_pos );
        newt = lastt + bigball::sqrt( newd );
        m_ctrl_points[i].m_knot = newt;
        m_ctrl_points[i].m_dist = newd;
        lastt = newt;
        lastd = newd;
    }
    m_clamped_knot_distance = GetClampedKnotDistance( cp_count - 2 );
    m_clamped_sum_distance = GetClampedSumDistance( cp_count - 2 );
}

float CoPath::GetClampedKnotDistance( int at_cp_idx ) const
{
	BB_ASSERT( at_cp_idx >= 0 && at_cp_idx < m_ctrl_points.size() );

	return m_ctrl_points[at_cp_idx].m_knot - m_ctrl_points[1].m_knot;
}

float CoPath::GetClampedSumDistance( int at_cp_idx ) const
{
    BB_ASSERT( at_cp_idx >= 0 && at_cp_idx < m_ctrl_points.size() );
    
    return m_ctrl_points[at_cp_idx].m_dist - m_ctrl_points[1].m_dist;
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

void CoPath::Tick( TickContext& tick_ctxt )
{
	//CoPosition* CoPos = static_cast<CoPosition*>( GetEntity()->GetComponent( CoPosition::StaticClass() ) );

	// Retrieve current camp pos from path interpolation (centripetal catmull-rom spline)


	//CameraView const& CamView = Controller::GetStaticInstance()->GetRenderView();
	//dvec3 CamPos = CamView.m_Transform.GetTranslation();
	//dtransform const& EntityT = CoPos->GetTransform();
	//dvec3 CamPosBlock = EntityT.TransformPositionInverse( CamPos );
}

void CoPath::InterpPath( float dist_along_path, vec3& pos, vec3& tan ) const
{
    BB_ASSERT( m_ctrl_points.size() >= 2 );
    
    float udist = bigball::clamp( (dist_along_path / m_clamped_sum_distance) * m_clamped_knot_distance, 0.f, m_clamped_knot_distance );
    udist += m_ctrl_points[1].m_knot;
    
    int k = 1;
    for( ; k < m_ctrl_points.size() - 1; k++ )
    {
        if( udist <= m_ctrl_points[k + 1].m_knot )
            break;
    }
    
    BB_ASSERT( k - 1 < m_splines.size() );
    udist = (udist - m_ctrl_points[k].m_knot) / (m_ctrl_points[k + 1].m_knot - m_ctrl_points[k].m_knot);
    m_splines[k - 1].Eval( udist, pos, tan );
}

bool CoPath::InsertControlPointMidWay( int cp_idx )
{
    BB_ASSERT( cp_idx < m_ctrl_points.size() - 1 );
    if( m_ctrl_points.size() < 3 )
        return false;
    
    float t0 = GetClampedKnotDistance( cp_idx );
    float t1 = GetClampedKnotDistance( cp_idx+1 );
    ControlPoint new_cp;
    vec3 tan;
    InterpPath( 0.5f*(t0 + t1), new_cp.m_pos, tan );
    
    m_ctrl_points.insert( new_cp, cp_idx+1 );
    
    ComputeKnotDistances( cp_idx+1 );
    
    return  true;
}

void CoPath::InsertControlPoint( int cp_idx, vec3& pos )
{
    
}

void CoPath::DeleteControlPoint( int cp_idx )
{

}

void CoPath::_DrawDebug( RenderContext& render_ctxt )
{
    static float seg_dist = 0.1f;
    static float box_scale = 0.002f;
    static u8vec4 base_color( 255, 255, 255, 255 );
    u8vec4 color( base_color );
    Array<vec3> seg_list;
    Array<u8vec4> col_list;
    vec3 Pos, Tan;
    
    //for( int i = 0; i < m_LevelPaths.size(); i++ )
    {
        seg_list.clear();
        col_list.clear();
        
        //LevelPath& LPath = m_LevelPaths[i];
        int SegCount = (int)(m_clamped_sum_distance / seg_dist) + 1;
        for( int SegIdx = 0; SegIdx <= SegCount; SegIdx++ )
        {
            float ratio = (float)SegIdx / (float)SegCount;
            InterpPath( ratio * m_clamped_sum_distance, Pos, Tan );
            color.a = (uint8) (base_color.a * ratio);
            seg_list.push_back( Pos );
            col_list.push_back( color );
        }
        DrawUtils::GetStaticInstance()->PushSegmentList( seg_list, col_list );
        
        const int PCount = m_ctrl_points.size();
        for( int PIdx = 0; PIdx < PCount; PIdx++ )
        {
            float ratio = (float)PIdx / (float)PCount;
            color.r = (uint8) (base_color.r * ratio);
            DrawUtils::GetStaticInstance()->PushAABB( m_ctrl_points[PIdx].m_pos, box_scale, color );
        }
    }
}
