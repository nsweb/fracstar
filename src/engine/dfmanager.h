
#ifndef DFMANAGER_H
#define DFMANAGER_H

#include "engine/componentmanager.h"
#include "bgfx_utils.h"

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
		vec2	m_pos;
		vec2	m_tex;

		static void init()
		{
			ms_layout
				.begin()
				.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
				.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
				.end();
		};
		static bgfx::VertexLayout ms_layout;
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

protected:

	bgfx::VertexBufferHandle	m_vbh_fullscreen_quad;
	bgfx::UniformHandle			u_z_var_screen_res;
	bgfx::UniformHandle			u_camera_pos;
};

#endif // DFMANAGER_H