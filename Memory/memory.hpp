#pragma once

#include <string>		//std::string
#include <vector>		//std::vector
#include <Windows.h>	//CloseHandle
#include <Tlhelp32.h>	//CreateToolhelp32Snapshot, Process32First, Process32Next, Module32First, Module32Next

class MemoryMgr
{
protected:
	struct ProcessInfo
	{
		std::string		m_name;
		unsigned long	m_pid;
		HANDLE			m_handle;
	};

	struct ModuleInfo
	{
		std::string		m_name;
		uint32_t		m_base_address;
		uint32_t		m_size;
	};

	MemoryMgr() :
		m_selected_process	{ -1 },
		m_selected_module	{ -1 }
	{}
	~MemoryMgr()
	{
		close_old_handle();
	}
	std::vector< ProcessInfo >	m_process_info;
	std::vector< ModuleInfo >	m_modules;

	int							m_selected_process;
	int							m_selected_module;

	void get_processes();

	bool get_modules( int index );

	bool open_handle( int index );

	void close_old_handle();

	bool maybe_valid( uint32_t address );

	template< typename T >
	T					read_remote( const uint32_t base_address, const size_t size_read = sizeof T );

	std::string         read_remote_str( const uint32_t address, const size_t size_read = 200u );
public:
	MemoryMgr( const MemoryMgr& )				= delete;
	MemoryMgr& operator=( const MemoryMgr& )	= delete;
};

template< typename T >
T MemoryMgr::read_remote( const uint32_t base_address, const size_t size_read )
{
	T buffer{};
	return ( ReadProcessMemory( m_process_info.at( m_selected_process ).m_handle, reinterpret_cast< const void * >( base_address ), reinterpret_cast< void * >( &buffer ), size_read, 0 ) ? buffer : 0 );
}