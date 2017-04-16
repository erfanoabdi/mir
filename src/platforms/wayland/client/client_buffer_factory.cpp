/*
 * Copyright © 2017 The Ubports project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by:
 *   Marius Gripsgard <marius@ubports.com>
 */

#include "client_buffer_factory.h"
#include "client_buffer.h"
#include <boost/throw_exception.hpp>
#include <stdexcept>

namespace mcl=mir::client;
namespace mclw=mir::client::wayland;

std::shared_ptr<mcl::ClientBuffer>
mclw::ClientBufferFactory::create_buffer(
    std::shared_ptr<MirBufferPackage> const& package,
    geometry::Size /*size*/,
    MirPixelFormat pf)
{
    return std::make_shared<mclw::ClientBuffer>(
        package,
        geometry::Size{package->width, package->height},
        pf);
}

std::shared_ptr<mcl::ClientBuffer>
mclw::ClientBufferFactory::create_buffer(
    std::shared_ptr<MirBufferPackage> const&,
    unsigned int, unsigned int)
{
    BOOST_THROW_EXCEPTION(std::runtime_error("no native buffers on wayland platform"));
}
