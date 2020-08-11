#pragma once
#include "..\PE\pe_format.hpp"

#include <ostream>
#include <string>
#include <vector>

class Finder : public PE
{
protected:
	/// <summary>
	/// Stores the class pointer
	/// </summary>
	std::vector< uint32_t >                   m_pointer;
	/// <summary>
	/// Used if there is only one class instance
	/// </summary>
	std::vector< std::vector< uint32_t > >    m_pointer_instance;
	/// <summary>
	/// Stores the class name and its hierarchy
	/// </summary>
	std::vector< std::string >                m_name;
	/// <summary>
	/// Stores the local address for each class pointer or pointer to instance of class.
	/// </summary>
	std::vector< std::vector< uint32_t > >    m_reference;
	/// <summary>
	/// Stores the assembly of the address contained in a references.
	/// </summary>
	std::vector< std::vector< std::string > > m_assembly_inf;
	/// <summary>
	/// Location where the log will be stored
	/// </summary>
	std::string                               m_path;

	/// <summary>
	/// Temporary information to fill the vectors with information from the new found class or reference.
	/// </summary>
	struct Info
	{
		uint32_t    m_pointer;
		uint32_t    m_pointer_instance;
		std::string m_name;
		uint32_t    m_reference;
		std::string m_assembly_inf;

		Info( const uint32_t pointer, const uint32_t pointer_instance, const std::string name, const uint32_t reference, const std::string assembly_inf ) :
			m_pointer          { pointer },
			m_pointer_instance { pointer_instance },
			m_name             { name },
			m_reference        { reference },
			m_assembly_inf     { assembly_inf }
		{}
		
		~Info() {}
	};

	/// <summary>
	/// Checks whether a pointer already exists in our cache.
	/// </summary>
	/// <param name="pointer"></param>
	/// <returns></returns>
	const short find_ptr( const uint32_t pointer ) const;
public:
	Finder() :
		m_path	{ "class_finder.txt" }
	{}
	~Finder() {}

	/// <summary>
	/// Add a found class or reference
	/// </summary>
	/// <param name="class_info"></param>
	/// <returns></returns>
	void add( const Info class_info );

	/// <summary>
	/// Export the information to a .txt file
	/// </summary>
	/// <returns></returns>
	bool save();

	friend std::ostream& operator<<( std::ostream &out, const Finder &finder );
};
