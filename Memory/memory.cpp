#include "memory.hpp"

#include <memory>

void MemoryMgr::get_processes()
{
	close_old_handle();

	const auto h_snapshot{ CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 ) };

	PROCESSENTRY32 process{ sizeof process };

	m_process_info.clear();

	if( Process32First( h_snapshot, &process ) )
	{
		do
		{
			m_process_info.push_back( { process.szExeFile, process.th32ProcessID, nullptr } );
		} while( Process32Next( h_snapshot, &process ) );
	}

	CloseHandle( h_snapshot );
}

bool MemoryMgr::get_modules( int index )
{
	const auto h_snapshot{ CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, m_process_info.at( index ).m_pid ) };

	MODULEENTRY32 module{ sizeof module };

	m_modules.clear();

	if( Module32First( h_snapshot, &module ) )
	{
		do
		{
			m_modules.push_back( { module.szModule, reinterpret_cast< uint32_t >( module.modBaseAddr ), module.modBaseSize } );
		} while( Module32Next( h_snapshot, &module ) );
	}

	CloseHandle( h_snapshot );

	return !m_modules.empty();
}

bool MemoryMgr::open_handle( int index )
{
	close_old_handle();

	return ( m_process_info.at( index ).m_handle = OpenProcess( PROCESS_ALL_ACCESS, PROCESS_VM_READ, m_process_info.at( index ).m_pid ) ) != 0;
}

void MemoryMgr::close_old_handle()
{
	if( m_selected_process >= 0 && m_process_info.at( m_selected_process ).m_handle )
	{
		CloseHandle( m_process_info.at( m_selected_process ).m_handle );
	}
}

bool MemoryMgr::maybe_valid( uint32_t address )
{
	return ( address >= 0x10000 && address <= 0x7f00'0000 );
}

std::string MemoryMgr::read_remote_str( const  uint32_t address, const size_t size_read )
{
	auto string{ std::make_unique< char[] >( size_read + 1 ) };

	if( !ReadProcessMemory( m_process_info.at( m_selected_process ).m_handle, reinterpret_cast< const void * >( address ), reinterpret_cast< void * >( string.get() ), size_read, 0 ) )
		return {};

	if( !string[ 0 ] )
		return {};

	for( size_t index{ 0 }; index < strlen( string.get() ); ++index )
	{
		uint8_t character{ static_cast< unsigned char >( string[ index ] ) };
		if( !isprint( character ) )
			return {};
	}

	return string.get();
}
