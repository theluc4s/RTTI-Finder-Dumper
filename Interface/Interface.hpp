#pragma once
#include "..\Finder\finder.hpp"
#include "..\Graphics\imgui\imgui.h"

#include "..\nmd_assembly.h"

#include <thread>

class FinderInterface : Finder
{
private:
	std::thread m_thread_scan;
	bool		m_finished_scan;

	char		m_filter_process[ 128 ];
	char		m_filter_module [ 128 ];

	int			m_tmp_selected_proc;
	int			m_tmp_selected_mod;

	void		show_menu_items();

	void		show_menu_bar();

	void		show_layout( const std::string &title, const ImVec2 &wnd_size, NMD_X86Instruction *instruction );

	void		show_scan( NMD_X86Instruction *instruction );

	bool		init();

	void		start_scan( NMD_X86Instruction *instruction );

	void		clear();

public:
	FinderInterface() : 
				m_finished_scan		{ true },
				m_filter_process	{ 0 },
				m_filter_module		{ 0 },
				m_tmp_selected_proc	{ 0 },
				m_tmp_selected_mod	{ 0 }
	{}

	void		show( const std::string &title, const ImVec2 &wnd_size, NMD_X86Instruction *instruction );
};