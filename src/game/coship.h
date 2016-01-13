


#ifndef FSCOSHIP_H
#define FSCOSHIP_H

#include "engine/component.h"


namespace bigball
{
	class BIGBALL_API Shader;
	struct BIGBALL_API TickContext;
	struct BIGBALL_API RenderContext;
    class BIGBALL_API Camera;
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

	virtual void		Create( Entity* owner, class json::Object* proto = nullptr );
	virtual void		Destroy();	
	virtual void		AddToWorld();
	virtual void		RemoveFromWorld();
	virtual void		Tick( TickContext& tick_ctxt );
	void				_Render( RenderContext& render_ctxt );
    bool                OnControllerInput( Camera* pCamera, ControllerInput const& Input );

    void				SetCurrentLevel( Entity* pcurrent_level );
    void                ChangeState( eShipState new_state );
	const transform&	GetPathTransform()	{ return m_path_frame;	}

public:
	Entity*		m_pcurrent_level;
	Shader*     m_ship_shader;
	transform	m_path_frame;
    
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
};

#endif // FSCOSHIP_H