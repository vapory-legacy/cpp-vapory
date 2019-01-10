// Copyright 2018 cpp-vapory Authors.
// Licensed under the GNU General Public License v3. See the LICENSE file.

#pragma once

#include "VVMC.h"

namespace dev
{
namespace vap
{

/// The eWASM VM with a fallback to VVM 1.0.
class HeraPlusVM: public VVM, public VMFace
{
public:
    HeraPlusVM();

    owning_bytes_ref exec(u256& io_gas, ExtVMFace& _ext, OnOpFunc const& _onOp) final;
};

}
}
