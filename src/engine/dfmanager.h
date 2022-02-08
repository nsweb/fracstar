


#ifndef DFMANAGER_H
#define DFMANAGER_H

#include "engine/componentmanager.h"

namespace bigfx
{
	class BIGFX_API Component;
	class BIGFX_API Shader;
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
	void				DrawCube();

protected:

	enum eVAType
	{
		eVAScene = 0,
		eVACube,
		eVACount
	};
	enum eVBType
	{
		eVBScene = 0,        /** Dynamic VB used to render segments */
		eVBCube,
		eVBCubeElt,
		eVBCount
	};

	GLuint			m_varrays[eVACount];
	GLuint			m_vbuffers[eVBCount];
};


#endif // DFMANAGER_H