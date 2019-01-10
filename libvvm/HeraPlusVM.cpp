// Copyright 2018 cpp-vapory Authors.
// Licensed under the GNU General Public License v3. See the LICENSE file.

#include "HeraPlusVM.h"
#include "VM.h"
#include <hera.h>

namespace dev
{
namespace vap
{
HeraPlusVM::HeraPlusVM() : VVM(hera_create()) {}

owning_bytes_ref HeraPlusVM::exec(u256& io_gas, ExtVMFace& _ext, OnOpFunc const& _onOp)
{
    // FIXME: This implementation copies most of the code from VVMC::exec().

    auto gas = static_cast<int64_t>(io_gas);
    auto r = execute(_ext, gas);

    if (r.status() == VVM_UNSUPPORTED_CODE_TYPE)
        return VM{}.exec(io_gas, _ext, _onOp);

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
}
}