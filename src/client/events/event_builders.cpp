/*
 * Copyright © 2015 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Robert Carr <robert.carr@canonical.com>
 */

#define MIR_LOG_COMPONENT "event-builders"

#include "mir/log.h"

#include "make_empty_event.h"

#include "mir/events/event_builders.h"
#include "mir/events/event_private.h"
#include "mir/cookie/blob.h"
#include "mir/input/xkb_mapper.h"

#include <string.h>

#include <boost/throw_exception.hpp>

#include <algorithm>
#include <stdexcept>

namespace mi = mir::input;
namespace mf = mir::frontend;
namespace mev = mir::events;
namespace geom = mir::geometry;

namespace
{
    mir::cookie::Blob copy_vector_to_cookie_blob(std::vector<uint8_t> const& vector)
    {
        mir::cookie::Blob blob{{}};

        if (vector.size() > blob.size())
        {
            throw std::runtime_error("Vector size " + std::to_string(vector.size()) +
                                     " is larger then array size: " +
                                     std::to_string(blob.size()));
        }

        std::copy_n(vector.begin(), vector.size(), blob.begin());

        return blob;
    }
}

mir::EventUPtr mev::make_event(mf::SurfaceId const& surface_id, MirOrientation orientation)
{
    auto e = make_empty_event();

    e->type = mir_event_type_orientation;
    e->orientation.surface_id = surface_id.as_value();
    e->orientation.direction = orientation;
    return e;
}

mir::EventUPtr mev::make_event(MirPromptSessionState state)
{
    auto e = make_empty_event();

    e->type = mir_event_type_prompt_session_state_change;
    e->prompt_session.new_state = state;
    return e;
}

mir::EventUPtr mev::make_event(mf::SurfaceId const& surface_id, geom::Size const& size)
{
    auto e = make_empty_event();

    e->type = mir_event_type_resize;
    e->resize.surface_id = surface_id.as_value();
    e->resize.width = size.width.as_int();
    e->resize.height = size.height.as_int();
    return e;
}

mir::EventUPtr mev::make_event(mf::SurfaceId const& surface_id, MirSurfaceAttrib attribute, int value)
{
    auto e = make_empty_event();

    e->type = mir_event_type_surface;
    e->surface.id = surface_id.as_value();
    e->surface.attrib = attribute;
    e->surface.value = value;
    return e;
}

mir::EventUPtr mev::make_event(mf::SurfaceId const& surface_id)
{
    auto e = make_empty_event();

    e->type = mir_event_type_close_surface;
    e->close_surface.surface_id = surface_id.as_value();
    return e;
}

mir::EventUPtr mev::make_event(
    mf::SurfaceId const& surface_id,
    int dpi,
    float scale,
    MirFormFactor form_factor,
    uint32_t output_id)
{
    auto e = make_empty_event();

    e->type = mir_event_type_surface_output;
    e->surface_output.surface_id = surface_id.as_value();
    e->surface_output.dpi = dpi;
    e->surface_output.scale = scale;
    e->surface_output.form_factor = form_factor;
    e->surface_output.output_id = output_id;

    return e;
}

namespace
{
// Never exposed in old event, so lets avoid leaking it in to a header now.
enum 
{
    AINPUT_SOURCE_CLASS_MASK = 0x000000ff,

    AINPUT_SOURCE_CLASS_BUTTON = 0x00000001,
    AINPUT_SOURCE_CLASS_POINTER = 0x00000002,
    AINPUT_SOURCE_CLASS_NAVIGATION = 0x00000004,
    AINPUT_SOURCE_CLASS_POSITION = 0x00000008,
    AINPUT_SOURCE_CLASS_JOYSTICK = 0x00000010
};
enum 
{
    AINPUT_SOURCE_UNKNOWN = 0x00000000,

    AINPUT_SOURCE_KEYBOARD = 0x00000100 | AINPUT_SOURCE_CLASS_BUTTON,
    AINPUT_SOURCE_DPAD = 0x00000200 | AINPUT_SOURCE_CLASS_BUTTON,
    AINPUT_SOURCE_GAMEPAD = 0x00000400 | AINPUT_SOURCE_CLASS_BUTTON,
    AINPUT_SOURCE_TOUCHSCREEN = 0x00001000 | AINPUT_SOURCE_CLASS_POINTER,
    AINPUT_SOURCE_MOUSE = 0x00002000 | AINPUT_SOURCE_CLASS_POINTER,
    AINPUT_SOURCE_STYLUS = 0x00004000 | AINPUT_SOURCE_CLASS_POINTER,
    AINPUT_SOURCE_TRACKBALL = 0x00010000 | AINPUT_SOURCE_CLASS_NAVIGATION,
    AINPUT_SOURCE_TOUCHPAD = 0x00100000 | AINPUT_SOURCE_CLASS_POSITION,
    AINPUT_SOURCE_JOYSTICK = 0x01000000 | AINPUT_SOURCE_CLASS_JOYSTICK,

