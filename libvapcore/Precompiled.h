/*
	This file is part of cpp-vapory.

	cpp-vapory is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-vapory is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-vapory.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file Precompiled.h
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#pragma once

#include <unordered_map>
#include <functional>
#include <libdevcore/CommonData.h>
#include <libdevcore/Exceptions.h>

namespace dev
{
namespace vap
{

using PrecompiledExecutor = std::function<std::pair<bool, bytes>(bytesConstRef _in)>;
using PrecompiledPricer = std::function<bigint(bytesConstRef _in)>;

DEV_SIMPLE_EXCEPTION(ExecutorNotFound);
DEV_SIMPLE_EXCEPTION(PricerNotFound);

class PrecompiledRegistrar
{
public:
	/// Get the executor object for @a _name function or @throw ExecutorNotFound if not found.
	static PrecompiledExecutor const& executor(std::string const& _name);

	/// Get the price calculator object for @a _name function or @throw PricerNotFound if not found.
	static PrecompiledPricer const& pricer(std::string const& _name);

	/// Register an executor. In general just use VAP_REGISTER_PRECOMPILED.
	static PrecompiledExecutor registerExecutor(std::string const& _name, PrecompiledExecutor const& _exec) { return (get()->m_execs[_name] = _exec); }
	/// Unregister an executor. Shouldn't generally be necessary.
	static void unregisterExecutor(std::string const& _name) { get()->m_execs.erase(_name); }

	/// Register a pricer. In general just use VAP_REGISTER_PRECOMPILED_PRICER.
	static PrecompiledPricer registerPricer(std::string const& _name, PrecompiledPricer const& _exec) { return (get()->m_pricers[_name] = _exec); }
	/// Unregister a pricer. Shouldn't generally be necessary.
	static void unregisterPricer(std::string const& _name) { get()->m_pricers.erase(_name); }

private:
	static PrecompiledRegistrar* get() { if (!s_this) s_this = new PrecompiledRegistrar; return s_this; }

	std::unordered_map<std::string, PrecompiledExecutor> m_execs;
	std::unordered_map<std::string, PrecompiledPricer> m_pricers;
	static PrecompiledRegistrar* s_this;
};

// TODO: unregister on unload with a static object.
#define VAP_REGISTER_PRECOMPILED(Name) static std::pair<bool, bytes> __vap_registerPrecompiledFunction ## Name(bytesConstRef _in); static PrecompiledExecutor __vap_registerPrecompiledFactory ## Name = ::dev::vap::PrecompiledRegistrar::registerExecutor(#Name, &__vap_registerPrecompiledFunction ## Name); static std::pair<bool, bytes> __vap_registerPrecompiledFunction ## Name
#define VAP_REGISTER_PRECOMPILED_PRICER(Name) static bigint __vap_registerPricerFunction ## Name(bytesConstRef _in); static PrecompiledPricer __vap_registerPricerFactory ## Name = ::dev::vap::PrecompiledRegistrar::registerPricer(#Name, &__vap_registerPricerFunction ## Name); static bigint __vap_registerPricerFunction ## Name
}
}
