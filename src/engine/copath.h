


#ifndef FSCOPATH_H
#define FSCOPATH_H

#include "engine/component.h"


namespace bigball
{
	class BIGBALL_API Shader;
	struct BIGBALL_API TickContext;
	struct BIGBALL_API RenderContext;
};

// From http://stackoverflow.com/questions/9489736/catmull-rom-curve-with-no-cusps-and-no-self-intersections/23980479#23980479
struct CubicSpline
{
	vec3 c0, c1, c2, c3;
	void Eval( float u, vec3& Pos, vec3& Tan ) const
	{
		float u2 = u * u;
		float u3 = u2 * u;
		Pos = c0 + c1*u + c2*u2 + c3*u3;
        Tan = c1 + 2.f*c2*u + 3.f*c3*u2;
	}

	void InitNonuniformCatmullRom( const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3, float dt0, float dt1, float dt2 );
	void InitCubicSpline( const vec3& p0, const vec3& p1, const vec3& t0, const vec3& t1 );
};

class ControlPoint
{
public:
    ControlPoint()
	{
	}

	vec3 m_pos;

	float m_knot;
    float m_dist;
};

class CoPath : public Component 
{
	CLASS_EQUIP_H(CoPath, Component)

public:
						CoPath();
	virtual				~CoPath();

	static Component*	NewComponent()		{ return new CoPath();	}

	virtual void		Create( Entity* owner, class json::Object* proto = nullptr );
	virtual void		Destroy();	
	virtual void		AddToWorld();
	virtual void		RemoveFromWorld();
	virtual void		Tick( TickContext& tick_ctxt );
    void                _DrawDebug( RenderContext& render_ctxt );
	//void				_Render( RenderContext& RenderCtxt, Shader* BlockShader );
    
    void                InterpPath( float dist_along_path, vec3& pos, vec3& tan ) const;
    bool                InsertControlPoint( int cp_idx, bool insert_after );
    void                InsertControlPoint( int cp_idx, vec3& pos );
    bool				DeleteControlPoint( int cp_idx );
	float				GetClampedKnotDistance( int at_cp_idx ) const;
    float               GetClampedSumDistance( int at_cp_idx ) const;

public:
    Name m_level_name;
    
    /** Control points of the spline */
    Array<ControlPoint> m_ctrl_points;
    /** Piecewise cubic splines - if cp_count are defined, cp_count-3 splines are required */
    Array<CubicSpline> m_splines;
    /* Sum of dist between control points, excluding first and last CP*/
    float m_clamped_sum_distance;
    /* Sum of knot dist between control points, excluding first and last CP*/
    float m_clamped_knot_distance;
    
private:
    void                ComputeKnotDistances();
	void				ComputeSplines( int from_sp_idx, int to_sp_idx );
};

#endif // FSCOPATH_H