/*
- This header is free to use and uses the MIT license.

*********************************************************************************************************
* This header was developed exclusively for windows, it makes use of at least one function of WinUser.h *
*********************************************************************************************************

- It was developed based on the main.cpp of the example_win32_directx9 of ImGui.

- It was designed on top of ImGui 1.76 release and there is no guarantee of operation in other previous versions.

- It allows the user to create an interface based on Win32 - DirectX9 with ImGui.
- In a few lines of code and does not require std::c++14 or later for this.

- DirectX10, 11 and 12 are not supported.

-> Special thanks to github.com/Darkratos and github.com/Nomade040 for their suggestions for improvements.

*/

#pragma once

// ImGui Headers
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

// DirectX includes
#include <d3d9.h>
#include <exception>    //std::exception
#include <string>       //std::string
#pragma comment(lib, "d3d9.lib")

// Templates
template< typename T, typename A >
T low_order( A data )
{
    return ( ( sizeof( T ) < sizeof( A ) ) ? static_cast< T >( data & 0xffffffff ) : throw );
}
template< typename T, typename A >
T high_order( A data )
{
    return ( ( sizeof( T ) < sizeof( A ) ) ? static_cast< T >( ( data >> ( sizeof( T ) * 8 ) ) & 0xffffffff ) : throw );
}

// Global prototypes
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT __stdcall WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

static LPDIRECT3D9              g_pd3d          { 0 };
static LPDIRECT3DDEVICE9        g_pd3d_device   { 0 };
static D3DPRESENT_PARAMETERS    g_d3dpp         { 0 };

// The reset device is a static function because WindowProc uses it
static void reset_device();

// Creates and registers a window directly from the constructor.
class Implementation_Window
{
public:
    // Enum class style preset
    enum class class_style
    {
        VREDRAW = 0x0001,
        HREDRAW = 0x0002,
        DBLCLKS = 0x0008,
        OWNDC = 0x0020,
        CLASSDC = 0x0040,
        PARENTDC = 0x0080,
        NOCLOSE = 0x0200,
        SAVEBITS = 0x0800,
        BYTEALIGNCLIENT = 0x1000,
        BYTEALIGNWINDOW = 0x2000,
        GLOBALCLASS = 0x4000
    };

protected:
    //x and y for pos or size
    struct vector2
    {
        int x;
        int y;
    };

    // If a m_hwnd is not nullptr then CreateWindowA has returned success and we have a window.
    HWND        m_hwnd           {};
    // If m_wnd_class is not nullptr, then our window structure provided by user parameters has been generated.
    WNDCLASSEX  m_window_class   {};

    // Defines the class name and the window name.
    std::string m_window_name    {};
    std::string m_class_name     {};

    // Saves the window size and startup position.
    vector2 m_size               {};
    vector2 m_pos                {};

    // Class of our window.
    class_style m_class_style    {};

    // Style of our window.
    unsigned long m_window_style {};

    // Builder will provide the necessary arguments for creating our window class and then the window.
    // If CreateWindowA fails then m_hwnd will be nullptr.
    Implementation_Window( const std::string &window_name,
                           const std::string &class_name,
                           vector2 size,
                           vector2 pos,
                           const class_style &cs,
                           unsigned long ws ) :
        m_hwnd {},
        m_window_class {},
        m_window_name { window_name },
        m_class_name { class_name },
        m_size { size },
        m_pos { pos },
        m_class_style { cs },
        m_window_style { ws }
    {}

    // The destructor will destroy our window, invalidating m_hwnd and unregistering the window.
    ~Implementation_Window()
    {
        DestroyWindow    ( m_hwnd );
        UnregisterClassA ( m_window_class.lpszClassName, m_window_class.hInstance );
    }
public:
    // No builders available to create a new object of type
    Implementation_Window ( const Implementation_Window& )           = delete;
    Implementation_Window& operator=( const Implementation_Window& ) = delete;

    // Const references return to variables that can only be accessed.
    const HWND&         get_hwnd()           const { return m_hwnd; }
    const WNDCLASSEX&   get_wnd_class()      const { return m_window_class; }
    const std::string&  get_name()           const { return m_window_name; }
    const std::string&  get_class_name()     const { return m_class_name; }
    const vector2&      get_size()           const { return m_size; }
    const vector2&      get_pos()            const { return m_pos; }
    const class_style&  get_class_style()    const { return m_class_style; }
    const unsigned long get_wnd_style()      const { return m_window_style; }
    const vector2       get_max_resolution() const
    {
        const HWND h_wnd { GetDesktopWindow() };

        RECT lp_rect{};

        GetWindowRect( h_wnd, &lp_rect );

        return { static_cast< int >( lp_rect.right ), static_cast< int >( lp_rect.bottom ) };
    }

