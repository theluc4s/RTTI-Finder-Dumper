#pragma once

#include <string>		//std::string
#include <vector>		//std::vector
#include <Windows.h>	//CloseHandle
#include <Tlhelp32.h>	//CreateToolhelp32Snapshot, Process32First, Process32Next, Module32First, Module32Next

class MemoryMgr
{
protected:
	/// <summary>
	/// Basic process information.
	/// </summary>
	struct ProcessInfo
	{
		std::string				m_name;
		unsigned long			m_pid;
		HANDLE					m_handle;
	};
	/// <summary>
	/// Basic module information.
	/// </summary>
	struct ModuleInfo
	{
		std::string				m_name;
		uint32_t				m_base_address;
		uint32_t				m_size;
	};

protected:
	MemoryMgr() :
		m_selected_process	{ -1 },
		m_selected_module	{ -1 }
	{}
	~MemoryMgr()
	{
		close_old_handle();
	}

	/// <summary>
	/// Temporarily stores information about running processes.
	/// </summary>
	std::vector< ProcessInfo >	m_process_info;
	/// <summary>
	/// Stores the modules of the selected process.
	/// </summary>
	std::vector< ModuleInfo >	m_modules;

	/// <summary>
	/// Informs which is the current selected process
	/// </summary>
	int							m_selected_process;
	/// <summary>
	/// Informs which is the current selected module
	/// </summary>
	int							m_selected_module;

	/// <summary>
	/// Get all processes running on the computer.
	/// </summary>
	void						get_processes	();
	/// <summary>
	/// Get all modules from a specific process.
	/// </summary>
	/// <param name="current_process"></param>
	/// <returns></returns>
	bool						get_modules		( const int current_process );
	/// <summary>
	/// Opens a handle for the selected process.
	/// </summary>
	/// <param name="current_process"></param>
	/// <returns></returns>
	bool						open_handle		( const int current_process );
	/// <summary>
	/// Closes the current open handle before opening a new one.
	/// </summary>
	void						close_old_handle();
	/// <summary>
	/// Checks whether the address can be valid.
	/// </summary>
	/// <param name="address"></param>
	/// <returns></returns>
	const bool					maybe_valid		( const uint32_t address ) const;

	/// <summary>
	/// Read the memory of a process remotely.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="base_address"></param>
	/// <param name="size_read"></param>
	/// <returns></returns>
	template< typename T >
	T							read_remote		( const uint32_t base_address, const size_t size_read = sizeof T );
	/// <summary>
	/// Read a string from a remote process.
	/// </summary>
	/// <param name="address"></param>
	/// <param name="size_read"></param>
	/// <returns></returns>
	std::string					read_remote_str	( const uint32_t address, const size_t size_read = 200u );
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