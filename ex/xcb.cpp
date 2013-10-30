#include <xcb/xcb.h>
#include <stdexcept>

#include <cstdio>

namespace xcb
{

struct error : std::runtime_error
{
    error(const std::string& what)
     : std::runtime_error(what)
    {
    }
};

class connection_t
{
friend connection_t connect(const char *displayname, int *screenp);
private:
    xcb_connection_t* c;

    connection_t(const char *displayname, int *screenp)
     : c(xcb_connect (displayname, screenp))
    {
        if(!c) throw error("Unable to allocate xcb_connection_t");
        if(int err = has_error())
        {
            std::string what;
            switch(err)
            {
                case XCB_CONN_ERROR:
                    what = "Connection Error";
                    break;
                case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:
                    what = "Unsupported Extension";
                    break;
                case XCB_CONN_CLOSED_MEM_INSUFFICIENT:
                    what = "Insufficient Memory";
                    break;
                case XCB_CONN_CLOSED_REQ_LEN_EXCEED:
                    what = "Exceeded maximum request length";
                    break;
                case XCB_CONN_CLOSED_PARSE_ERR:
                    what = "Parse Error";
                    break;
                default:
                    what = "Unknown Error";
                    break;
            }
            
            xcb_disconnect(c);
            throw error(what);
        }
    }
    
    connection_t(const connection_t&) = delete;
    connection_t& operator=(const connection_t&) = delete;
public:
    
    connection_t(connection_t&& conn)
     : c(nullptr)
    {
        std::swap(c, conn.c);
    }
    
    const xcb_setup_t* get_setup()
    {
        return xcb_get_setup(c);
    }
    
    bool flush()
    {
        return xcb_flush(c) > 0;
    }
    
    uint32_t generate_id()
    {
        return xcb_generate_id(c);
    }
    
    int get_file_descriptor()
    {
        return xcb_get_file_descriptor(c);
    }
    
    int has_error()
    {
        return xcb_connection_has_error(c);
    }
    
    xcb_generic_event_t* wait_for_event()
    {
        return xcb_wait_for_event(c);
    }
    xcb_generic_event_t* poll_for_event()
    {
        return xcb_poll_for_event(c);
    }
    xcb_generic_event_t* poll_for_queued_event()
    {
        return xcb_poll_for_queued_event(c);
    }
    xcb_generic_error_t* request_check(xcb_void_cookie_t cookie)
    {
        return xcb_request_check(c, cookie);
    }
    void discard_reply(unsigned int sequence)
    {
        xcb_discard_reply(c, sequence);
    }
    
    ~connection_t()
    {
        if(c)
            xcb_disconnect(c);
    }
};

connection_t connect(const char *displayname = nullptr, int *screenp = nullptr)
{
    return connection_t{displayname, screenp};
}

struct roots_iterator_adapter_t
{
    const xcb_setup_t* setup;

    struct iterator
    {
        xcb_screen_iterator_t it;
        
        xcb_screen_t* operator*()
        {
            return it.data;
        }
        
        iterator& operator++()
        {
            xcb_screen_next(&it);
            return *this;
        }

        bool operator!=(const iterator& o) const
        {
            return it.rem != o.it.rem;
        }
    };
};

roots_iterator_adapter_t::iterator begin(roots_iterator_adapter_t& a)
{
    return { xcb_setup_roots_iterator(a.setup) };
}
roots_iterator_adapter_t::iterator end(roots_iterator_adapter_t& a)
{
    auto it = xcb_setup_roots_iterator(a.setup);
    it.rem = 0;
    return { it };
}

roots_iterator_adapter_t iterate_roots(const xcb_setup_t* setup)
{
    return { setup };
}

}

int main()
{
    int screen;
    auto connection = xcb::connect(nullptr, &screen);
    auto setup = connection.get_setup();
    
    for(auto screen : xcb::iterate_roots(setup))
    {
        printf ("\n");
        printf ("Informations of screen %ld:\n", screen->root);
        printf ("  width.........: %d\n", screen->width_in_pixels);
        printf ("  height........: %d\n", screen->height_in_pixels);
        printf ("  white pixel...: %ld\n", screen->white_pixel);
        printf ("  black pixel...: %ld\n", screen->black_pixel);
        printf ("\n");
    }
    
    return 0;
}
