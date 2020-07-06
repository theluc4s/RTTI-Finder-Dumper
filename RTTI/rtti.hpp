#pragma once
#include "..\Memory\memory.hpp"

#include <map>		//std::map
#include <string>	//std::string
#include <utility>	//std::pair
#include <vector>	//std::vector

class RTTI : public MemoryMgr
{
protected:
	using pair_t						= std::pair < uint32_t, std::string >;
	using cache_t						= std::map  < uint32_t, std::string >;

	RTTI() :
		m_filter	{},
		m_rtti_cache{},
		m_rtti		{}
	{}
	~RTTI(){}

	std::vector< std::string >			m_filter;
	cache_t								m_rtti_cache;
	std::string							m_rtti;

	bool find_cache						( uint32_t address );

	std::string undecorate_symbol_name	( std::string name );

	std::string read_rtti32				( uint32_t object_locator );

	std::string read_rtti				( uint32_t vftable );

public:
	RTTI( const RTTI& )					= delete;
	RTTI& operator=( const RTTI& )		= delete;
};
