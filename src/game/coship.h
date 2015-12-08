


#ifndef FSCOSHIP_H
#define FSCOSHIP_H

#include "engine/component.h"


namespace bigball
{
	class BIGBALL_API Shader;
	struct BIGBALL_API TickContext;
	struct BIGBALL_API RenderContext;
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
	//void				_Render( RenderContext& RenderCtxt, Shader* BlockShader );

	void				SetCurrentLevel( Entity* pcurrent_level ) { m_pcurrent_level = pcurrent_level; }
    void                ChangeState( eShipState new_state );

public:
	Entity*		m_pcurrent_level;
    
    eShipState  m_state;
    float       m_speed;
	/** Current distance along path in a given level */
	float		m_path_dist_level;
};

#endif // FSCOSHIP_H