
// fseditor.h


#ifndef FSEDITOR_H
#define FSEDITOR_H

#include "engine/engine.h"

class FSEditor
{

public:
                        FSEditor();
	virtual             ~FSEditor();

	virtual bool        Init();
	virtual void        Shutdown();
    
    static FSEditor*	Get()		{ return ms_peditor;	}
    
    void                UIDrawEditor( bool* bshow_editor );
    
    int                 m_current_cp_idx;
    
private:
    static FSEditor*    ms_peditor;

	static bool			GetItemStringArray( void* data, int idx, const char** out_text );

};


#endif  // FSEDITOR_H