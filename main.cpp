#include "Graphics/implementation.hpp"

#define NMD_X86_FORMATTER_NUM_PADDING_BYTES 10
#define NMD_ASSEMBLY_IMPLEMENTATION
#include "nmd_assembly.h"

#include "Interface/Interface.hpp"

#include <memory>

int CALLBACK WinMain(
	_In_ HINSTANCE,
	_In_opt_ HINSTANCE,
	_In_ LPSTR,
	_In_ int
)
{
	auto wnd{ std::make_unique< Implementation >( "Class Finder Dumper by Lucas" ) };
	{
		wnd->set_class_name( "Class_Finder" );
		wnd->set_size( { 800, 600 } );

		wnd->start_window();
	}

	auto finterface { std::make_unique< FinderInterface		>() };
	auto ins        { std::make_unique< NMD_X86Instruction	>() };

	while( wnd->get_msg().message != WM_QUIT )
	{
		if( PeekMessageA( &wnd->get_msg(), nullptr, 0u, 0u, 1 ) )
		{
			TranslateMessage( &wnd->get_msg() );
			DispatchMessageA( &wnd->get_msg() );
			continue;
		}

		wnd->start_frame();

		finterface->show( wnd->get_name(),
						{ static_cast< float >( g_d3dpp.BackBufferWidth  ),
						  static_cast< float >( g_d3dpp.BackBufferHeight ) },
						  ins.get() );

		wnd->end_frame();

		wnd->set_background_color( 110, 145, 190 );
	}
}
