#include "Graphics/implementation.hpp"

#define NMD_X86_FORMATTER_NUM_PADDING_BYTES 10
#define NMD_ASSEMBLY_IMPLEMENTATION
#include "nmd_assembly.h"

#include "Interface/Interface.hpp"

#include <iostream>	//std::cout
#include <memory>	//std::unique

int main()
{
	auto wnd{ std::make_unique< Implementation >( "Class Finder" ) };
	{
		wnd->set_class_name( "Class_Finder" );
		wnd->set_size( { 800, 600 } );

		wnd->start_window();
	}

	auto finterface	{ std::make_unique< FinderInterface		>() };
	auto ins		{ std::make_unique< NMD_X86Instruction	>() };

	while( wnd->get_msg().message != WM_QUIT )
	{
		if( PeekMessageA( &wnd->get_msg(), nullptr, 0u, 0u, 1 ) )
		{
			TranslateMessage( &wnd->get_msg() );
			DispatchMessageA( &wnd->get_msg() );
			continue;
		}

		wnd->start_frame();

		finterface->show( wnd->get_name(),										//Window name
						{ static_cast< float >( g_d3dpp.BackBufferWidth  ),		//Window width
						  static_cast< float >( g_d3dpp.BackBufferHeight ) },	//Window height
						  ins.get() );											//Pointer NMD_X86Instructioon

		wnd->end_frame();

		wnd->set_background_color( 110, 145, 190 );
	}

	system( "pause" );
}
