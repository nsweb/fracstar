


#ifndef FSCOLEVEL_H
#define FSCOLEVEL_H

#include "engine/component.h"


namespace bigball
{
	class BIGBALL_API Shader;
	struct BIGBALL_API TickContext;
	struct BIGBALL_API RenderContext;
};


class CoLevel : public Component 
{
	CLASS_EQUIP_H(CoLevel, Component)

public:
						CoLevel();
	virtual				~CoLevel();

	static Component*	NewComponent()		{ return new CoLevel();	}

	virtual void		Create( Entity* owner, class json::Object* proto = nullptr );
	virtual void		Destroy();	
	virtual void		AddToWorld();
	virtual void		RemoveFromWorld();
	virtual void		Tick( TickContext& tick_ctxt );
	//void				_Render( RenderContext& RenderCtxt, Shader* BlockShader );

	//void				SetCurrentLevel( Entity* pcurrent_level ) { m_pcurrent_level = pcurrent_level; }
    //void                ChangeState( eShipState new_state );

public:
    Name            m_level_name;
	/*Entity*		m_pcurrent_level;
    
    eShipState  m_state;
    float       m_speed;
	float		m_path_dist_level;
    float       m_path_knot_dist_level;*/
};

#endif // FSCOLEVEL_H