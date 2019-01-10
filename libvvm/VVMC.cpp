// Copyright 2018 cpp-vapory Authors.
// Licensed under the GNU General Public License v3. See the LICENSE file.

#include "VVMC.h"

#include <libdevcore/Log.h>
#include <libvvm/VM.h>
#include <libvvm/VMFactory.h>

namespace dev
{
namespace vap
{
VVM::VVM(vvm_instance* _instance) noexcept : m_instance(_instance)
{
    assert(m_instance != nullptr);
    assert(m_instance->abi_version == VVM_ABI_VERSION);

    // Set the options.
    for (auto& pair : vvmcOptions())
        m_instance->set_option(m_instance, pair.first.c_str(), pair.second.c_str());
}

owning_bytes_ref VVMC::exec(u256& io_gas, ExtVMFace& _ext, OnOpFunc const&)
{
    constexpr int64_t int64max = std::numeric_limits<int64_t>::max();

    // TODO: The following checks should be removed by changing the types
    //       used for gas, block number and timestamp.
    (void)int64max;
    assert(io_gas <= int64max);
    assert(_ext.envInfo().number() <= int64max);
    assert(_ext.envInfo().timestamp() <= int64max);
    assert(_ext.envInfo().gasLimit() <= int64max);
    assert(_ext.depth <= std::numeric_limits<int32_t>::max());

    auto gas = static_cast<int64_t>(io_gas);
    VVM::Result r = execute(_ext, gas);

    // TODO: Add VVM-C result codes mapping with exception types.
    if (r.status() == VVM_FAILURE)
        BOOST_THROW_EXCEPTION(OutOfGas());

    io_gas = r.gasLeft();
    // FIXME: Copy the output for now, but copyless version possible.
    owning_bytes_ref output{r.output().toVector(), 0, r.output().size()};

    if (r.status() == VVM_REVERT)
        throw RevertInstruction(std::move(output));

    return output;
}

vvm_revision toRevision(VVMSchedule const& _schedule)
{
	if (_schedule.haveCreate2)
		return VVM_CONSTANTINOPLE;
	if (_schedule.haveRevert)
		return VVM_BYZANTIUM;
	if (_schedule.eip158Mode)
		return VVM_SPURIOUS_DRAGON;
	if (_schedule.eip150Mode)
		return VVM_TANGERINE_WHISTLE;
	if (_schedule.haveDelegateCall)
		return VVM_HOMESTEAD;
	return VVM_FRONTIER;
}

}
}