    // The functions below allow you to edit members individually.
    void set_name       ( std::string name )       { m_window_name = name; }
    void set_class_name ( std::string class_name ) { m_class_name = class_name; }
    void set_size       ( vector2 init_wnd_size )  { m_size = init_wnd_size; }
    void set_pos        ( vector2 init_wnd_pos )   { m_pos = init_wnd_pos; }
    void set_class_style( class_style cs )         { m_class_style = cs; }
    void set_wnd_style  ( unsigned long wnd_style ){ m_window_style = wnd_style; }
};

// It has device controland directx functions.
class Implementation_Render : public Implementation_Window
{
protected:
    // Release the buffer via "Release()" and set the pointer to nullptr, so it is ready to use again.
    void clear_device_d3d() noexcept
    {
        if ( g_pd3d_device )
        {
            g_pd3d_device->Release();
            g_pd3d_device = nullptr;
        }
        if ( g_pd3d )
        {
            g_pd3d->Release();
            g_pd3d = nullptr;
        }
    }

    // Creates the device and defines vsync or not.
    bool create_device_d3d( const bool vsync = true )
    {
        if ( ( g_pd3d = Direct3DCreate9( D3D_SDK_VERSION ) ) == NULL )
            return false;

        // Create the D3DDevice
        ZeroMemory( &g_d3dpp, sizeof( g_d3dpp ) );
        g_d3dpp.Windowed = TRUE;
        g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
        g_d3dpp.EnableAutoDepthStencil = TRUE;
        g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
        // Present with vertical sincronization
        // Present without vertical sincronization, maximum unthrottled framerate
        // The boolean m_vertical_sincronization define this.
        g_d3dpp.PresentationInterval = ( vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE );
        if ( g_pd3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3d_device ) < 0 )
            return false;

        return true;
    }

    // The builder is responsible for calling the "Implementation_Window" builder.
    Implementation_Render( const std::string &window_name,
                           const std::string &class_name,
                           vector2 size,
                           vector2 pos,
                           const class_style &cs,
                           unsigned long ws ) :
    Implementation_Window( window_name,
                           class_name,
                           size,
                           pos,
                           cs,
                           ws )
    {}

    // Clear device
    ~Implementation_Render()
    {
        clear_device_d3d();
    }
public:
    //No builders available to create a new object of type
    Implementation_Render( const Implementation_Render& )               = delete;
    Implementation_Render& operator=( const Implementation_Render& )    = delete;
};

//std::string &window_name
//std::string &class_name = "ImGui Basic Setup"
//vector2 size { int x, int y } = { 1280, 800 }
//vector2 pos  { int x, int y } = { 0, 0 }
//style_class &class_style = style_class::CLASSDC
class Implementation final : public Implementation_Render
{
public:
    //Enum Imgui preset set color style
    enum class color_style
    {
        classic,
        dark,
        light
    };
private:
    // MSG struct to while loop main.cpp
    MSG m_msg{};

    // Initializes the context of imgui and sets the color style to classic.
    void setup_imgui_context()
    {
        ImGui::CreateContext();
        set_style( color_style::classic );
    }

    // Implements Win32 and DirectX
    // get_hwnd() returns a reference to the m_hwnd variable
    void setup_imgui_impl()
    {
        if( !ImGui_ImplWin32_Init( get_hwnd() ) || !ImGui_ImplDX9_Init( g_pd3d_device ) )
            throw std::exception( "Implementation Win32 or DX9 ImGui failed!" );
    }
public:
    //window_name is required.
    Implementation( const std::string &window_name,
                    const std::string &class_name = "ImGui DX9 Implementation",
                    vector2 size = { 1280, 800 },
                    vector2 pos = { 0, 0 },
                    const class_style &cs = class_style::CLASSDC,
                    unsigned long ws = WS_OVERLAPPEDWINDOW ) :
        Implementation_Render( window_name,
                           class_name,
                           size,
                           pos,
                           cs,
                           ws ),
        m_msg {}
    {}

