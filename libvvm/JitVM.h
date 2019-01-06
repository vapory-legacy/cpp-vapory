#pragma once

#include <vvmjit.h>
#include <libvvm/VMFace.h>

namespace dev
{
namespace vap
{

class JitVM: public VMFace
{
public:
	owning_bytes_ref exec(u256& io_gas, ExtVMFace& _ext, OnOpFunc const& _onOp) override final;

	static vvm_revision toRevision(VVMSchedule const& _schedule);
	static bool isCodeReady(vvm_revision _mode, uint32_t _flags, h256 _codeHash);
	static void compile(vvm_revision _mode, uint32_t _flags, bytesConstRef _code, h256 _codeHash);
};


}
}
