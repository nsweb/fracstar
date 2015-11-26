


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
	void Eval( float u, vec3& Pos, vec3& Tan )
	{
		float u2 = u * u;
		float u3 = u2 * u;
		Pos = c0 + c1*u + c2*u2 + c3*u3;
        Tan = c1*u + 2.f*c2*u + 3.f*c3*u2;
	}

	void InitNonuniformCatmullRom( const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3, float dt0, float dt1, float dt2 );
	void InitCubicSpline( const vec3& p0, const vec3& p1, const vec3& t0, const vec3& t1 );
};

class LevelPath
{
public:
    LevelPath();

	Name m_LevelName;
    
	/** Control points of the spline */
	Array<vec3> m_CPoints;
	/** Piecewise cubic splines - if nCP are defined, nCP-3 splines are required */
	Array<CubicSpline> m_Splines;
	/** Knot sequence for interpolation */
	Array<float> m_Knots;
    /* Sum of dist between control points */
    float m_SumDistance;
	/* Sum of dist between control points, excluding first and last CP*/
	float m_ClampedSumDistance;
	/* Dist between m_Knots[nCP-2] and m_Knots[1] */
	float m_ClampedKnotDistance;
    
    void InterpPath( float DistAlongPath, vec3& Pos, vec3& Tan );
};

class CoPath : public Component 
{
	CLASS_EQUIP_H(CoPath, Component)

public:
						CoPath();
	virtual				~CoPath();

	static Component*	NewComponent()		{ return new CoPath();	}

	virtual void		Create( Entity* Owner, class json::Object* Proto = nullptr );
	virtual void		Destroy();	
	virtual void		AddToWorld();
	virtual void		RemoveFromWorld();
	virtual void		Tick( TickContext& TickCtxt );
	//void				_Render( RenderContext& RenderCtxt, Shader* BlockShader );
	LevelPath*			GetLevelPath( Name const& LevelName );

protected:
	Array<LevelPath>	m_LevelPaths;
};

#endif // FSCOPATH_H