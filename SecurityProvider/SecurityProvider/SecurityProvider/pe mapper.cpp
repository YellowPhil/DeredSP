#include <iostream>
#include <windows.h>

namespace mapper {
	namespace detail {
		std::uintptr_t allocate_region( std::size_t size_of_image ) noexcept {
			return reinterpret_cast< std::uintptr_t >( VirtualAlloc( nullptr, size_of_image, MEM_COMMIT, PAGE_EXECUTE_READWRITE ) );
		}
	}

	struct mapper_ctx_t {
	public:
		mapper_ctx_t( ) = default;
		mapper_ctx_t( const std::uint8_t* pe_file_buffer ) : m_pe_file_buffer( pe_file_buffer ) {
			m_dos_header = reinterpret_cast< PIMAGE_DOS_HEADER >( const_cast< std::uint8_t* >( m_pe_file_buffer ) );
			m_nt_headers = reinterpret_cast< PIMAGE_NT_HEADERS >( const_cast< std::uint8_t* >( m_pe_file_buffer ) + m_dos_header->e_lfanew );

			m_mapped_to = detail::allocate_region( m_nt_headers->OptionalHeader.SizeOfImage );
			if ( !m_mapped_to )
				; // @todo: throw errors
		}
		~mapper_ctx_t( ) = default;
	public:
		void map_sections( ) const {
			PIMAGE_SECTION_HEADER section_header = IMAGE_FIRST_SECTION( m_nt_headers );

			for ( int i = 0; i < m_nt_headers->FileHeader.NumberOfSections; i++ ) {
				std::memcpy(
					reinterpret_cast< void* >( m_mapped_to + section_header[ i ].VirtualAddress ),
					reinterpret_cast< void* >( const_cast< std::uint8_t* >( m_pe_file_buffer ) + section_header[ i ].PointerToRawData ),
					section_header[ i ].SizeOfRawData
				);
			}
		}

		void relocate( ) const {
			PIMAGE_BASE_RELOCATION relocation = reinterpret_cast< PIMAGE_BASE_RELOCATION >(
				m_mapped_to + m_nt_headers->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_BASERELOC ].VirtualAddress
			);
			const auto delta = m_mapped_to - m_nt_headers->OptionalHeader.ImageBase;

			while ( relocation->VirtualAddress ) {
				if ( relocation->SizeOfBlock >= sizeof( IMAGE_BASE_RELOCATION ) ) {
					const auto count = ( relocation->SizeOfBlock - sizeof( IMAGE_BASE_RELOCATION ) ) / sizeof( uint16_t );
					const auto list = reinterpret_cast< uint16_t* >( relocation + 1 );

					for ( int i = 0; i < count; i++ ) {
						if ( list[ i ] ) {
							uintptr_t* ptr = reinterpret_cast< uintptr_t* >( m_mapped_to + ( relocation->VirtualAddress + ( list[ i ] & 0xFFF ) ) );
							*ptr += delta;
						}
					}
				}

				relocation = reinterpret_cast< PIMAGE_BASE_RELOCATION >( reinterpret_cast< uintptr_t >( relocation ) + relocation->SizeOfBlock );
			}
		}
	public:
		const std::uint8_t* m_pe_file_buffer = nullptr;
		std::uintptr_t m_mapped_to = 0x0;

		PIMAGE_DOS_HEADER m_dos_header = nullptr;
		PIMAGE_NT_HEADERS m_nt_headers = nullptr;
	};

	/*
		@note: @es3n1n: Maps PE image to a memory and returns context with already mapped image
	*/
	mapper_ctx_t map_pe( const std::uint8_t* pe_file_buffer ) {
		mapper_ctx_t ctx( pe_file_buffer );
		ctx.map_sections( );
		ctx.relocate( );
		return ctx;
	}
}
