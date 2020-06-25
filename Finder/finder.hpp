#pragma once
#include <algorithm>	//std::find
#include <fstream>      //std::fstream
#include <string>       //std::string
#include <vector>       //std::vector

class Finder
{
private:
    static constexpr auto m_invalid_index   = -1;
    using ulong_t                           = unsigned long long;

    std::vector< ulong_t >                  m_pointer;
    std::vector< std::string >              m_name;
    std::vector< std::vector< ulong_t > >   m_reference;

    std::string                             m_path;
public:
    Finder( std::string path = "class_finder.txt" ) :
        m_pointer{},
        m_name{},
        m_path{ path }
    {}
    ~Finder() {}

    void add( const ulong_t pointer, const std::string name, const ulong_t reference );

    int find_ptr( const ulong_t pointer );

    bool save();

    friend std::ostream& operator<<( std::ostream& out, const Finder& finder );
};
