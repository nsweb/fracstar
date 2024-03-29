
// fseditor.h


#ifndef FSEDITOR_H
#define FSEDITOR_H

#include "engine/engine.h"

namespace bigfx
{
    struct BIGFX_API RenderContext;
};

class FSEditor
{

public:
                        FSEditor();
	virtual             ~FSEditor();

	virtual bool        Init();
	virtual void        Shutdown();
    
    static FSEditor*	Get()		{ return ms_peditor;	}
    
    void                UIDrawEditor( bool* bshow_editor, bigfx::RenderContext& render_ctxt );
    
    int                 m_current_cp_idx;
    int                 m_current_lvl_idx;
    
private:
    static FSEditor*    ms_peditor;

	static bool			GetItemStringArray( void* data, int idx, const char** out_text );

};


#endif  // FSEDITOR_H