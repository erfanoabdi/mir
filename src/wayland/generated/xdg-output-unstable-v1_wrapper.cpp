/*
 * AUTOGENERATED - DO NOT EDIT
 *
 * This file is generated from xdg-output-unstable-v1.xml
 * To regenerate, run the “refresh-wayland-wrapper” target.
 */

#include "xdg-output-unstable-v1_wrapper.h"

#include <boost/throw_exception.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <wayland-server-core.h>

#include "mir/log.h"

namespace mir
{
namespace wayland
{
extern struct wl_interface const wl_output_interface_data;
extern struct wl_interface const zxdg_output_manager_v1_interface_data;
extern struct wl_interface const zxdg_output_v1_interface_data;
}
}

namespace mw = mir::wayland;

namespace
{
struct wl_interface const* all_null_types [] {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr};
}

// XdgOutputManagerV1

struct mw::XdgOutputManagerV1::Thunks
{
    static int const supported_version;

    static void destroy_thunk(struct wl_client* client, struct wl_resource* resource)
    {
        auto me = static_cast<XdgOutputManagerV1*>(wl_resource_get_user_data(resource));
        try
        {
            me->destroy();
        }
        catch(ProtocolError const& err)
        {
            wl_resource_post_error(err.resource(), err.code(), "%s", err.message());
        }
        catch(...)
        {
            internal_error_processing_request(client, "XdgOutputManagerV1::destroy()");
        }
    }

    static void get_xdg_output_thunk(struct wl_client* client, struct wl_resource* resource, uint32_t id, struct wl_resource* output)
    {
        auto me = static_cast<XdgOutputManagerV1*>(wl_resource_get_user_data(resource));
        wl_resource* id_resolved{
            wl_resource_create(client, &zxdg_output_v1_interface_data, wl_resource_get_version(resource), id)};
        if (id_resolved == nullptr)
        {
            wl_client_post_no_memory(client);
            BOOST_THROW_EXCEPTION((std::bad_alloc{}));
        }
        try
        {
            me->get_xdg_output(id_resolved, output);
        }
        catch(ProtocolError const& err)
        {
            wl_resource_post_error(err.resource(), err.code(), "%s", err.message());
        }
        catch(...)
        {
            internal_error_processing_request(client, "XdgOutputManagerV1::get_xdg_output()");
        }
    }

    static void resource_destroyed_thunk(wl_resource* resource)
    {
        delete static_cast<XdgOutputManagerV1*>(wl_resource_get_user_data(resource));
    }

    static void bind_thunk(struct wl_client* client, void* data, uint32_t version, uint32_t id)
    {
        auto me = static_cast<XdgOutputManagerV1::Global*>(data);
        auto resource = wl_resource_create(
            client,
            &zxdg_output_manager_v1_interface_data,
            std::min((int)version, Thunks::supported_version),
            id);
        if (resource == nullptr)
        {
            wl_client_post_no_memory(client);
            BOOST_THROW_EXCEPTION((std::bad_alloc{}));
        }
        try
        {
            me->bind(resource);
        }
        catch(...)
        {
            internal_error_processing_request(client, "XdgOutputManagerV1 global bind");
        }
    }

    static struct wl_interface const* get_xdg_output_types[];
    static struct wl_message const request_messages[];
    static void const* request_vtable[];
};

int const mw::XdgOutputManagerV1::Thunks::supported_version = 3;

mw::XdgOutputManagerV1::XdgOutputManagerV1(struct wl_resource* resource, Version<3>)
    : client{wl_resource_get_client(resource)},
      resource{resource}
{
    if (resource == nullptr)
    {
        BOOST_THROW_EXCEPTION((std::bad_alloc{}));
    }
    wl_resource_set_implementation(resource, Thunks::request_vtable, this, &Thunks::resource_destroyed_thunk);
}

bool mw::XdgOutputManagerV1::is_instance(wl_resource* resource)
{
    return wl_resource_instance_of(resource, &zxdg_output_manager_v1_interface_data, Thunks::request_vtable);
}

void mw::XdgOutputManagerV1::destroy_wayland_object() const
{
    wl_resource_destroy(resource);
}

mw::XdgOutputManagerV1::Global::Global(wl_display* display, Version<3>)
    : wayland::Global{
          wl_global_create(
              display,
              &zxdg_output_manager_v1_interface_data,
              Thunks::supported_version,
              this,
              &Thunks::bind_thunk)}
{}

auto mw::XdgOutputManagerV1::Global::interface_name() const -> char const*
{
    return XdgOutputManagerV1::interface_name;
}

struct wl_interface const* mw::XdgOutputManagerV1::Thunks::get_xdg_output_types[] {
    &zxdg_output_v1_interface_data,
    &wl_output_interface_data};

struct wl_message const mw::XdgOutputManagerV1::Thunks::request_messages[] {
    {"destroy", "", all_null_types},
    {"get_xdg_output", "no", get_xdg_output_types}};

