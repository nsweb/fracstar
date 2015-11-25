


#ifndef FSSHIPMANAGER_H
#define FSSHIPMANAGER_H

#include "engine/componentmanager.h"

namespace bigball
{
	class BIGBALL_API Component;
	class BIGBALL_API Shader;
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
	virtual void		_Render( struct RenderContext& RenderCtxt );

	Array<CoShip*> const& GetShipArray()	{ return m_Ships;	}

protected:
	Array<CoShip*>		m_Ships;
};


#endif // FSSHIPMANAGER_H