    AINPUT_SOURCE_ANY = 0xffffff00
};
}

mir::EventUPtr mev::make_event(MirInputDeviceId device_id, std::chrono::nanoseconds timestamp,
    std::vector<uint8_t> const& cookie, MirKeyboardAction action, xkb_keysym_t key_code,
    int scan_code, MirInputEventModifiers modifiers)
{
    auto e = make_empty_event();

    e->type = mir_event_type_key;
    auto& kev = e->key;
    kev.device_id = device_id;
    kev.source_id = AINPUT_SOURCE_KEYBOARD;
    kev.event_time = timestamp;
    kev.cookie = copy_vector_to_cookie_blob(cookie);
    kev.action = action;
    kev.key_code = key_code;
    kev.scan_code = scan_code;
    kev.modifiers = modifiers;

    return e;
}

void mev::set_modifier(MirEvent& event, MirInputEventModifiers modifiers)
{
    switch(event.type)
    {
    case mir_event_type_key:
        {
            auto& kev = event.key;
            kev.modifiers = modifiers;
            break;
        }
    case mir_event_type_motion:
        {
            auto& mev = event.motion;
            mev.modifiers = modifiers;
            break;
        }
    default:
        BOOST_THROW_EXCEPTION(std::invalid_argument("Input event modifiers are only valid for pointer, key and touch events."));
    }
}

void mev::set_cursor_position(MirEvent& event, mir::geometry::Point const& pos)
{
    if (event.type != mir_event_type_motion &&
        event.motion.source_id != AINPUT_SOURCE_MOUSE &&
        event.motion.pointer_count == 1)
        BOOST_THROW_EXCEPTION(std::invalid_argument("Cursor position is only valid for pointer events."));

    event.motion.pointer_coordinates[0].x = pos.x.as_float();
    event.motion.pointer_coordinates[0].y = pos.y.as_float();
}

void mev::set_button_state(MirEvent& event, MirPointerButtons button_state)
{
    if (event.type != mir_event_type_motion &&
        event.motion.source_id != AINPUT_SOURCE_MOUSE &&
        event.motion.pointer_count == 1)
        BOOST_THROW_EXCEPTION(std::invalid_argument("Cursor position is only valid for pointer events."));

    event.motion.buttons = button_state;
}

// Deprecated version with uint64_t mac
mir::EventUPtr mev::make_event(MirInputDeviceId device_id, std::chrono::nanoseconds timestamp,
    uint64_t /*mac*/, MirKeyboardAction action, xkb_keysym_t key_code,
    int scan_code, MirInputEventModifiers modifiers)
{
    return make_event(device_id, timestamp, std::vector<uint8_t>{}, action, key_code, scan_code, modifiers);
}

// Deprecated version without mac
mir::EventUPtr mev::make_event(MirInputDeviceId device_id, std::chrono::nanoseconds timestamp,
    MirKeyboardAction action, xkb_keysym_t key_code,
    int scan_code, MirInputEventModifiers modifiers)
{
    return make_event(device_id, timestamp, std::vector<uint8_t>{}, action, key_code, scan_code, modifiers);
}

mir::EventUPtr mev::make_event(MirInputDeviceId device_id, std::chrono::nanoseconds timestamp,
    std::vector<uint8_t> const& cookie, MirInputEventModifiers modifiers)
{
    auto e = make_empty_event();

    e->type = mir_event_type_motion;
    auto& mev = e->motion;
    mev.device_id = device_id;
    mev.event_time = timestamp;
    mev.cookie = copy_vector_to_cookie_blob(cookie);
    mev.modifiers = modifiers;
    mev.source_id = AINPUT_SOURCE_TOUCHSCREEN;

    return e;
}

// Deprecated version with uint64_t mac
mir::EventUPtr mev::make_event(MirInputDeviceId device_id, std::chrono::nanoseconds timestamp,
    uint64_t /*mac*/, MirInputEventModifiers modifiers)
{
    return make_event(device_id, timestamp, std::vector<uint8_t>{}, modifiers);
}

// Deprecated version without mac
mir::EventUPtr mev::make_event(MirInputDeviceId device_id, std::chrono::nanoseconds timestamp,
    MirInputEventModifiers modifiers)
{
    return make_event(device_id, timestamp, std::vector<uint8_t>{}, modifiers);
}

void mev::add_touch(MirEvent &event, MirTouchId touch_id, MirTouchAction action,
    MirTouchTooltype tooltype, float x_axis_value, float y_axis_value,
    float pressure_value, float touch_major_value, float touch_minor_value, float size_value)
{
    auto& mev = event.motion;
    auto& pc = mev.pointer_coordinates[mev.pointer_count++];
    pc.id = touch_id;
    pc.tool_type = tooltype;
    pc.x = x_axis_value;
    pc.y = y_axis_value;
    pc.pressure = pressure_value;
    pc.touch_major = touch_major_value;
    pc.touch_minor = touch_minor_value;
    pc.size = size_value;
    pc.action = action;
}

