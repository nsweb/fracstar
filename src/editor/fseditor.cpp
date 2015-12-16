// fseditor.cpp
//

#include "../fracstar.h"
#include "fseditor.h"

//#include "engine/entity.h"
//#include "engine/entitymanager.h"
#include "engine/controller.h"
//#include "engine/coposition.h"
#include "ui/uimanager.h"

#include "../engine/copath.h"
#include "../engine/pathmanager.h"
#include "../engine/fscamera.h"
#include "../game/coship.h"
#include "../game/shipmanager.h"
//#include "dfmanager.h"
////#include "../cmd/cmdbuilddata.h"

FSEditor* FSEditor::ms_peditor = nullptr;

FSEditor::FSEditor() :
    m_current_cp_idx(0)
{
    ms_peditor = this;
}

FSEditor::~FSEditor()
{
    ms_peditor = nullptr;
}

static void UIOnToggleEditorCB( bool bshow_editor )
{
    if( bshow_editor )
    {
        // Allow menu interaction
        //SDL_SetRelativeMouseMode(SDL_FALSE);
        //SDL_SetWindowGrab( g_pEngine->GetDisplayWindow(), SDL_FALSE );
        Array<CoShip*> const& ship_array = ShipManager::GetStaticInstance()->GetShipArray();
        for( int SIdx=0; SIdx < ship_array.size(); SIdx++ )
        {
            ship_array[SIdx]->ChangeState(eShipState::Edit);
        }
        
    }
    else
    {
        // Allow FPS style mouse movement
        //SDL_SetRelativeMouseMode(SDL_TRUE);
        //SDL_SetWindowGrab( g_pEngine->GetDisplayWindow(), SDL_TRUE );
        
        Array<CoShip*> const& ShipArray = ShipManager::GetStaticInstance()->GetShipArray();
        for( int SIdx=0; SIdx < ShipArray.size(); SIdx++ )
        {
            ShipArray[SIdx]->ChangeState(eShipState::Run);
        }
    }
}

static void UIDrawEditorCB( bool* bshow_editor )
{
    FSEditor::Get()->UIDrawEditor( bshow_editor );
}

bool FSEditor::GetItemStringArray( void* data, int idx, const char** out_text )
{
	Array<String>* str_array = (Array<String>*)data;
	if( idx >= 0 && idx < str_array->size() )
	{
		*out_text = (*str_array)[idx].c_str();
		return true;
	}

	return false;
}

void FSEditor::UIDrawEditor( bool* bshow_editor )
{
	CoShip* pcoship = ShipManager::GetStaticInstance()->_GetShip();
    CoPath* pcopath = PathManager::GetStaticInstance()->_GetCurrentPath();
    
    ImGui::Begin("Editor", bshow_editor, ImVec2(200,400), -1.f, 0/*ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar*/ );

#if 0
	ImGui::ShowTestWindow();
#else

	//float dist_level = pShip->m_path_dist_level;
	//float max_dist_level = pPath->m_clamped_sum_distance;

	if( ImGui::CollapsingHeader("Camera") )
	{
		ImGui::SliderFloat("PathDist", &pcoship->m_path_dist_level, 0.f, pcopath->m_clamped_sum_distance);
	}
	if( ImGui::CollapsingHeader("Path") )
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 5.0f);
		ImGui::BeginChild("Sub2", ImVec2(0,300), true);
		ImGui::Text("Control points");

		Array<String> str_cp_array;
		for( int cp_idx = 0; cp_idx < pcopath->m_ctrl_points.size(); cp_idx++ )
			str_cp_array.push_back( String::Printf( "%d", cp_idx ) );

        ImGui::PushItemWidth( 50 );
		if( ImGui::ListBox( "", &m_current_cp_idx, GetItemStringArray, &str_cp_array, str_cp_array.size(), 8 ) )
		{
			if( m_current_cp_idx >= 0 && m_current_cp_idx < pcopath->m_ctrl_points.size() )
			{
				float knot_dist = pcopath->GetClampedKnotDistance( m_current_cp_idx );
				if( pcopath->m_clamped_knot_distance > 0.f )
					pcoship->m_path_dist_level = pcopath->m_clamped_sum_distance * (knot_dist / pcopath->m_clamped_knot_distance);
				else
					pcoship->m_path_dist_level = 0.f;
			}
		}
        ImGui::PopItemWidth();

		ImGui::SameLine();
        ImGui::BeginChild("Action", ImVec2(0,100), true);
        if( m_current_cp_idx >= 0 && m_current_cp_idx < pcopath->m_ctrl_points.size() )
        {
            if( ImGui::Button( "here" ) )
            {
                float knot_dist = pcopath->GetClampedKnotDistance( m_current_cp_idx );
                if( pcopath->m_clamped_knot_distance > 0.f )
                    pcoship->m_path_dist_level = pcopath->m_clamped_sum_distance * (knot_dist / pcopath->m_clamped_knot_distance);
                else
                    pcoship->m_path_dist_level = 0.f;
            }
            ImGui::SameLine();
            bool insert_before = ImGui::Button( "ins. before" );
            ImGui::SameLine();
            bool insert_after = ImGui::Button( "ins. after" );
            if( insert_before || insert_after )
            {
                pcopath->InsertControlPoint( m_current_cp_idx, insert_after );
            }
            ImGui::SameLine();

			CameraCtrl_Base* cam_ctrl = Controller::GetStaticInstance()->GetActiveCameraCtrl();
			FSCameraCtrl_EditPath* cam_edit = nullptr;
			if( cam_ctrl && cam_ctrl->IsA( FSCameraCtrl_EditPath::StaticClass() ) )
				cam_edit = static_cast<FSCameraCtrl_EditPath*>( cam_ctrl );

            bool edit_button = ImGui::Button( "edit" );
            if( edit_button )
            {
                if( !cam_edit )
                {
                    // Open edit mode
					Controller::GetStaticInstance()->SetActiveCameraCtrl( "FSCameraCtrl_EditPath" );
					cam_edit = static_cast<FSCameraCtrl_EditPath*>( Controller::GetStaticInstance()->GetActiveCameraCtrl() );
					if( cam_edit )
					{
						cam_edit->SetTarget( pcopath );
						cam_edit->ResetEdit( m_current_cp_idx );
					}
                }
                else
                {
                    // Close edit mode
					Controller::GetStaticInstance()->SetActiveCameraCtrl( "FSCameraCtrl_Fly" );
					cam_edit = nullptr;
                }
            }
            ImGui::SameLine();
            // disable if one remaining ctrl point...
            if( pcopath->m_ctrl_points.size() > 1 && ImGui::Button( "del" ) )
            {
                pcopath->DeleteControlPoint( m_current_cp_idx );
            }
            if( cam_edit)
            {
				cam_edit->BuildGui();
            }
        }
        ImGui::EndChild();  // Action
		ImGui::EndChild();  // Sub2
		ImGui::PopStyleVar();

	}
    
#endif
    ImGui::End();
}

bool FSEditor::Init()
{
	UIManager::GetStaticInstance()->SetDrawEditorFn( &UIDrawEditorCB );
	UIManager::GetStaticInstance()->SetToggleEditorFn( &UIOnToggleEditorCB );

	return true;
}

void FSEditor::Shutdown()
{

}
