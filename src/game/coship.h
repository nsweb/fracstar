#ifndef FSCOSHIP_H
#define FSCOSHIP_H

#include "engine/component.h"


namespace bigfx
{
	struct BIGFX_API TickContext;
	struct BIGFX_API RenderContext;
    class BIGFX_API Camera;
    struct ControllerInput;
};

enum class eShipState
{
    Run = 0,
    Edit,
};

class CoShip : public Component 
{
	CLASS_EQUIP_H(CoShip, Component)

public:
						CoShip();
	virtual				~CoShip();

	static Component*	NewComponent()		{ return new CoShip();	}

	virtual void		Create( Entity* owner, class json::Object* proto = nullptr ) override;
	virtual void		Destroy() override;
	virtual void		AddToWorld() override;
	virtual void		RemoveFromWorld() override;
	virtual void		Tick( TickContext& tick_ctxt );
	void				_Render( RenderContext& render_ctxt );
    bool                OnControllerInput( Camera* camera, ControllerInput const& input );

    void				SetCurrentLevel( Entity* current_level );
    void                ChangeState( eShipState new_state );
	const transform&	GetPathTransform()	{ return m_path_frame;	}

public:
	Entity*				m_current_level;
	bgfx::UniformHandle u_collision_dist;
	bgfx::ProgramHandle m_ship_program;
	transform			m_path_frame;
    
    eShipState  m_state;
    vec3        m_ship_cam_pos;
    /** Prototype parameters */
    float       m_speed;
    float       m_speed_lateral;
    float       m_cam_zoffset;
    float       m_ship_scale;
    float       m_move_range;
	/** Current distance along path in a given level */
	float		m_path_dist_level;
    float       m_path_knot_dist_level;
	/** Current distance to nearest obstacle */
	float		m_current_collision_dist;
};

#endif // FSCOSHIP_H