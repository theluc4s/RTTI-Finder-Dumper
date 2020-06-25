#include "finder.hpp"

void Finder::add( const ulong_t pointer, const std::string name, const ulong_t reference )
{
    int index{ find_ptr( pointer ) };

    if ( index != m_invalid_index )
    {
        this->m_reference.at( index ).push_back( reference );
        return;
    }

    this->m_pointer.push_back   ( pointer );
    this->m_name.push_back      ( name );
    this->m_reference.push_back ( { reference } );
}

int Finder::find_ptr( const ulong_t pointer )
{
    const auto found{ std::find( this->m_pointer.begin(), this->m_pointer.end(), pointer ) };

    return ( found != this->m_pointer.end() ? static_cast< int >( std::distance( this->m_pointer.begin(), found ) ) : m_invalid_index );
}

bool Finder::save()
{
    std::ofstream file{};
    file.open( m_path, std::ios::trunc );

    if ( file.is_open() )
    {
        file << *this;

        file.close();

        return true;
    }

    return false;
}

std::ostream& operator<<( std::ostream& out, const Finder& finder )
{
    out << std::hex;

    size_t ptr_size{ finder.m_pointer.size() };

    for ( size_t index{ 0 }; index < ptr_size; ++index )
    {
        out << "m_pointer:    " << finder.m_pointer.at( index ) << '\n';
        out << "m_name:       " << finder.m_name.at( index ) << '\n';
        out << "m_references: [ ";

        size_t ref_size{ finder.m_reference.at( index ).size() };

        for ( size_t sub_index{ 0 }; sub_index < ref_size; ++sub_index )
        {
            out << finder.m_reference.at( index ).at( sub_index );
            if ( ( sub_index + 1 ) != ref_size )
                out << ',';
        }

        out << " ]\n";
        out << "------------------------\n";
    };

    out << std::dec;

    return out;
}
