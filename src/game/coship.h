


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

	virtual void		Create( Entity* Owner, class json::Object* Proto = nullptr );
	virtual void		Destroy();	
	virtual void		AddToWorld();
	virtual void		RemoveFromWorld();
	virtual void		Tick( TickContext& TickCtxt );
	//void				_Render( RenderContext& RenderCtxt, Shader* BlockShader );

	void				SetCurrentLevel( Entity* _pCurrentLevel ) { m_pCurrentLevel = _pCurrentLevel; }
    void                ChangeState( eShipState eNewState );

protected:
	Entity*		m_pCurrentLevel;
    
    eShipState  m_State;
    float       m_Speed;
	/** Current distance along path in a given level */
	float		m_PathDistLevel;
};

#endif // FSCOSHIP_H