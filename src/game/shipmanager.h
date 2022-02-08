


#ifndef FSSHIPMANAGER_H
#define FSSHIPMANAGER_H

#include "engine/componentmanager.h"

namespace bigfx
{
	class BIGFX_API Component;
	class BIGFX_API Shader;
    struct BIGFX_API RenderContext;
};
class CoShip;

class ShipManager : public ComponentManager 
{
	STATIC_MANAGER_H(ShipManager);

private:
	typedef ComponentManager Super;

public:
						ShipManager();
	virtual				~ShipManager();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		AddComponentToWorld( Component* pComponent );
	virtual void		RemoveComponentFromWorld( Component* pComponent );
	virtual void		Tick( struct TickContext& TickCtxt );
    virtual void		_Render( bigfx::RenderContext& RenderCtxt );

	Array<CoShip*> const& GetShipArray()	{ return m_ships;	}
	CoShip*				_GetShip()			{ if( m_ships.size() > 0 ) { return m_ships[0]; } return nullptr; }

protected:
	Array<CoShip*>		m_ships;
};


#endif // FSSHIPMANAGER_H