#include "finder.hpp"

#include <algorithm>	//std::find
#include <fstream>      //std::ofstream

short Finder::find_ptr( const uint32_t something )
{
	const auto found{ std::find( this->m_pointer.begin(),
								 this->m_pointer.end(),
								 something ) };

	return ( found != this->m_pointer.end() ? static_cast< short >( std::distance( this->m_pointer.begin(), found ) ) : -1 );
}

void Finder::add( Info info )
{
	short pos{ find_ptr( info.m_ptr ) };

	if( pos != -1 )
	{
		this->m_pointer_instance.at( pos ).push_back( info.m_ptr_inst );
		this->m_reference.at( pos ).push_back( info.m_ref );
		this->m_assembly_inf.at( pos ).push_back( info.m_asm_inf );
		return;
	}

	this->m_pointer.push_back( info.m_ptr );
	this->m_pointer_instance.push_back( { info.m_ptr_inst } );

	this->m_name.push_back( info.m_name );

	this->m_reference.push_back( { info.m_ref } );
	this->m_assembly_inf.push_back( { info.m_asm_inf } );
}

bool Finder::save()
{
	std::ofstream file{};
	file.open( m_path, std::ios::trunc );

	if( file.is_open() && m_pointer.size() )
	{
		file << *this;

		file.close();

		return true;
	}

	return false;
}

std::ostream& operator<<( std::ostream &out, const Finder &finder )
{
	const size_t list_count{ finder.m_pointer.size() };

	if( !list_count )
		return out << "No information found.";

	out << std::hex;

	out << "Class Finder Dumper by luc4s!!!\n\n";

	out << "-------------------------------------------------------\n";

	out << "Process:     \t" << finder.m_process_info	.at( finder.m_selected_process ).m_name.data()	<< '\n';
	out << "Module:      \t" << finder.m_modules		.at( finder.m_selected_module  ).m_name.data()	<< '\n';
	out << "Base address:\t" << finder.m_modules		.at( finder.m_selected_module  ).m_base_address	<< '\n';
	out << "Base size:   \t" << finder.m_modules		.at( finder.m_selected_module  ).m_size			<< '\n';

	out << "-------------------------------------------------------\n\n";

	for( size_t index{ 0 }; index < list_count; ++index )
	{
		out << "Class pointer:   \t" << finder.m_pointer		  .at( index ) << '\n';

		out << "Class name:      \t" << finder.m_name.at( index ) << '\n';

		out << "Pointer instance\n";

		out << "{\n";

		const size_t inst_count{ finder.m_pointer_instance.at( index ).size() };

		for( size_t item{ 0 }; item < inst_count; ++item )
		{
			out << '\t' << finder.m_pointer_instance.at( index ).at( item );
			out << '\t' << "Pointer to instance of " << finder.m_name.at( index ) << '\n';
		}

		out << "}\n";

		out << "Memory references\n";

		out << "{\n";

		const size_t ref_count{ finder.m_reference.at( index ).size() };

		for( size_t item{ 0 }; item < ref_count; ++item )
		{
			out << '\t' << finder.m_reference	.at( index ).at( item ) << '\t';
			out << '\t' << finder.m_assembly_inf.at( index ).at( item ) << '\n';
		}

		out << "}\n";

		out << "_______________________________________________________\n";
	};

	return out;
}