mir::EventUPtr mev::make_event(MirInputDeviceId device_id, std::chrono::nanoseconds timestamp,
    std::vector<uint8_t> const& cookie, MirInputEventModifiers modifiers, MirPointerAction action,
    MirPointerButtons buttons_pressed,                               
    float x_axis_value, float y_axis_value,
    float hscroll_value, float vscroll_value,
    float relative_x_value, float relative_y_value)
{
    auto e = make_empty_event();

    e->type = mir_event_type_motion;
    auto& mev = e->motion;
    mev.device_id = device_id;
    mev.event_time = timestamp;
    mev.cookie = copy_vector_to_cookie_blob(cookie);
    mev.modifiers = modifiers;
    mev.source_id = AINPUT_SOURCE_MOUSE;
    mev.buttons = buttons_pressed;

    mev.pointer_count = 1;
    auto& pc = mev.pointer_coordinates[0];
    mev.pointer_coordinates[0].action = action;
    pc.x = x_axis_value;
    pc.y = y_axis_value;
    pc.dx = relative_x_value;
    pc.dy = relative_y_value;
    pc.hscroll = hscroll_value;
    pc.vscroll = vscroll_value;

    return e;
}

// Deprecated version with uint64_t mac
mir::EventUPtr mev::make_event(MirInputDeviceId device_id, std::chrono::nanoseconds timestamp,
    uint64_t /*mac*/, MirInputEventModifiers modifiers, MirPointerAction action,
    MirPointerButtons buttons_pressed,                               
    float x_axis_value, float y_axis_value,
    float hscroll_value, float vscroll_value,
    float relative_x_value, float relative_y_value)
{
    return make_event(device_id, timestamp, std::vector<uint8_t>{}, modifiers, action,
                      buttons_pressed, x_axis_value, y_axis_value, hscroll_value,
                      vscroll_value, relative_x_value, relative_y_value);
}

// Deprecated version without mac
mir::EventUPtr mev::make_event(MirInputDeviceId device_id, std::chrono::nanoseconds timestamp,
    MirInputEventModifiers modifiers, MirPointerAction action,
    MirPointerButtons buttons_pressed,
    float x_axis_value, float y_axis_value,
    float hscroll_value, float vscroll_value,
    float relative_x_value, float relative_y_value)
{
    return make_event(device_id, timestamp, std::vector<uint8_t>{}, modifiers, action,
                      buttons_pressed, x_axis_value, y_axis_value, hscroll_value,
                      vscroll_value, relative_x_value, relative_y_value);
}

// Deprecated version without relative axis
mir::EventUPtr mev::make_event(MirInputDeviceId device_id, std::chrono::nanoseconds timestamp,
    uint64_t /*mac*/, MirInputEventModifiers modifiers, MirPointerAction action,
    MirPointerButtons buttons_pressed,                               
    float x_axis_value, float y_axis_value,
    float hscroll_value, float vscroll_value)
{
    return make_event(device_id, timestamp, std::vector<uint8_t>{}, modifiers, action, buttons_pressed,
                      x_axis_value, y_axis_value, hscroll_value, vscroll_value, 0, 0);
}

// Deprecated version without relative axis, and mac
mir::EventUPtr mev::make_event(MirInputDeviceId device_id, std::chrono::nanoseconds timestamp,
    MirInputEventModifiers modifiers, MirPointerAction action,
    MirPointerButtons buttons_pressed,
    float x_axis_value, float y_axis_value,
    float hscroll_value, float vscroll_value)
{
    return make_event(device_id, timestamp, std::vector<uint8_t>{}, modifiers, action, buttons_pressed,
                      x_axis_value, y_axis_value, hscroll_value, vscroll_value, 0, 0);
}

mir::EventUPtr mev::make_event(mf::SurfaceId const& surface_id, MirInputDeviceId id, std::string const& model,
                               std::string const& layout, std::string const& variant, std::string const& options)
{
    auto e = make_empty_event();

    auto ctx = mi::make_unique_context();
    auto map = mi::make_unique_keymap(ctx.get(), model, layout, variant, options);

    if (!map.get())
        BOOST_THROW_EXCEPTION(std::runtime_error("failed to assemble keymap from given parameters"));

    e->type = mir_event_type_keymap;
    e->keymap.surface_id = surface_id.as_value();
    e->keymap.device_id = id;
    // TODO consider caching compiled keymaps
    e->keymap.buffer = xkb_keymap_get_as_string(map.get(), XKB_KEYMAP_FORMAT_TEXT_V1);
    e->keymap.size = strlen(e->keymap.buffer);

    return e;
}

mir::EventUPtr mev::make_event(MirInputConfigurationAction action, MirInputDeviceId id, std::chrono::nanoseconds time)
{
    auto e = make_empty_event();

    e->type = mir_event_type_input_configuration;
    e->input_configuration.action = action;
    e->input_configuration.when = time;
    e->input_configuration.id = id;

    return e;
}
