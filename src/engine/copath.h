


#ifndef FSCOPATH_H
#define FSCOPATH_H

#include "engine/component.h"


namespace bigball
{
	class BIGBALL_API Shader;
	struct BIGBALL_API TickContext;
	struct BIGBALL_API RenderContext;
};

class LevelPath
{
public:
    LevelPath();
    
	/** Control points of the spline */
	Array<vec3> m_CPoints;
	/** Knot sequence for interpolation */
	Array<float> m_Knots;
    /* Sum of dist  between knots */
    float m_SumDistance;
    
    vec3 InterpPath( float t );
};

class CoPath : public Component 
{
	CLASS_EQUIP_H(CoPath, Component)

public:
						CoPath();
	virtual				~CoPath();

	static Component*	NewComponent()		{ return new CoPath();	}

	virtual void		Create( Entity* Owner, class json::Object* Proto = nullptr );
	virtual void		Destroy();	
	virtual void		AddToWorld();
	virtual void		RemoveFromWorld();
	virtual void		Tick( TickContext& TickCtxt );
	//void				_Render( RenderContext& RenderCtxt, Shader* BlockShader );

protected:
	Array<LevelPath>	m_LevelPaths;
};

#endif // FSCOPATH_H