// fseditor.cpp
//

#include "../fracstar.h"
#include "fseditor.h"

//#include "engine/entity.h"
//#include "engine/entitymanager.h"
//#include "engine/controller.h"
//#include "engine/coposition.h"
#include "ui/uimanager.h"

#include "../engine/copath.h"
#include "../engine/pathmanager.h"
#include "../game/coship.h"
#include "../game/shipmanager.h"
//#include "dfmanager.h"
//#include "fscamera.h"
////#include "../cmd/cmdbuilddata.h"

FSEditor* FSEditor::ms_peditor = nullptr;

FSEditor::FSEditor() :
    m_current_cp_edit(0)
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
	CoShip* pShip = ShipManager::GetStaticInstance()->_GetShip();
    CoPath* pPath = PathManager::GetStaticInstance()->_GetCurrentPath();
    
    ImGui::Begin("Editor", bshow_editor, ImVec2(200,400), -1.f, 0/*ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar*/ );

#if 0
	ImGui::ShowTestWindow();
#else

	//float dist_level = pShip->m_path_dist_level;
	//float max_dist_level = pPath->m_clamped_sum_distance;

	if( ImGui::CollapsingHeader("Camera") )
	{
		ImGui::SliderFloat("PathDist", &pShip->m_path_dist_level, 0.f, pPath->m_clamped_sum_distance);
	}
	if( ImGui::CollapsingHeader("Path") )
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 5.0f);
		ImGui::BeginChild("Sub2", ImVec2(0,300), true);
		ImGui::Text("Control points");

		Array<String> str_cp_array;
		for( int cp_idx = 0; cp_idx < pPath->m_ctrl_points.size(); cp_idx++ )
			str_cp_array.push_back( String::Printf( "%d", cp_idx ) );

        ImGui::PushItemWidth( 50 );
		if( ImGui::ListBox( "", &m_current_cp_edit, GetItemStringArray, &str_cp_array, str_cp_array.size(), 5 ) )
		{
			if( m_current_cp_edit >= 0 && m_current_cp_edit < pPath->m_ctrl_points.size() )
			{
				pShip->m_path_dist_level = pPath->GetClampedKnotDistance( m_current_cp_edit );
			}
		}
        ImGui::PopItemWidth();

		ImGui::SameLine();
		if( ImGui::Button( "here" ) )
		{
			if( m_current_cp_edit >= 0 && m_current_cp_edit < pPath->m_ctrl_points.size() )
			{
				pShip->m_path_dist_level = pPath->GetClampedKnotDistance( m_current_cp_edit );
			}
		}
		ImGui::SameLine();
		if( ImGui::Button( "new" ) )
		{
            if( m_current_cp_edit >= 0 && m_current_cp_edit < pPath->m_ctrl_points.size() )
            {
                pPath->InsertControlPointMidWay(m_current_cp_edit );
            }
		}
		ImGui::SameLine();
		if( ImGui::Button( "del" ) )
		{
			pPath->DeleteControlPoint( m_current_cp_edit );
		}

		//ImGui::Columns(2);
		//for (int i = 0; i < 100; i++)
		//{
		//	if (i == 50)
		//		ImGui::NextColumn();
		//	char buf[32];
		//	sprintf(buf, "%08x", i*5731);
		//	ImGui::Button(buf, ImVec2(-1.0f, 0.0f));
		//}
		ImGui::EndChild();
		ImGui::PopStyleVar();


        //String str_cp;
        //Array<char> cb_buffer;
        //for( int cp_idx = 0; cp_idx < pPath->m_ctrl_points.size(); cp_idx++ )
        //{
        //    str_cp = String::Printf( "%d", cp_idx );
        //    cb_buffer += *(Array<char>*)&str_cp;
        //}
        //cb_buffer.push_back( '\0' );
        //
        //ImGui::Combo("CP", &m_current_cp_edit, cb_buffer.Data());
        
       






        
	}
    
    //for( int PIdx = 0; PIdx < PathArray.size(); PIdx++ )
    //{
    //    Name const& LevelName = PathArray[PIdx]->m_level_name;
    //    
    //    bool bTreeNode = ImGui::TreeNode(LevelName.c_str());
    //    //ImGui::SameLine(0.f, 100.f);
    //    //ImGui::Text( "[%d / %d] %.2f / %.2f ms", m_LastCallCount, m_MaxCallCount, m_fLastTimeSpent*100.0f, m_fMaxTimeSpent*100.0f );
    //    
    //    if( bTreeNode )
    //    {
    //        ImGui::TreePop();
    //    }
    //}

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
