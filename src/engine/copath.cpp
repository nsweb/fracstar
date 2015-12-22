

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

    ComputeKnotDistances();

	// Compute cubic splines
	m_splines.resize(cp_count - 3);
	ComputeSplines( 0, m_splines.size()-1 );
    
    ComputeSplineDistances();
}

void CoPath::ComputeKnotDistances()
{
    const int cp_count = m_ctrl_points.size();
    
    float sumt = 0.f;//, sumd = 0.f;
    //m_sum_distance = 0.f;
	m_sum_knot_distance = 0.f;
    m_ctrl_points[0].m_knot = 0.f;
    //m_ctrl_points[0].m_dist = 0.f;
    for( int i = 1; i < cp_count; i++ )
    {
        float dist = bigball::distance( m_ctrl_points[i].m_pos, m_ctrl_points[i-1].m_pos );
        sumt += bigball::sqrt( dist );
		//sumd += dist;
        m_ctrl_points[i].m_knot = sumt;
        //m_ctrl_points[i].m_dist = sumd;
    }

	//m_sum_distance = GetSumDistance( cp_count - 1 );
	m_sum_knot_distance = GetSumKnotDistance( cp_count - 1 );
}

void CoPath::ComputeSplineDistances()
{
    const int cp_count = m_ctrl_points.size();
    const int sp_step_count = 10;
    
    float sumd = 0.f, dist;
    m_sum_distance = 0.f;
    m_ctrl_points[0].m_dist = 0.f;
    for( int i = 1; i < cp_count; i++ )
    {
        if( i == 1 || i == cp_count - 1 )
            dist = bigball::distance( m_ctrl_points[i].m_pos, m_ctrl_points[i-1].m_pos );
        else
        {
            // Sample spline to evaluate arc distance
            dist = 0.f;
            vec3 lastp = m_ctrl_points[i-1].m_pos, pos, tan;
            for( int j = 1 ; j <= sp_step_count; j++ )
            {
                m_splines[i-2].Eval( (float)j / sp_step_count, pos, tan );
                dist += bigball::distance( pos, lastp );
                lastp = pos;
            }
        }
        sumd += dist;
        m_ctrl_points[i].m_dist = sumd;
    }
    
    m_sum_distance = GetSumDistance( cp_count - 1 );
    //m_sum_knot_distance = GetSumKnotDistance( cp_count - 1 );
}

void CoPath::ComputeSplines( int from_sp_idx, int to_sp_idx )
{
	from_sp_idx = bigball::max( 0, from_sp_idx );
	to_sp_idx = bigball::min( m_splines.size()-1, to_sp_idx );

	for( int i = from_sp_idx; i <= to_sp_idx; i++ )
	{
		float dt0 = m_ctrl_points[i+1].m_knot - m_ctrl_points[i].m_knot;
		float dt1 = m_ctrl_points[i+2].m_knot - m_ctrl_points[i+1].m_knot;
		float dt2 = m_ctrl_points[i+3].m_knot - m_ctrl_points[i+2].m_knot;
		m_splines[i].InitNonuniformCatmullRom( m_ctrl_points[i].m_pos, m_ctrl_points[i+1].m_pos, m_ctrl_points[i+2].m_pos, m_ctrl_points[i+3].m_pos, dt0, dt1, dt2 );
	}
}

float CoPath::GetSumKnotDistance( int at_cp_idx ) const
{
	const int cp_count = m_ctrl_points.size();
	if( at_cp_idx > 0 && at_cp_idx < cp_count  )
	{
		return m_ctrl_points[at_cp_idx].m_knot;
	}

	return 0.f;
}

float CoPath::GetSumDistance( int at_cp_idx ) const
{
	const int cp_count = m_ctrl_points.size();
	if( at_cp_idx > 0 && at_cp_idx < cp_count  )
	{
		return m_ctrl_points[at_cp_idx].m_dist;
	}

	return 0.f;
}

