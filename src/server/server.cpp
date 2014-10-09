/*
 * Copyright © 2014 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored By: Alan Griffiths <alan@octopull.co.uk>
 */

#include "mir/server.h"

#include "mir/options/default_configuration.h"
#include "mir/default_server_configuration.h"
#include "mir/main_loop.h"
#include "mir/report_exception.h"
#include "mir/run_mir.h"

#include <iostream>

namespace mo = mir::options;

#define MIR_SERVER_COPY_BUILDER(name)\
std::function<decltype(static_cast<mir::DefaultServerConfiguration*>(nullptr)->the_##name())()> name##_builder;\
\
auto the_##name()\
-> decltype(mir::DefaultServerConfiguration::the_##name()) override\
{\
    if (name##_builder)\
        return name(\
            [this] { return name##_builder(); });\
\
    return mir::DefaultServerConfiguration::the_##name();\
}

struct mir::Server::DefaultServerConfiguration : mir::DefaultServerConfiguration
{
    using mir::DefaultServerConfiguration::DefaultServerConfiguration;
    using mir::DefaultServerConfiguration::the_options;

    std::function<std::shared_ptr<scene::PlacementStrategy>()> placement_strategy_builder;

    auto the_placement_strategy()
    -> std::shared_ptr<scene::PlacementStrategy> override
    {
        if (placement_strategy_builder)
            return shell_placement_strategy(
                [this] { return placement_strategy_builder(); });

        return mir::DefaultServerConfiguration::the_placement_strategy();
    }

//    MIR_SERVER_COPY_BUILDER(placement_strategy)
    MIR_SERVER_COPY_BUILDER(session_listener);
    MIR_SERVER_COPY_BUILDER(prompt_session_listener);
    MIR_SERVER_COPY_BUILDER(surface_configurator);
    MIR_SERVER_COPY_BUILDER(session_authorizer);
    MIR_SERVER_COPY_BUILDER(compositor);
    MIR_SERVER_COPY_BUILDER(input_dispatcher);
    MIR_SERVER_COPY_BUILDER(gl_config);
    MIR_SERVER_COPY_BUILDER(server_status_listener);
//    MIR_SERVER_COPY_BUILDER(focus_setter);
//    MIR_SERVER_COPY_BUILDER(placement_cursor_listener);
};

#undef MIR_SERVER_COPY_BUILDER

struct mir::Server::BuildersAndWrappers
{
    std::function<std::shared_ptr<scene::PlacementStrategy>()> placement_strategy_builder;
    std::function<std::shared_ptr<scene::SessionListener>()> session_listener_builder;
    std::function<std::shared_ptr<scene::PromptSessionListener>()> prompt_session_listener_builder;
    std::function<std::shared_ptr<scene::SurfaceConfigurator>()> surface_configurator_builder;
    std::function<std::shared_ptr<frontend::SessionAuthorizer>()> session_authorizer_builder;
    std::function<std::shared_ptr<compositor::Compositor>()> compositor_builder;
    std::function<std::shared_ptr<input::InputDispatcher>()> input_dispatcher_builder;
    std::function<std::shared_ptr<graphics::GLConfig>()> gl_config_builder;
    std::function<std::shared_ptr<ServerStatusListener>()> server_status_listener_builder;
    std::function<std::shared_ptr<shell::FocusSetter>()> focus_setter_builder;
    std::function<std::shared_ptr<scene::PlacementStrategy>()> placement_cursor_listener_builder;
};

namespace
{
std::shared_ptr<mo::DefaultConfiguration> configuration_options(
    int argc,
    char const** argv,
    std::function<void(int argc, char const* const* argv)> const& command_line_hander)
{
    if (command_line_hander)
        return std::make_shared<mo::DefaultConfiguration>(argc, argv, command_line_hander);
    else
        return std::make_shared<mo::DefaultConfiguration>(argc, argv);

}
}

mir::Server::Server() :
    builders_and_wrappers(std::make_shared<BuildersAndWrappers>())
{
}

void mir::Server::set_add_configuration_options(
    std::function<void(mo::DefaultConfiguration& config)> const& add_configuration_options)
{
    this->add_configuration_options = add_configuration_options;
}


void mir::Server::set_command_line(int argc, char const* argv[])
{
    this->argc = argc;
    this->argv = argv;
}

void mir::Server::set_init_callback(std::function<void()> const& init_callback)
{
    this->init_callback = init_callback;
}

auto mir::Server::get_options() const -> std::shared_ptr<options::Option>
{
    return options.lock();
}

void mir::Server::set_exception_handler(std::function<void()> const& exception_handler)
{
    this->exception_handler = exception_handler;
}

#define MIR_SERVER_COPY_BUILDER(name)\
    config.name##_builder = builders_and_wrappers->name##_builder

void mir::Server::run()
try
{
    auto const options = configuration_options(argc, argv, command_line_hander);

    add_configuration_options(*options);

    DefaultServerConfiguration config{options};

    MIR_SERVER_COPY_BUILDER(placement_strategy);
    MIR_SERVER_COPY_BUILDER(session_listener);
    MIR_SERVER_COPY_BUILDER(prompt_session_listener);
    MIR_SERVER_COPY_BUILDER(surface_configurator);
    MIR_SERVER_COPY_BUILDER(session_authorizer);
    MIR_SERVER_COPY_BUILDER(compositor);
    MIR_SERVER_COPY_BUILDER(input_dispatcher);
    MIR_SERVER_COPY_BUILDER(gl_config);
    MIR_SERVER_COPY_BUILDER(server_status_listener);
//    MIR_SERVER_COPY_BUILDER(focus_setter);
//    MIR_SERVER_COPY_BUILDER(placement_cursor_listener);

    server_config = &config;

    run_mir(config, [&](DisplayServer&)
        {
            this->options = config.the_options();
            init_callback();
        });

    exit_status = true;
    server_config = nullptr;
}
catch (...)
{
    server_config = nullptr;

    if (exception_handler)
        exception_handler();
    else
        mir::report_exception(std::cerr);
}

void mir::Server::stop()
{
    std::cerr << "DEBUG: " << __PRETTY_FUNCTION__ << std::endl;
    if (auto const main_loop = the_main_loop())
        main_loop->stop();
}

bool mir::Server::exited_normally()
{
    return exit_status;
}

namespace
{
auto const no_config_to_access = "Cannot access config when no config active.";
}

#define MIR_SERVER_ACCESSOR(name)\
auto mir::Server::name() const -> decltype(server_config->name())\
{\
    if (server_config) return server_config->name();\
    BOOST_THROW_EXCEPTION(std::logic_error(no_config_to_access));\
}

MIR_SERVER_ACCESSOR(the_graphics_platform)
MIR_SERVER_ACCESSOR(the_display)
MIR_SERVER_ACCESSOR(the_main_loop)
MIR_SERVER_ACCESSOR(the_composite_event_filter)
MIR_SERVER_ACCESSOR(the_shell_display_layout)
MIR_SERVER_ACCESSOR(the_session_authorizer)
MIR_SERVER_ACCESSOR(the_session_listener)
MIR_SERVER_ACCESSOR(the_prompt_session_listener)
MIR_SERVER_ACCESSOR(the_surface_configurator)

#undef MIR_SERVER_ACCESSOR

#define MIR_SERVER_WRAPPER(name)\
void mir::Server::override_the_##name(decltype(BuildersAndWrappers::name##_builder) const& value)\
{\
    builders_and_wrappers->name##_builder = value;\
}

MIR_SERVER_WRAPPER(placement_strategy)
//MIR_SERVER_WRAPPER(session_listener)
//MIR_SERVER_WRAPPER(prompt_session_listener)
//MIR_SERVER_WRAPPER(surface_configurator)
//MIR_SERVER_WRAPPER(session_authorizer)
//MIR_SERVER_WRAPPER(compositor)
//MIR_SERVER_WRAPPER(cursor_listener)
//MIR_SERVER_WRAPPER(input_dispatcher)
//MIR_SERVER_WRAPPER(gl_config)
//MIR_SERVER_WRAPPER(server_status_listener)
//MIR_SERVER_WRAPPER(shell_focus_setter)
//MIR_SERVER_WRAPPER(cursor_listener)

#undef MIR_SERVER_WRAPPER
