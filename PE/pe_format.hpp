#pragma once
#include "..\RTTI\rtti.hpp"

#include <stdint.h>
#include <Windows.h>

class PE : public RTTI
{
protected:
	/// <summary>
	/// Stores the name of the section, its beginning and size for search.
	/// </summary>
	struct Section_Text
	{
		/// <summary>
		/// An 8-byte, null-padded UTF-8 encoded string
		/// </summary>
		std::string m_name;
		/// <summary>
		/// For executable images, the address of the first byte of the section relative to the image base when the section is loaded into memory
		/// </summary>
		uint32_t    m_section_address;
		/// <summary>
		/// The total size of the section
		/// </summary>
		uint32_t    m_page_size;

		Section_Text() :
			m_name            {},
			m_section_address {},
			m_page_size       {}
		{}
		~Section_Text() {}

		void clear()
		{
			this->m_name.clear();
			this->m_section_address = 0;
			this->m_page_size       = 0;
		}
	};
private:
	/// <summary>
	/// Dos Header
	/// </summary>
	PIMAGE_DOS_HEADER   m_dos_header;
	/// <summary>
	/// IMAGE_DOS_SIGNATURE
	/// </summary>
	short               m_e_magic;
	/// <summary>
	/// NT Header
	/// </summary>
	PIMAGE_NT_HEADERS32 m_nt_header;
	/// <summary>
	/// IMAGE_NT_SIGNATURE
	/// </summary>
	uint32_t            m_signature;
	/// <summary>
	/// File Header
	/// </summary>
	PIMAGE_FILE_HEADER  m_file_header;
	/// <summary>
	/// Number of sections
	/// </summary>
	short               m_num_of_sections;
	/// <summary>
	/// Size of optional header
	/// </summary>
	short               m_size_optional_header;
	/// <summary>
	/// Section table address
	/// </summary>
	uint32_t            m_section_header;

	/// <summary>
	/// Validates that it is a valid PE file.
	/// </summary>
	/// <returns></returns>
	const bool is_valid_file() const { return m_e_magic	== IMAGE_DOS_SIGNATURE; };
	/// <summary>
	/// Validates whether the executable to be searched is of the PE format
	/// </summary>
	/// <returns></returns>
	const bool is_pe_file()	const { return m_signature	== IMAGE_NT_SIGNATURE;  };
	/// <summary>
	/// Gets the starting address of the section header.
	/// </summary>
	/// <param name="module_base"></param>
	/// <returns></returns>
	bool get_section_header( const uint32_t module_base );
protected:
	PE() :
		m_dos_header            {},
		m_e_magic               {},
		m_nt_header             {},
		m_signature             {},
		m_file_header           {},
		m_num_of_sections       {},
		m_size_optional_header  {},
		m_section_header        {},
		m_section_text          {}
	{}
	~PE() {}

	/// <summary>
	/// Basic info section .text
	/// </summary>
	Section_Text        m_section_text;

	/// <summary>
	/// Saves the byte buffer from the beginning of the .text section to the end.
	/// </summary>
	std::vector< char > m_buffer;

	/// <summary>
	/// Fill in m_section_text with the correct information for the name, beginning and size of the section.
	/// </summary>
	/// <param name="module_base"></param>
	/// <returns></returns>
	bool set_section_text( const uint32_t module_base );

	/// <summary>
	/// Read all bytes from the .text section
	/// </summary>
	/// <returns></returns>
	bool read_buffer();
public:
	PE( const PE & )            = delete;
	PE &operator=( const PE & ) = delete;
};

