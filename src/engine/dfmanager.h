


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
		vec2	pos;
		vec2	tex;
	};

private:
	typedef ComponentManager Super;

public:
						DFManager();
	virtual				~DFManager();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		AddComponentToWorld( Component* pcomponent );
	virtual void		RemoveComponentFromWorld( Component* pcomponent );
	virtual void		Tick( struct TickContext& tick_ctxt );
	virtual void		_Render( struct RenderContext& render_ctxt );

	//Array<CoBlocks*> const& GetBlocksArray()	{ return m_Blocks;	}

protected:

	GLuint				m_df_vao;
	GLuint				m_df_vbo;
	Shader*				m_df_shader;
};


#endif // DFMANAGER_H