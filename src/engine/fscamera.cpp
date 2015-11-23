
#include "../fracstar.h"
#include "fscamera.h"


CLASS_EQUIP_CPP(FSCameraCtrl_Fly);

FSCameraCtrl_Fly::FSCameraCtrl_Fly() :
	m_pTarget(nullptr)
{

}


void FSCameraCtrl_Fly::UpdateView( CameraView& CamView, float DeltaSeconds )
{
	//CoDPosition* pTargetCoPos = static_cast<CoDPosition*>( m_pTargetPlanet ? m_pTargetPlanet->GetComponent( CoDPosition::StaticClass() ) : nullptr );
	//CoBlocks* pTargetCoBlocks = static_cast<CoBlocks*>( m_pTargetPlanet ? m_pTargetPlanet->GetComponent( CoBlocks::StaticClass() ) : nullptr );
	//if( pTargetCoPos && pTargetCoBlocks )
	//{
	//	// Update speed depending on distance
	//	dvec3 CamPos = CamView.m_Transform.GetTranslation();
	//	float GroundDist = pTargetCoBlocks->ComputeGroundSignedDistance( CamPos );
	//	
	//	//dvec3 TargetPos = pTargetCoPos->GetPosition();
	//	//float Dist = bigball::length( CamPos - TargetPos );

	//	double OctreeSize = pTargetCoBlocks->GetOctreeSize();
	//	double MaxDist = 10.0 * OctreeSize;
	//	float DistRatio = bigball::clamp( (float)((GroundDist) / MaxDist), 0.0f, 1.0f );
	//	m_StrafeSpeed = bigball::lerp( 1.0f, 4.0f * (float)OctreeSize, DistRatio );

	//	// Adjust near and far planes
	//	float NearFarRatio = 100000.0f;
	//	static float reduce = 0.5f;
	//	float NearD = bigball::max( 1.0f, (float)((GroundDist) * reduce) );
	//	CamView.m_fParameters[eCP_NEAR] = NearD;
	//	CamView.m_fParameters[eCP_FAR] = NearD * NearFarRatio;

	//	if( g_pEngine->m_FrameCount % 60 == 0 )
	//		BB_LOG( Test, Log, "Distance to target = %f - NearPlane = %f", (float)(GroundDist), NearD );
	//}

	Super::UpdateView( CamView, DeltaSeconds );
}

bool FSCameraCtrl_Fly::OnControllerInput( Camera* pCamera, ControllerInput const& Input )
{
	return Super::OnControllerInput( pCamera, Input );
}
