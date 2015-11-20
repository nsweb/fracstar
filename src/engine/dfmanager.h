


#ifndef DFMANAGER_H
#define DFMANAGER_H

#include "engine/componentmanager.h"

namespace bigball
{
	class BIGBALL_API Component;
	class BIGBALL_API Shader;
};
class CoBlocks;

class DFManager : public ComponentManager 
{
	STATIC_MANAGER_H(DFManager);

private:
	typedef ComponentManager Super;

public:
						DFManager();
	virtual				~DFManager();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		AddComponentToWorld( Component* pComponent );
	virtual void		RemoveComponentFromWorld( Component* pComponent );
	virtual void		Tick( struct TickContext& TickCtxt );
	virtual void		_Render( struct RenderContext& RenderCtxt );

	//Array<CoBlocks*> const& GetBlocksArray()	{ return m_Blocks;	}

protected:
	//Array<CoBlocks*>	m_Blocks;
	Shader*				m_DFShader;
	Shader*				m_SampleShader;
};


#endif // DFMANAGER_H