#include "rtti.hpp"

#include <DbgHelp.h>
#include <memory>

#pragma comment( lib, "Dbghelp.lib" )

/*

- Reversing Microsoft Visual C++ Part II: Classes, Methods and RTTI
- http://www.openrce.org/articles/full_view/23

*/

#pragma warning( push )
#pragma warning( disable : 4200 )

struct TypeDescriptor
{
	unsigned long                       hash;                  // Hash value computed from type's decorated name
	void                                *spare;                // reserved, possible for RTTI
	char                                name[];                // The decorated name of the type; 0 terminated.
};

struct PMD
{
	int                                 mdisp;                  //member displacement
	int                                 pdisp;                  //vbtable displacement
	int                                 vdisp;                  //displacement inside vbtable
};

struct RTTICompleteObjectLocator
{
	unsigned long                       signature;              //always zero ?
	unsigned long                       offset;                 //offset of this vtable in the complete class
	unsigned long                       cd_offset;              //constructor displacement offset
	struct TypeDescriptor               *p_type_descriptor;     //TypeDescriptor of the complete class
	struct RTTIClassHierarchyDescriptor *p_class_descriptor;    //describes inheritance hierarchy
};

struct RTTIClassHierarchyDescriptor
{
	unsigned long                       signature;              //always zero?
	unsigned long                       attributes;             //bit 0 set = multiple inheritance, bit 1 set = virtual inheritance
	unsigned long                       num_base_classes;       //number of classes in pBaseClassArray
	struct RTTIBaseClassArray           *p_base_class_array;
};

struct RTTIBaseClassArray 
{
	struct RTTIBaseClassDescriptor      *arrayOfBaseClassDescriptors[];
};

#pragma warning( pop )

struct RTTIBaseClassDescriptor
{
	struct TypeDescriptor               *p_type_descriptor;     //type descriptor of the class
	unsigned long                       numContainedBases;      //number of nested classes following in the Base Class Array
	struct PMD                          where;                  //pointer-to-member displacement info
	unsigned long                       attributes;             //flags, usually 0
};

const bool RTTI::find_cache( const uint32_t address )
{
	const auto cache{ this->m_rtti_cache.find( address ) };

	return ( cache != this->m_rtti_cache.end() ? this->m_rtti = cache->second, true : false );
}

std::string RTTI::undecorate_symbol_name( const std::string name )
{
	auto string{ std::make_unique< char[] >( name.size() + 1 ) };

	UnDecorateSymbolName( name.data(), string.get(), name.size(), UNDNAME_NAME_ONLY );

	return string.get();
}

std::string RTTI::read_rtti32( const uint32_t object_locator )
{
	const auto *col{ reinterpret_cast< RTTICompleteObjectLocator * >( object_locator ) };
	const auto *chd{ reinterpret_cast< RTTIClassHierarchyDescriptor * >( read_remote< uint32_t >( reinterpret_cast< uint32_t >( &col->p_class_descriptor ) ) ) };

	if( maybe_valid( reinterpret_cast< uint32_t >( &chd ) ) )
	{
		const auto num_base_classes{ read_remote< uint32_t >( reinterpret_cast< uint32_t >( &chd->num_base_classes ) ) };
		if( num_base_classes > 0 && num_base_classes < 25 )
		{
			const auto *bca{ reinterpret_cast< RTTIBaseClassArray * >( read_remote< uint32_t >( reinterpret_cast< uint32_t >( &chd->p_base_class_array ) ) ) };
			if( maybe_valid( reinterpret_cast< uint32_t >( &bca ) ) )
			{
				std::string rtti{};
				for( uint32_t index{ 0 }; index < num_base_classes; ++index )
				{
					const auto *bcd{ reinterpret_cast< RTTIBaseClassDescriptor * >( read_remote< uint32_t >( reinterpret_cast< uint32_t >( bca ) + ( 0x4 * index ) ) ) };
					if( maybe_valid( reinterpret_cast< uint32_t >( bcd ) ) )
					{
						const auto *td{ reinterpret_cast< TypeDescriptor * >( read_remote< uint32_t >( reinterpret_cast< uint32_t >( &bcd->p_type_descriptor ) ) ) };
						if( maybe_valid( reinterpret_cast< uint32_t >( td ) ) )
						{
							auto name{ read_remote_str( reinterpret_cast< uint32_t >( &td->name ) + 0x4 ) }; //alignment

							if( name.empty() )
								continue;

							if( name.find( "@@", name.size() - 2 ) != std::string::npos )
								name = undecorate_symbol_name( "?" + name );

							rtti.append( name );
							rtti.append( " : " );

							continue;
						}
					}

					break;
				}

				if( !rtti.empty() )
				{
					rtti.erase( rtti.size() - 3 );
					return rtti;
				}
			}
		}
	}
	return {};
}

std::string RTTI::read_rtti( const uint32_t vftable )
{
	if( maybe_valid( vftable ) )
	{
		if( !find_cache( vftable ) )
		{
			const auto object_locator{ read_remote< uint32_t >( vftable - 0x4 ) };
			if( maybe_valid( object_locator ) )
			{
				this->m_rtti = read_rtti32( object_locator );
				this->m_rtti_cache.insert( pair_t( vftable, this->m_rtti ) );

				return this->m_rtti;
			}
			return {};
		}
		return this->m_rtti;
	}
	return {};
}
