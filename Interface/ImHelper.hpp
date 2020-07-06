#pragma once
#include "..\Graphics\imgui\imgui.h"

#include <initializer_list>	//std::initializer_list
#include <string>			//std::string

namespace ImHelper
{
	void BeginColumn( const std::initializer_list< std::string > list, const std::string id, const bool column_border = true )
	{
		ImGui::Columns( list.size(), id.data(), column_border );

		ImGui::Separator();

		for( const auto &name_column : list )
		{
			ImGui::Text( "%s", name_column.data() );
			ImGui::NextColumn();
		}

		ImGui::Separator();
	}
	void ColumnWidth( const std::initializer_list< short > width )
	{
		int index{ 0 };

		for( const auto &width_column : width )
		{
			ImGui::SetColumnWidth( index, static_cast< float >( width_column ) );
			++index;
		}
	}
	void EndColumn()
	{
		ImGui::Columns();
		ImGui::Separator();
	}
}