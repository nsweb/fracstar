
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
    
private:
    static FSEditor*    ms_peditor;
    int                 m_current_cp_edit;

};


#endif  // FSEDITOR_H