void const* mw::XdgOutputManagerV1::Thunks::request_vtable[] {
    (void*)Thunks::destroy_thunk,
    (void*)Thunks::get_xdg_output_thunk};

mw::XdgOutputManagerV1* mw::XdgOutputManagerV1::from(struct wl_resource* resource)
{
    if (wl_resource_instance_of(resource, &zxdg_output_manager_v1_interface_data, XdgOutputManagerV1::Thunks::request_vtable))
    {
        return static_cast<XdgOutputManagerV1*>(wl_resource_get_user_data(resource));
    }
    return nullptr;
}

// XdgOutputV1

struct mw::XdgOutputV1::Thunks
{
    static int const supported_version;

    static void destroy_thunk(struct wl_client* client, struct wl_resource* resource)
    {
        auto me = static_cast<XdgOutputV1*>(wl_resource_get_user_data(resource));
        try
        {
            me->destroy();
        }
        catch(ProtocolError const& err)
        {
            wl_resource_post_error(err.resource(), err.code(), "%s", err.message());
        }
        catch(...)
        {
            internal_error_processing_request(client, "XdgOutputV1::destroy()");
        }
    }

    static void resource_destroyed_thunk(wl_resource* resource)
    {
        delete static_cast<XdgOutputV1*>(wl_resource_get_user_data(resource));
    }

    static struct wl_message const request_messages[];
    static struct wl_message const event_messages[];
    static void const* request_vtable[];
};

int const mw::XdgOutputV1::Thunks::supported_version = 3;

mw::XdgOutputV1::XdgOutputV1(struct wl_resource* resource, Version<3>)
    : client{wl_resource_get_client(resource)},
      resource{resource}
{
    if (resource == nullptr)
    {
        BOOST_THROW_EXCEPTION((std::bad_alloc{}));
    }
    wl_resource_set_implementation(resource, Thunks::request_vtable, this, &Thunks::resource_destroyed_thunk);
}

void mw::XdgOutputV1::send_logical_position_event(int32_t x, int32_t y) const
{
    wl_resource_post_event(resource, Opcode::logical_position, x, y);
}

void mw::XdgOutputV1::send_logical_size_event(int32_t width, int32_t height) const
{
    wl_resource_post_event(resource, Opcode::logical_size, width, height);
}

void mw::XdgOutputV1::send_done_event() const
{
    wl_resource_post_event(resource, Opcode::done);
}

bool mw::XdgOutputV1::version_supports_name()
{
    return wl_resource_get_version(resource) >= 2;
}

void mw::XdgOutputV1::send_name_event(std::string const& name) const
{
    const char* name_resolved = name.c_str();
    wl_resource_post_event(resource, Opcode::name, name_resolved);
}

bool mw::XdgOutputV1::version_supports_description()
{
    return wl_resource_get_version(resource) >= 2;
}

void mw::XdgOutputV1::send_description_event(std::string const& description) const
{
    const char* description_resolved = description.c_str();
    wl_resource_post_event(resource, Opcode::description, description_resolved);
}

bool mw::XdgOutputV1::is_instance(wl_resource* resource)
{
    return wl_resource_instance_of(resource, &zxdg_output_v1_interface_data, Thunks::request_vtable);
}

void mw::XdgOutputV1::destroy_wayland_object() const
{
    wl_resource_destroy(resource);
}

struct wl_message const mw::XdgOutputV1::Thunks::request_messages[] {
    {"destroy", "", all_null_types}};

struct wl_message const mw::XdgOutputV1::Thunks::event_messages[] {
    {"logical_position", "ii", all_null_types},
    {"logical_size", "ii", all_null_types},
    {"done", "", all_null_types},
    {"name", "2s", all_null_types},
    {"description", "2s", all_null_types}};

void const* mw::XdgOutputV1::Thunks::request_vtable[] {
    (void*)Thunks::destroy_thunk};

mw::XdgOutputV1* mw::XdgOutputV1::from(struct wl_resource* resource)
{
    if (wl_resource_instance_of(resource, &zxdg_output_v1_interface_data, XdgOutputV1::Thunks::request_vtable))
    {
        return static_cast<XdgOutputV1*>(wl_resource_get_user_data(resource));
    }
    return nullptr;
}

namespace mir
{
namespace wayland
{

struct wl_interface const zxdg_output_manager_v1_interface_data {
    mw::XdgOutputManagerV1::interface_name,
    mw::XdgOutputManagerV1::Thunks::supported_version,
    2, mw::XdgOutputManagerV1::Thunks::request_messages,
    0, nullptr};

struct wl_interface const zxdg_output_v1_interface_data {
    mw::XdgOutputV1::interface_name,
    mw::XdgOutputV1::Thunks::supported_version,
    1, mw::XdgOutputV1::Thunks::request_messages,
    5, mw::XdgOutputV1::Thunks::event_messages};

}
}