float CoPath::ConvertDistanceToKnot( float dist_along_path ) const
{
	float knot_dist = (m_sum_distance > 1e-4f ? bigball::clamp( (dist_along_path / m_sum_distance) * m_sum_knot_distance, 0.f, m_sum_knot_distance ) : 0.f);
	return knot_dist;
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

float CoPath::GetDeltaKnotDist( float from_knot_dist_along_path, float desired_delta_dist_along_path )
{
    // TODO
    return 0.f;
}

void CoPath::InterpPathDist( float dist_along_path, transform& tf ) const
{
	float knot_dist = (m_sum_distance > 1e-4f ? bigball::clamp( (dist_along_path / m_sum_distance) * m_sum_knot_distance, 0.f, m_sum_knot_distance ) : 0.f);
	InterpPathKnotDist( knot_dist, tf );
}

void CoPath::InterpPathKnotDist( float knot_dist_along_path, transform& tf ) const
{
    vec3 pos, tan;
    InterpPathKnotDist( knot_dist_along_path, pos, tan );
    tan = bigball::normalize( tan );
    
    const vec3 up( 0.f, 0.f, 1.f ); // TODO: should be defined per ctrl point
    mat3 cam_rot;
    
    // Back
    cam_rot.v2 = -tan;
    // Right
    cam_rot.v0 = bigball::normalize( bigball::cross(tan, up) );
    // Up
    cam_rot.v1 = bigball::cross(cam_rot.v0, tan);
    
    tf.Set( quat(cam_rot), pos, 1.f );
}

void CoPath::InterpPathDist( float dist_along_path, vec3& pos, vec3& tan ) const
{
	float knot_dist = (m_sum_distance > 1e-4f ? bigball::clamp( (dist_along_path / m_sum_distance) * m_sum_knot_distance, 0.f, m_sum_knot_distance ) : 0.f);
	InterpPathKnotDist( knot_dist, pos, tan );
}

void CoPath::InterpPathKnotDist( float knot_dist_along_path, vec3& pos, vec3& tan ) const
{
	const int cp_count = m_ctrl_points.size();
	int cp_idx = 0;
	for( ; cp_idx < m_ctrl_points.size()-1; cp_idx++ )
	{
		if( knot_dist_along_path <= m_ctrl_points[cp_idx + 1].m_knot )
			break;
	}
	
	if( cp_count >= 4 && cp_idx > 0 && cp_idx < cp_count - 2 )
	{
		float udist = (knot_dist_along_path - m_ctrl_points[cp_idx].m_knot) / (m_ctrl_points[cp_idx + 1].m_knot - m_ctrl_points[cp_idx].m_knot);
		m_splines[cp_idx - 1].Eval( udist, pos, tan );
	}
	else if( cp_count > 1 )
	{
		float udist = (knot_dist_along_path - m_ctrl_points[cp_idx].m_knot) / (m_ctrl_points[cp_idx + 1].m_knot - m_ctrl_points[cp_idx].m_knot);
		pos = bigball::lerp( m_ctrl_points[cp_idx].m_pos, m_ctrl_points[cp_idx+1].m_pos, udist );
		tan = m_ctrl_points[cp_idx + 1].m_pos - m_ctrl_points[cp_idx].m_pos;
	}
	else if( cp_count == 1 )
	{
		pos = m_ctrl_points[0].m_pos;
		tan = vec3( 1.f, 0.f, 0.f );
	}
	else
	{
		pos = vec3( 0.f, 0.f, 0.f );
		tan = vec3( 1.f, 0.f, 0.f );
	}
}

bool CoPath::InsertControlPoint( int cp_idx, bool insert_after )
{
	if( !insert_after )
		cp_idx--;

	const int cp_count = m_ctrl_points.size();
    if( cp_idx < -1 || cp_idx >= cp_count )
        return false;
    
	ControlPoint new_cp;
	if( cp_idx == 0 || cp_idx == cp_count - 2 )
	{
		// Insert at middle of first / last segment (can't evaluate curve nearest spline there)
		if( cp_count > 1 )
			new_cp.m_pos = (m_ctrl_points[cp_idx].m_pos + m_ctrl_points[cp_idx + 1].m_pos) * 0.5f;
		else
			new_cp.m_pos = m_ctrl_points[cp_idx].m_pos + vec3( 0.2f, 0.f, 0.f );
	}
	else if( cp_idx == -1 || cp_idx == cp_count - 1 )
	{
		if( cp_idx == -1 )
			new_cp.m_pos = 2.f*m_ctrl_points[0].m_pos - m_ctrl_points[1].m_pos;
		else
			new_cp.m_pos = 2.f*m_ctrl_points[cp_count - 1].m_pos - m_ctrl_points[cp_count - 2].m_pos;

		// Insert a new point a the end by extrapolating from last spline
		//vec3 pos, t0, t1, t2;
		//CubicSpline const& last_spline = m_splines.Last();
		//last_spline.Eval( 0.f, pos, t0 );
		//last_spline.Eval( 1.f, pos, t1 );
		//quat qrot = quat::rotate( t0, t1 );
		//t2 = qrot.transform( t1 );
		//t2 = normalize( t2 );
		//float dt1 = m_ctrl_points[cp_idx].m_knot - m_ctrl_points[cp_idx-1].m_knot;

	}
	else
	{
		// Splines are well defined in all other cases
		int sp_idx = cp_idx - 1;
		float eval_value = 0.5f;
		vec3 tan, p0, p1;
		m_splines[sp_idx].Eval( eval_value, new_cp.m_pos, tan );
	}
	
    m_ctrl_points.insert( new_cp, cp_idx+1 );
    ComputeKnotDistances();
    
	if( cp_count >= 3 )
	{
		CubicSpline new_sp;
		if( cp_idx <= 0 )
			m_splines.insert( new_sp, 0 );
		else if( cp_idx >= cp_count - 2 )
			m_splines.push_back( new_sp );
		else
			m_splines.insert( new_sp, cp_idx );
		ComputeSplines( cp_idx - 3, cp_idx + 2 );
	}
    
    ComputeSplineDistances();

	return true;
}

bool CoPath::DeleteControlPoint( int cp_idx )
{
	const int cp_count = m_ctrl_points.size();
	if( cp_idx < 0 || cp_idx >= cp_count )
		return false;

	m_ctrl_points.erase( cp_idx );
	ComputeKnotDistances();

	if( m_splines.size() )
	{
		if( cp_idx <= 0 )
			m_splines.erase( 0 );
		else if( cp_idx >= cp_count - 2 )
			m_splines.pop_back();
		else
			m_splines.erase( cp_idx - 1 );
		ComputeSplines( cp_idx - 3, cp_idx + 2 );
	}
    
    ComputeSplineDistances();
	
	return true;
}

void CoPath::OnControlPointMoved( int at_cp_idx )
{
    ComputeKnotDistances();
    ComputeSplines( at_cp_idx - 3, at_cp_idx + 2 );
    ComputeSplineDistances();
    
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
        int SegCount = (int)(m_sum_distance / seg_dist) + 1;
        for( int SegIdx = 0; SegIdx <= SegCount; SegIdx++ )
        {
            float ratio = (float)SegIdx / (float)SegCount;
            InterpPathKnotDist( ratio * m_sum_knot_distance, Pos, Tan );
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
