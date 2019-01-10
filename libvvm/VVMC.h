// Copyright 2018 cpp-vapory Authors.
// Licensed under the GNU General Public License v3. See the LICENSE file.

#pragma once

#include <vvm.h>
#include <libvvm/VMFace.h>

namespace dev
{
namespace vap
{
/// Translate the VVMSchedule to VVM-C revision.
vvm_revision toRevision(VVMSchedule const& _schedule);

/// The RAII wrapper for an VVM-C instance.
class VVM
{
public:
    explicit VVM(vvm_instance* _instance) noexcept;

    ~VVM() { m_instance->destroy(m_instance); }

    VVM(VVM const&) = delete;
    VVM& operator=(VVM) = delete;

    class Result
    {
    public:
        explicit Result(vvm_result const& _result):
            m_result(_result)
        {}

        ~Result()
        {
            if (m_result.release)
                m_result.release(&m_result);
        }

        Result(Result&& _other) noexcept:
            m_result(_other.m_result)
        {
            // Disable releaser of the rvalue object.
            _other.m_result.release = nullptr;
        }

        Result(Result const&) = delete;
        Result& operator=(Result const&) = delete;

        vvm_status_code status() const
        {
            return m_result.status_code;
        }

        int64_t gasLeft() const
        {
            return m_result.gas_left;
        }

        bytesConstRef output() const
        {
            return {m_result.output_data, m_result.output_size};
        }

    private:
        vvm_result m_result;
    };

    /// Handy wrapper for vvm_execute().
    Result execute(ExtVMFace& _ext, int64_t gas)
    {
        auto mode = toRevision(_ext.vvmSchedule());
        uint32_t flags = _ext.staticCall ? VVM_STATIC : 0;
        vvm_message msg = {toVvmC(_ext.myAddress), toVvmC(_ext.caller),
                           toVvmC(_ext.value), _ext.data.data(),
                           _ext.data.size(), toVvmC(_ext.codeHash), gas,
                           static_cast<int32_t>(_ext.depth), VVM_CALL, flags};
        return Result{m_instance->execute(
            m_instance, &_ext, mode, &msg, _ext.code.data(), _ext.code.size()
        )};
    }

    bool isCodeReady(vvm_revision _mode, uint32_t _flags, h256 _codeHash)
    {
        return m_instance->get_code_status(m_instance, _mode, _flags, toVvmC(_codeHash)) == VVM_READY;
    }

    void compile(vvm_revision _mode, uint32_t _flags, bytesConstRef _code, h256 _codeHash)
    {
        m_instance->prepare_code(
            m_instance, _mode, _flags, toVvmC(_codeHash), _code.data(), _code.size()
        );
    }

private:
    /// The VM instance created with VVM-C <prefix>_create() function.
    vvm_instance* m_instance = nullptr;
};


/// The wrapper implementing the VMFace interface with a VVM-C VM as a backend.
class VVMC : public VVM, public VMFace
{
public:
    explicit VVMC(vvm_instance* _instance) : VVM(_instance) {}

    owning_bytes_ref exec(u256& io_gas, ExtVMFace& _ext, OnOpFunc const& _onOp) final;
};
}
}
