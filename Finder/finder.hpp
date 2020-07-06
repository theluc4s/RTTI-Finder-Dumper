#pragma once
#include "..\PE\pe_format.hpp"

#include <ostream>      //std::ostream
#include <string>		//std::string
#include <vector>		//std::vector

class Finder : public PE
{
protected:
	std::vector< uint32_t >						m_pointer;          //Stores the class pointer
	std::vector< std::vector< uint32_t > >		m_pointer_instance; //Used if there is only one class instance
	std::vector< std::string >					m_name;             //Stores the class name and its hierarchy
	std::vector< std::vector< uint32_t > >		m_reference;        //Stores the local address for each class pointer-.

	std::vector< std::vector< std::string > >	m_assembly_inf;     //Used to store the pointer assembly in m references

	std::string                                 m_path;             //Location where the log will be stored

	struct Info
	{
		uint32_t   m_ptr;
		uint32_t   m_ptr_inst;
		std::string m_name;
		uint32_t   m_ref;

		std::string m_asm_inf;
	};

	short find_ptr( const uint32_t something );
public:
	Finder( std::string path = "class_finder.txt" ) :
		m_path	{ path }
	{}
	~Finder() {}

	void add( Info info );
	bool save();

	friend std::ostream& operator<<( std::ostream &out, const Finder &finder );
};
