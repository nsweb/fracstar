

#ifndef FSCAMERA_H
#define FSCAMERA_H

#include "engine/camera.h"


//////////////////////////////////////////////////////////////////////////

class BIGBALL_API FSCameraCtrl_Fly : public CameraCtrl_Base
{
	CLASS_EQUIP_H(FSCameraCtrl_Fly, CameraCtrl_Base)

public:
					FSCameraCtrl_Fly();

	//float			m_StrafeSpeed;
	//float			m_RotationSpeed;

public:
	virtual void		UpdateView( CameraView& CamView, float DeltaSeconds );
	virtual bool		OnControllerInput( Camera* pCamera, struct ControllerInput const& Input );
	void				SetTarget( Entity* pTarget )		{ m_pTarget = pTarget; }

protected:
	Entity*			m_pTarget;
};

#endif // FSCAMERA_H