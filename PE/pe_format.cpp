#include "pe_format.hpp"

bool PE::get_section_header( const uint32_t module_base )
{
	m_dos_header = reinterpret_cast< PIMAGE_DOS_HEADER >( module_base );

	if( m_dos_header )
	{
		m_e_magic				= read_remote< short >( reinterpret_cast< uint32_t >( &m_dos_header->e_magic ) );
		if( !is_valid_file() )
			return false;

		m_nt_header = reinterpret_cast< PIMAGE_NT_HEADERS32 >( reinterpret_cast< uint32_t >( m_dos_header ) + read_remote< uint32_t >( reinterpret_cast< uint32_t >( &m_dos_header->e_lfanew ) ) );

		m_signature				= read_remote< uint32_t >( reinterpret_cast< uint32_t >( &m_nt_header->Signature ) );
		if( !is_pe_file() )
			return false;

		m_file_header			= reinterpret_cast< PIMAGE_FILE_HEADER >( &m_nt_header->FileHeader );
		m_num_of_sections		= read_remote< short >( reinterpret_cast< uint32_t >( &m_file_header->NumberOfSections ) );
		m_size_optional_header	= read_remote< short >( reinterpret_cast< uint32_t >( &m_file_header->SizeOfOptionalHeader ) );


		m_section_header		= reinterpret_cast< uint32_t >( m_nt_header ) + 0x18 + m_size_optional_header;		//0x18 - size of File Header

		return true;
	}

	return false;
}

bool PE::set_section_text( const uint32_t module_base )
{
	if( get_section_header( module_base ) )
	{
		for( short index{ 0 }; index < m_num_of_sections; ++index )
		{
			const auto section_header			{ reinterpret_cast< PIMAGE_SECTION_HEADER >( m_section_header /*+ ( IMAGE_SIZEOF_SECTION_HEADER * index )*/ ) };

			std::string name					{ read_remote_str( reinterpret_cast< uint32_t >( section_header ), 8 ) };

			if( name != ".text" )
				continue;

			uint32_t va							{ read_remote< uint32_t >( reinterpret_cast< uint32_t >( &section_header->Misc.VirtualSize ) ) };
			uint32_t rd							{ read_remote< uint32_t >( reinterpret_cast< uint32_t >( &section_header->SizeOfRawData ) ) };

			m_section_text.m_name				= name;
			m_section_text.m_section_address	= module_base + read_remote< uint32_t >( reinterpret_cast< uint32_t >( &section_header->VirtualAddress ) );
			m_section_text.m_page_size			= ( va > rd ? va : rd );

			return true;
		}
	}
	return false;
}

bool PE::read_buffer()
{
	if( maybe_valid( m_section_text.m_section_address ) )
	{
		m_buffer.clear();
		m_buffer.resize( m_section_text.m_page_size );

		return ReadProcessMemory( m_process_info.at( m_selected_process ).m_handle, reinterpret_cast< const void * >( m_section_text.m_section_address ), reinterpret_cast< void * >( &m_buffer[ 0 ] ), m_buffer.size(), 0 );
	}
	return false;
}
