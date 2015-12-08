
// fseditor.h


#ifndef FSEDITOR_H
#define FSEDITOR_H

#include "engine/engine.h"

class FSEditor
{

public:
					FSEditor();
	virtual			~FSEditor();

	virtual bool	Init();
	virtual void	Shutdown();

};


#endif  // FSEDITOR_H