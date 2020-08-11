#pragma once
#include "..\Finder\finder.hpp"
#include "..\Graphics\imgui\imgui.h"

#include "..\nmd_assembly.h"

class FinderInterface : Finder
{
private:
	/// <summary>
	/// Tells whether the .text section is being scanned.
	/// </summary>
	bool			m_scanning;
	/// <summary>
	/// Process filter
	/// </summary>
	ImGuiTextFilter	m_filter_process;
	/// <summary>
	/// Module filter
	/// </summary>
	ImGuiTextFilter	m_filter_module;

	/// <summary>
	/// Temporary current process
	/// </summary>
	int				m_current_proc;
	/// <summary>
	/// Temporary current module
	/// </summary>
	int				m_current_mod;

	/// <summary>
	/// Displays menu items, currently only one.
	/// </summary>
	void			show_menu_items();
	/// <summary>
	/// Initializes a bar menu
	/// </summary>
	void			show_menu_bar();
	/// <summary>
	/// Displays the basic and detailed layout (as well as the modules) of the found classes.
	/// </summary>
	/// <param name="title"></param>
	/// <param name="wnd_size"></param>
	/// <param name="instruction"></param>
	void			show_layout( const std::string &title, const ImVec2 &wnd_size, NMD_X86Instruction *instruction );
	/// <summary>
	/// Displays the option to initialize the scan.
	/// </summary>
	/// <param name="instruction"></param>
	void			show_scan( NMD_X86Instruction *instruction );
	/// <summary>
	/// Initializes the buffer with the bytes of the .text section
	/// </summary>
	/// <returns></returns>
	bool			init();
	/// <summary>
	/// Starts scanning the .text section
	/// </summary>
	/// <param name="instruction"></param>
	void			start_scan( NMD_X86Instruction *instruction );
	/// <summary>
	/// Clears some vectors, modules, class information, rtti cache etc.
	/// </summary>
	void			clear();

public:
	FinderInterface() : 
				m_scanning			{ false },
				m_filter_process	{ 0 },
				m_filter_module		{ 0 },
				m_current_proc		{ 0 },
				m_current_mod		{ 0 }
	{}
	/// <summary>
	/// Displays the general interface
	/// </summary>
	/// <param name="title"></param>
	/// <param name="wnd_size"></param>
	/// <param name="instruction"></param>
	void			show( const std::string &title, const ImVec2 &wnd_size, NMD_X86Instruction *instruction );
};