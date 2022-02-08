


#ifndef FSCOPATH_H
#define FSCOPATH_H

#include "engine/component.h"
#include "system/file.h"
#include "math/splines.h"


namespace bigfx
{
	class BIGFX_API Shader;
	struct BIGFX_API TickContext;
	struct BIGFX_API RenderContext;
    struct CubicSpline;
    struct ControlPoint;
};

class CoPath : public Component 
{
	CLASS_EQUIP_H(CoPath, Component)

public:
						CoPath();
	virtual				~CoPath();

	static Component*	NewComponent()		{ return new CoPath();	}

	virtual void		Create( Entity* owner, class json::Object* proto = nullptr ) override;
	virtual void		Destroy() override;
	virtual void		AddToWorld() override;
	virtual void		RemoveFromWorld() override;
	virtual void		Tick( TickContext& tick_ctxt );
    void                _DrawDebug( bigfx::RenderContext& render_ctxt );
	//void				_Render( RenderContext& RenderCtxt, Shader* BlockShader );
    
	void                InterpPathKnotDist( float knot_dist_along_path, vec3& pos, vec3& tan ) const;
    void                __InterpPathDist( float dist_along_path, vec3& pos, vec3& tan ) const;
	void                InterpPathKnotDist( float dist_along_path, transform& tf ) const;
    void                __InterpPathDist( float dist_along_path, transform& tf ) const;
    
    bool                InsertControlPoint( int cp_idx, bool insert_after );
    bool                InsertControlPoint( float knot_dist_along_path );
    bool				DeleteControlPoint( int cp_idx );
	float				GetSumKnotDistance( int at_cp_idx ) const;
    float               GetSumDistance( int at_cp_idx ) const;
	float				ConvertDistanceToKnot( float dist_along_path ) const;
    float				ConvertKnotToDistance( float knot_dist_along_path ) const;
    int                 GetNearestControlPointIdx( float knot_dist_along_path ) const;
    void                OnControlPointMoved( int at_cp_idx );
    
    virtual void        Serialize( bigfx::Archive& file ) override;

public:
    Name m_level_name;
    
    /** Control points of the spline */
    Array<ControlPoint> m_ctrl_points;
    /** Piecewise cubic splines - if cp_count are defined, cp_count-3 splines are required */
    Array<CubicSpline> m_splines;
    /* Sum of dist between control points*/
    float m_sum_distance;
    /* Sum of knot dist between control points*/
    float m_sum_knot_distance;
	/** Number of steps needed to evaluate arc distance on a spline */
	static const int m_sp_step_count = 10;
    
private:
    void                ComputeKnotDistances();
    void                ComputeSplineDistances();
	void				ComputeSplines( int from_sp_idx, int to_sp_idx );
    
	/** Evaluate signed arc distance in knot range on a given spline 
     * @from_knot_ratio, to_knot_ratio : should be in range [0, 1]
     */
	float				EvaluateSplineArcDistance( int spline_idx, float from_knot_ratio, float to_knot_ratio ) const;
	/** Evaluate arc distance from 0 to knot_ratio on a given spline 
     * @knot_ratio : should be in range [0, 1]
     */
	float				EvaluateSplineArcDistance( int spline_idx, float knot_ratio ) const;
    /** */
    float				EvaluateSplineKnotRatioFromArcDistance( int spline_idx, float arc_dist ) const;
};

#endif // FSCOPATH_H