    // ImGui Destroy
    ~Implementation()
    {
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    // Toggles color style simply by making a call and passing a member of the enum style_color
    void set_style( const color_style& style ) const
    {
        switch ( style )
        {
        case color_style::classic:
            ImGui::StyleColorsClassic();
            break;
        case color_style::dark:
            ImGui::StyleColorsDark();
            break;
        case color_style::light:
            ImGui::StyleColorsLight();
            break;
        default:
            ImGui::StyleColorsClassic();
            break;
        }
    }

    // A wrapper for the background color of the window.
    // If no arguments are passed it will be black.
    void set_background_color( const int r = 0, const int g = 0, const int b = 0 )
    {
        g_pd3d_device->SetRenderState( D3DRS_ZENABLE, 0             );
        g_pd3d_device->SetRenderState( D3DRS_ALPHABLENDENABLE, 0    );
        g_pd3d_device->SetRenderState( D3DRS_SCISSORTESTENABLE, 0   );
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA( r, g, b, 255 );
        g_pd3d_device->Clear( 0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0 );
    }

    // A wrapper for the New Frame
    void start_frame()
    {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    // A wrapper for the End Frame.
    // And also the device reset.
    void end_frame()
    {
        ImGui::EndFrame();
        if ( g_pd3d_device->BeginScene() >= 0 )
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData() );
            g_pd3d_device->EndScene();
        }
        HRESULT result = g_pd3d_device->Present( nullptr, nullptr, nullptr, nullptr);

        if ( result == D3DERR_DEVICELOST && g_pd3d_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET )
            reset_device();
    }

    // Returns a reference to our variable m_msg;
    // get_msg().message for example.
    MSG& get_msg() { return m_msg; }

     void start_window()
     {
        // Get current resolution of desktop
        vector2 desktop { get_max_resolution() };

        try
        {
           if ( desktop.x < m_size.x || desktop.y < m_size.y )
                throw std::exception( "Higher resolution than the resolution supported by the monitor.!" );
        }
        catch ( const std::exception &e )
        {
           MessageBoxA( m_hwnd, e.what(), "Exception", MB_ABORTRETRYIGNORE | MB_ICONSTOP );

           #if defined _DEBUG
               __debugbreak();
           #endif

           exit(0);
        }

        m_window_class = { sizeof( WNDCLASSEX ),
                           static_cast< unsigned int >( m_class_style ),
                           WndProc,
                           0, 0,
                           GetModuleHandleA( nullptr ),
                           nullptr, nullptr, nullptr, nullptr,
                           m_class_name.c_str(),
                           nullptr };

        RegisterClassExA ( &m_window_class );

        m_hwnd = CreateWindowExA( 0UL,
                                  m_window_class.lpszClassName,
                                  m_window_name.c_str(),
                                  m_window_style,
                                  m_pos.x, m_pos.y,
                                  m_size.x, m_size.y,
                                  nullptr, nullptr,
                                  m_window_class.hInstance,
                                  nullptr );
        try
        {
           // Then the device is created and the window updated.
           // If create_window_d3d fails then the device is cleared.
           if ( !create_device_d3d() )
           {
               clear_device_d3d();
               throw std::exception( "Create device D3D failed!" );
           }
        }
        catch ( const std::exception &e )
        {
           MessageBoxA( m_hwnd, e.what(), "Exception", MB_ABORTRETRYIGNORE | MB_ICONSTOP );

           #if defined _DEBUG
               __debugbreak();
           #endif

           exit(0);
        }

        ShowWindow  ( m_hwnd, SW_SHOWDEFAULT );
        UpdateWindow( m_hwnd );

        try
        {
            // Initializes the context of imgui and sets the color style to classic.
            setup_imgui_context();
            setup_imgui_impl();
        }
        catch ( const std::exception& e )
        {
            MessageBoxA( m_hwnd, e.what(), "Exception", MB_ABORTRETRYIGNORE | MB_ICONSTOP );

            #if defined _DEBUG
                __debugbreak();
            #endif

            exit(0);
        }
     }
};

// Reset the device
void reset_device()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3d_device->Reset( &g_d3dpp );
    if ( hr == D3DERR_INVALIDCALL )
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

LRESULT __stdcall WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    if ( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) )
        return true;

    switch ( msg )
    {
    case WM_SIZE:
        if ( g_pd3d_device != NULL && wParam != SIZE_MINIMIZED )
        {
            g_d3dpp.BackBufferWidth     = low_order < short, long >( lParam );
            g_d3dpp.BackBufferHeight    = high_order< short, long >( lParam );
            reset_device();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ( ( wParam & 0xfff0 ) == SC_KEYMENU ) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }
    return DefWindowProcA( hWnd, msg, wParam, lParam );
}