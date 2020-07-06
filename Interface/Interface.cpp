#include "Interface.hpp"
#include "ImHelper.hpp"

#include <iostream>

void FinderInterface::show_menu_items()
{
	if( ImGui::Button( "Select Process###menu_item_process" ) )
	{
		ImGui::OpenPopup( "###process" );
		get_processes();
	}

	if( ImGui::BeginPopupModal( "Select Process###process", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove ) )
	{
		if( m_selected_process >= 0 )
		{
			ImGui::Text( "Current: %s", m_process_info.at( m_selected_process ).m_name.data() );
			ImGui::Text( "PID:     %d", m_process_info.at( m_selected_process ).m_pid );
		}

		ImGui::SetNextItemWidth( 400.f );
		ImGui::InputText( "###filter_process", m_filter_process, sizeof m_filter_process );

		if( ImGui::BeginChild( "###process_list", { 400.f, 300.f }, true ) )
		{
			ImHelper::BeginColumn( { "Name", "PID" }, "###col_process" );
			ImHelper::ColumnWidth( { 320, 50 } );
			{
				for( size_t index{ 0 }; index < m_process_info.size(); ++index )
				{
					if( m_process_info.at( index ).m_name.find( m_filter_process ) == std::string::npos )
						continue;

					if( ImGui::Selectable( m_process_info.at( index ).m_name.data(), m_tmp_selected_proc == index, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick ) )
						m_tmp_selected_proc = index;

					if( ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
					{
						if( open_handle( index ) )
						{
							m_selected_process	= index;
							m_selected_module	= 0;

							if( !get_modules( index ) || !init() )
							{
								ImGui::OpenPopup( "###invalid_section" );
								clear();
								break;
							}

							ImGui::CloseCurrentPopup();
							break;
						}

						m_selected_process = -1;
						clear();

						ImGui::OpenPopup( "###invalid_handle" );
						break;
					}

					ImGui::NextColumn();

					ImGui::Text( "%d", m_process_info.at( index ).m_pid );
					ImGui::NextColumn();
				}

				ImGui::SetNextWindowSize( { 400.f, 85.f } );
				if( ImGui::BeginPopupModal( "Invalid Handle###invalid_handle", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize ) )
				{
					ImGui::PushStyleColor( ImGuiCol_Text, { 1.f, 0.f, 0.f, 1.f } );

					ImGui::TextWrapped( "OpenProcess has failed! You may not have sufficient privilege to open a PROCESS_ALL_ACCESS handle." );

					ImGui::PopStyleColor();

					if( ImGui::Button( "Close" ) )
						ImGui::CloseCurrentPopup();
					ImGui::EndPopup();
				}

				ImGui::SetNextWindowSize( { 470.f, 85.f } );
				if( ImGui::BeginPopupModal( "Invalid Section###invalid_section", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize ) )
				{
					ImGui::PushStyleColor( ImGuiCol_Text, { 1.f, 0.f, 0.f, 1.f } );

					ImGui::TextWrapped( "It was not possible to obtain the address, size and name of the section .text!" );

					ImGui::PopStyleColor();

					if( ImGui::Button( "Close" ) )
						ImGui::CloseCurrentPopup();

					ImGui::EndPopup();
				}
			}
			ImHelper::EndColumn();
			ImGui::EndChild();
		}
		ImGui::Separator();

		if( ImGui::Button( "Refresh", { 400.f, 0.f } ) )
			get_processes();

		if( ImGui::Button( "Close", { 400.f, 0.f } ) )
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}

void FinderInterface::show_menu_bar()
{
	if( ImGui::BeginMainMenuBar() )
	{
		if( ImGui::BeginMenu( "Menu###menu_bar" ) )
		{
			show_menu_items();
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void FinderInterface::show_layout( const std::string &title, const ImVec2 &wnd_size, NMD_X86Instruction *instruction )
{
	ImGui::SetNextWindowPos( { 0, 18.f } );
	ImGui::SetNextWindowSize( { wnd_size.x, wnd_size.y - 18.f } );

	ImGui::Begin( title.data(), 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize );
	{
		if( ImGui::BeginChild( "###layout", {}, true ) )
		{
			if( !m_modules.empty() )
			{
				ImGui::Text( "Current:      " ); ImGui::SameLine();
				ImGui::TextColored( { 0.0f, 0.5f, 1.0f, 1.0f }, "%s", m_modules.at( m_selected_module ).m_name.data() );
				ImGui::Text( "Base Address: " ); ImGui::SameLine();
				ImGui::TextColored( { 0.0f, 0.5f, 1.0f, 1.0f }, "%x", m_modules.at( m_selected_module ).m_base_address );
				ImGui::Text( "Base Size:    " ); ImGui::SameLine();
				ImGui::TextColored( { 0.0f, 0.5f, 1.0f, 1.0f }, "%x", m_modules.at( m_selected_module ).m_size );

				ImGui::Separator();
			}

			if( ImGui::BeginChild( "###module_list", { 470.f, 540.f }, true ) )
			{
				ImHelper::BeginColumn( { "Name", "Base Address", "Base Size" }, "###col_modules" );
				ImHelper::ColumnWidth( { 250, 100, 100 } );
				{
					for( size_t index{ 0 }; index < m_modules.size(); ++index )
					{
						if( m_modules.at( index ).m_name.find( m_filter_module ) == std::string::npos )
							continue;

						if( ImGui::Selectable( m_modules.at( index ).m_name.data(), m_tmp_selected_mod == index, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick ) )
							m_tmp_selected_mod = index;

						if( ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
						{
							m_selected_module = index;

							if( !init() )
							{
								ImGui::OpenPopup( "###invalid_section" );
								break;
							}
						}

						ImGui::NextColumn();

						ImGui::Text( "%x", m_modules.at( index ).m_base_address );
						ImGui::NextColumn();

						ImGui::Text( "%x", m_modules.at( index ).m_size );
						ImGui::NextColumn();

					}
				}
				ImHelper::EndColumn();
				ImGui::EndChild();
			}

			ImGui::SameLine();

			ImGui::BeginGroup();
			{
				ImGui::BeginChild( "###item_view", { 0, 540.f } );
				{
					if( !m_section_text.m_name.empty() )
					{
						ImGui::Text( "Section name:    " ); ImGui::SameLine();
						ImGui::TextColored( { 0.0f, 0.5f, 1.0f, 1.0f }, "%s", m_section_text.m_name.data() );
						ImGui::Text( "Section address: " ); ImGui::SameLine();
						ImGui::TextColored( { 0.0f, 0.5f, 1.0f, 1.0f }, "%x", m_section_text.m_section_address );
						ImGui::Text( "Section size:    " ); ImGui::SameLine();
						ImGui::TextColored( { 0.0f, 0.5f, 1.0f, 1.0f }, "%x", m_section_text.m_page_size );

						ImGui::Separator();

						if( ImGui::Button( "Start Scan" ) )
							ImGui::OpenPopup( "###start_scan" );

						if( ImGui::Button( "Export Scan" ) )
						{
							if( m_process_info.at( m_selected_process ).m_handle && save() )
								ImGui::OpenPopup( "###saved_successfully" );
						}

						show_scan( instruction );

						ImGui::SetNextWindowSize( { 400.f, 0.f } );
						if( ImGui::BeginPopupModal( "Saved successfully###saved_successfully", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize ) )
						{
							ImGui::TextColored( { 0.f, 1.f, 0.f, 1.f }, "Saved successfully!" );
							ImGui::TextWrapped( "Path: %s", m_path.data() );

							if( ImGui::Button( "Close", { 400.f, 0.f } ) )
								ImGui::CloseCurrentPopup();
							ImGui::EndPopup();
						}

					}

					if( ImGui::BeginTabBar( "###tabs" ) )
					{
						if( ImGui::BeginTabItem( "Basic###basic" ) )
						{
							ImHelper::BeginColumn( { "Pointer", "Name" }, "###col_basics" );
							ImHelper::ColumnWidth( { 90, 1000 } );
							{
								for( size_t index{ 0 }; m_finished_scan && index < m_pointer.size(); ++index )
								{
									ImGui::TextColored( { 0.0f, 0.5f, 1.0f, 1.0f }, "%x", m_pointer.at( index ) );
									ImGui::NextColumn();

									ImGui::TextColored( { 1.0f, 0.5f, 0.0f, 1.0f }, "%s", m_name.at( index ).data() );
									ImGui::NextColumn();
								}
							}
							ImHelper::EndColumn();

							ImGui::EndTabItem();
						}
						if( ImGui::BeginTabItem( "Details###details" ) )
						{
							ImHelper::BeginColumn( { "Pointer", "Name", "Pointer to instances", "References" }, "###col_details" );
							ImHelper::ColumnWidth( { 90, 800, 200, 200 } );
							{
								static std::string refs_buf{};
								static std::string ptr_inst_buf{};

								for( size_t index{ 0 }; m_finished_scan && index < m_pointer.size(); ++index )
								{
									ImGui::TextColored( { 0.0f, 0.5f, 1.0f, 1.0f }, "%x", m_pointer.at( index ) );
									ImGui::NextColumn();

									ImGui::TextColored( { 1.0f, 0.5f, 0.0f, 1.0f }, "%s", m_name.at( index ).data() );
									ImGui::NextColumn();

									ImGui::TextColored( { 0.0f, 0.5f, 1.0f, 1.0f }, "%d instances", m_pointer_instance.at( index ).size() );

									if( ImGui::IsItemHovered() )
										ImGui::SetMouseCursor( ImGuiMouseCursor_Hand );

									ptr_inst_buf = "###popup_ptr_inst" + std::to_string( index );

									if( ImGui::BeginPopupContextItem( ptr_inst_buf.data(), ImGuiMouseButton_Left ) )
									{
										for( size_t pos{ 0 }; pos < m_pointer_instance.at( index ).size(); ++pos )
										{
											ImGui::InputScalar( ( ptr_inst_buf + std::to_string( pos ) ).data(), ImGuiDataType_U32, &m_pointer_instance.at( index ).at( pos ), 0, 0, "%x", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly );
										}

										ImGui::EndPopup();
									}

									ImGui::NextColumn();

									ImGui::PushStyleColor( ImGuiCol_Text, { 0.0f, 0.5f, 1.0f, 1.0f } );
									ImGui::Text( "%d references", m_reference.at( index ).size() );
									ImGui::PopStyleColor();

									if( ImGui::IsItemHovered() )
										ImGui::SetMouseCursor( ImGuiMouseCursor_Hand );

									refs_buf = "###popup_refs" + std::to_string( index );

									if( ImGui::BeginPopupContextItem( refs_buf.data(), ImGuiMouseButton_Left ) )
									{
										for( size_t pos{ 0 }; pos < m_reference.at( index ).size(); ++pos )
										{
											ImGui::SetNextItemWidth( 70.f );
											ImGui::InputScalar( ( ptr_inst_buf + "#" + std::to_string( pos ) ).data(), ImGuiDataType_U32, &m_reference.at( index ).at( pos ), 0, 0, "%x", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly );

											ImGui::SameLine();

											ImGui::InputText( ( ptr_inst_buf + "##" + std::to_string( pos ) ).data(), m_assembly_inf.at( index ).at( pos ).data(), 255, ImGuiInputTextFlags_ReadOnly );
										}

										ImGui::EndPopup();
									}

									ImGui::NextColumn();
								}
							}
							ImHelper::EndColumn();
							ImGui::EndTabItem();
						}
						ImGui::EndTabBar();
					}
				}
				ImGui::EndChild();
			}
			ImGui::EndGroup();

			ImGui::NewLine();

			ImGui::SetNextItemWidth( 470.f );
			ImGui::InputText( "###filter_modules", m_filter_module, sizeof m_filter_module );

			if( ImGui::Button( "Refresh###refresh_mod", { 470.f, 0.f } ) )
				get_modules( m_selected_process );

			ImGui::Separator();

			ImGui::EndChild();
		}
	}
	ImGui::End();
}

void FinderInterface::show_scan( NMD_X86Instruction *instruction )
{
	ImGui::SetNextWindowSize( { 400.f, 0.f } );
	if( ImGui::BeginPopupModal( "Start scan section###start_scan", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize ) )
	{
		if( ImGui::Button( "Start", { 385.f, 0.f } ) && m_finished_scan )
		{
			if( m_process_info.at( m_selected_process ).m_handle )
			{
				std::thread thread{ &FinderInterface::start_scan, this, instruction };
				thread.detach();
			}
		}

		ImGui::SetNextWindowSize( { 400.f, 0.f } );
		if( ImGui::BeginPopupModal( "Failed to read the section buffer###failed_read_buffer" ) )
		{
			ImGui::PushStyleColor( ImGuiCol_Text, { 1.f, 0.f, 0.f, 1.f } );
			ImGui::TextWrapped( "It was not possible to create a buffer for the .text section." );
			ImGui::PopStyleColor();

			if( ImGui::Button( "Close" ) )
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		if( ImGui::Button( "Close", { 385.f, 0.f } ) && m_finished_scan )
			ImGui::CloseCurrentPopup();

		if( !m_finished_scan )
			ImGui::TextColored( { 0.f, 1.f, 0.f, 1.f }, "Scanning..." );

		ImGui::EndPopup();
	}
}

bool FinderInterface::init()
{
	return set_section_text( m_modules.at( m_selected_module ).m_base_address );
}

void FinderInterface::start_scan( NMD_X86Instruction *instruction )
{
	m_finished_scan = false;

	m_pointer.clear();
	m_name.clear();
	m_pointer_instance.clear();
	m_reference.clear();
	m_assembly_inf.clear();

	if( !read_buffer() )
	{
		ImGui::OpenPopup( "###failed_read_buffer" );
		m_finished_scan = true;
		return;
	}

	size_t offset{ 0 };

	while( offset < m_buffer.size() )
	{
		uint32_t address{ reinterpret_cast< uint32_t >( &m_buffer[ 0 ] ) + offset };

		if( !nmd_x86_decode_buffer( reinterpret_cast< const void *const >( address ), m_buffer.size(), instruction, NMD_X86_MODE_32, NMD_X86_FEATURE_FLAGS_ALL ) )
		{
			++offset;
			continue;
		}

		if( instruction->numOperands )
		{
			for( size_t index{ 0 }; index < instruction->numOperands; ++index )
			{
				if( ( instruction->operands[ index ].type == NMD_X86_OPERAND_TYPE_MEMORY &&
					instruction->operands[ index ].fields.mem.base == NMD_X86_REG_NONE &&
					instruction->operands[ index ].fields.mem.index == NMD_X86_REG_NONE ) ||
					( instruction->id == NMD_X86_INSTRUCTION_PUSH &&
						instruction->immMask != NMD_X86_IMM_NONE &&
						maybe_valid( instruction->operands[ 0 ].fields.imm ) ) )
				{
					uint32_t pointer{ static_cast< uint32_t >( ( instruction->id == NMD_X86_INSTRUCTION_PUSH ? instruction->operands[ 0 ].fields.imm : instruction->operands[ index ].fields.mem.disp ) ) };

					uint32_t pointer_inst{ pointer };

					uint32_t ptr_backup{ pointer };

					std::string rtti{};
					if( maybe_valid( pointer ) )
					{
						pointer = read_remote< uint32_t >( pointer );
						if( maybe_valid( pointer ) )
						{
							rtti = read_rtti( pointer );
							if( !rtti.empty() )
							{
								pointer_inst = ptr_backup;
							}
							else
							{
								ptr_backup = pointer;
								pointer = read_remote< uint32_t >( pointer );

								if( maybe_valid( pointer ) )
								{
									rtti = read_rtti( pointer );
									if( !rtti.empty() )
									{
										pointer = ptr_backup;
									}
								}
							}
						}
					}

					if( !rtti.empty() )
					{
						char temp_insn_str[ 256 ]{ 0 };
						nmd_x86_format_instruction( instruction, temp_insn_str, address, NMD_X86_FORMAT_FLAGS_0X_PREFIX | NMD_X86_FORMAT_FLAGS_HEX );

						add( { pointer, pointer_inst, rtti, m_section_text.m_section_address + offset, temp_insn_str } );
					}
				}
			}
		}

		offset += instruction->length;
	}

	m_finished_scan = true;
}

void FinderInterface::clear()
{
	m_selected_module	= -1;

	m_modules			.clear();

	m_pointer			.clear();
	m_name				.clear();
	m_pointer_instance	.clear();
	m_reference			.clear();
	m_assembly_inf		.clear();

	m_section_text		.clear();
	m_buffer			.clear();
}

void FinderInterface::show( const std::string &title, const ImVec2 &wnd_size, NMD_X86Instruction *instruction )
{
	show_menu_bar();
	show_layout( title, wnd_size, instruction );
}