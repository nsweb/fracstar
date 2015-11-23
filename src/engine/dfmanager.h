


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

	struct DFVertex
	{
		vec2	Pos;
		vec2	Tex;
	};

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

	GLuint				m_DF_VAO;
	GLuint				m_DF_VBO;
	Shader*				m_DFShader;
};


#endif // DFMANAGER_H