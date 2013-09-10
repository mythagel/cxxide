include guards

namespace xcb
{

class connection
{
xcb_connection_t conn;
public:
    void flush();
    uint32_t get_maximum_request_length();
    void prefetch_maximum_request_length();
    xcb_generic_event_t* wait_for_event();
    xcb_generic_event_t* poll_for_event();
    xcb_generic_event_t* poll_for_queued_event();
    xcb_generic_error_t* request_check(xcb_void_cookie_t cookie);
    void discard_reply(unsigned int sequence);
    // blah.
}

}
