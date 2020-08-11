#pragma once
#include "..\Memory\memory.hpp"

#include <map>		//std::map
#include <string>	//std::string
#include <utility>	//std::pair
#include <vector>	//std::vector

class RTTI : public MemoryMgr
{
protected:
	using pair_t	= std::pair < uint32_t, std::string >;
	using cache_t	= std::map  < uint32_t, std::string >;

	RTTI() :
		m_rtti_cache{},
		m_rtti		{}
	{}
	~RTTI(){}

	/// <summary>
	/// Stores a cache to avoid always getting a pointer's rtti.
	/// </summary>
	cache_t			m_rtti_cache;
	/// <summary>
	/// Temporary RTTI
	/// </summary>
	std::string		m_rtti;

	/// <summary>
	/// Searches for an address in the cache.
	/// </summary>
	/// <param name="address"></param>
	/// <returns></returns>
	const bool		find_cache( const uint32_t address );
	/// <summary>
	/// Formats the symbol for better visualization.
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	std::string		undecorate_symbol_name( const std::string name );
	/// <summary>
	/// Searches RTTI for a specified address.
	/// </summary>
	/// <param name="object_locator"></param>
	/// <returns></returns>
	std::string		read_rtti32( const uint32_t object_locator );
	/// <summary>
	/// Checks whether the address is valid and is not present in the cache.
	/// </summary>
	/// <param name="vftable"></param>
	/// <returns></returns>
	std::string		read_rtti( const uint32_t vftable );

public:
	RTTI( const RTTI& )					= delete;
	RTTI& operator=( const RTTI& )		= delete;
};
