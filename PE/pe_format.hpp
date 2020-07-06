#pragma once
#include "..\RTTI\rtti.hpp"

#include <stdint.h>
#include <Windows.h>

class PE : public RTTI
{
protected:
	struct Section_Header
	{
		std::string	m_name;				//An 8-byte, null-padded UTF-8 encoded string
		uint32_t	m_section_address;	//For executable images, the address of the first byte of the section relative to the image base when the section is loaded into memory
		uint32_t	m_page_size;		//The total size of the section

		Section_Header() :
			m_name				{},
			m_section_address	{},
			m_page_size			{}
		{}
		~Section_Header() {}

		void clear()
		{
			this->m_name.clear();
			this->m_section_address = 0;
			this->m_page_size = 0;
		}
	};
private:
	PIMAGE_DOS_HEADER	m_dos_header;			//Dos Header
	short				m_e_magic;				//IMAGE_DOS_SIGNATURE

	PIMAGE_NT_HEADERS32	m_nt_header;			//Nt Header
	uint32_t			m_signature;			//IMAGE_NT_SIGNATURE

	PIMAGE_FILE_HEADER	m_file_header;			//File Header
	short				m_num_of_sections;		//Number of sections
	short				m_size_optional_header;	//Size of optional header

	uint32_t			m_section_header;		//Section table address

	const bool is_valid_file()	const { return m_e_magic	== IMAGE_DOS_SIGNATURE; };
	const bool is_pe_file()		const { return m_signature	== IMAGE_NT_SIGNATURE;  };

	bool get_section_header( const uint32_t module_base );
protected:
	PE() :
		m_dos_header			{ 0 },
		m_e_magic				{ 0 },
		m_nt_header				{ 0 },
		m_signature				{ 0 },
		m_file_header			{ 0 },
		m_num_of_sections		{ 0 },
		m_size_optional_header	{ 0 },
		m_section_header		{ 0 },
		m_section_text			{	}
	{}
	~PE() {}
	Section_Header		m_section_text;			//Basic info section .text
	std::vector< char >	m_buffer;

	bool set_section_text( const uint32_t module_base );

	bool read_buffer();
public:
	PE( const PE & )			= delete;
	PE &operator=( const PE & ) = delete;
};

