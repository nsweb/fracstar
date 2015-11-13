

#ifndef FSCAMERA_H
#define FSCAMERA_H

#include "engine/camera.h"


//////////////////////////////////////////////////////////////////////////

#if 0
class BIGBALL_API MarsCameraCtrl_Fly : public CameraCtrl_Fly
{
	CLASS_EQUIP_H(MarsCameraCtrl_Fly, CameraCtrl_Fly)

public:
					MarsCameraCtrl_Fly();

	//float			m_StrafeSpeed;
	//float			m_RotationSpeed;

public:
	virtual void		UpdateView( CameraView& CamView, float DeltaSeconds );
	virtual bool		OnControllerInput( Camera* pCamera, struct ControllerInput const& Input );
	void				SetTargetPlanet( Entity* pPlanet )		{ m_pTargetPlanet = pPlanet; }

protected:
	Entity*			m_pTargetPlanet;
};
#endif

#endif // FSCAMERA_H