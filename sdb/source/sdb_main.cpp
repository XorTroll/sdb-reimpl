/*
 * Copyright (c) 2018-2020 Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "pl/sdb_pl_server.hpp"
#include "pdm/sdb_pdm_server.hpp"
#include "avm/sdb_avm_server.hpp"
#include "mii/sdb_mii_server.hpp"

extern "C" {
    extern u32 __start__;

    u32 __nx_applet_type = AppletType_None;
    u32 __nx_fs_num_sessions = 1;
    
    #define INNER_HEAP_SIZE 0x0
    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char nx_inner_heap[INNER_HEAP_SIZE];

    void __libnx_initheap(void);
    void __appInit(void);
    void __appExit(void);

    /* Exception handling. */
    alignas(16) u8 __nx_exception_stack[ams::os::MemoryPageSize];
    u64 __nx_exception_stack_size = sizeof(__nx_exception_stack);
    void __libnx_exception_handler(ThreadExceptionDump *ctx);

    void *__libnx_alloc(size_t size);
    void *__libnx_aligned_alloc(size_t alignment, size_t size);
    void __libnx_free(void *mem);
}

namespace ams {

    ncm::ProgramId CurrentProgramId = ncm::SystemProgramId::Sdb;

    namespace result {

        bool CallFatalOnResultAssertion = true;

    }

}

using namespace ams;

namespace {

    /* pdm */

    os::ThreadType g_pdm_server_thread;
    alignas(os::ThreadStackAlignment) u8 g_pdm_server_thread_stack[0x3000];

    void PdmServerThread(void*) {
        /* Run pdm's server routine. */
        pdm::StartAndLoopProcess();
    }

    /* avm */

    os::ThreadType g_avm_server_thread;
    alignas(os::ThreadStackAlignment) u8 g_avm_server_thread_stack[0x4000];

    void AvmServerThread(void*) {
        /* Run avm's server routine. */
        avm::StartAndLoopProcess();
    }

    /* mii */

    os::ThreadType g_mii_server_thread;
    alignas(os::ThreadStackAlignment) u8 g_mii_server_thread_stack[0x3000];

    void MiiServerThread(void*) {
        /* Run mii's server routine. */
        mii::StartAndLoopProcess();
    }

}

void __libnx_exception_handler(ThreadExceptionDump *ctx) {
    ams::CrashHandler(ctx);
}

void __libnx_initheap(void) {
	void*  addr = nx_inner_heap;
	size_t size = nx_inner_heap_size;

	/* Newlib */
	extern char* fake_heap_start;
	extern char* fake_heap_end;

	fake_heap_start = (char*)addr;
	fake_heap_end   = (char*)addr + size;
}

void __appInit(void) {
    hos::InitializeForStratosphere();

    /* Initialize services. */
    R_ABORT_UNLESS(sm::Initialize());
    R_ABORT_UNLESS(setsysInitialize());
    R_ABORT_UNLESS(fsInitialize());

    ams::CheckApiVersion();
}

void __appExit(void) {
    /* Cleanup services. */
    fsExit();
    setsysExit();
}

namespace ams {

    void *Malloc(size_t size) {
        AMS_ABORT("ams::Malloc was called");
    }

    void Free(void *ptr) {
        AMS_ABORT("ams::Free was called");
    }

}

void *operator new(size_t size) {
    AMS_ABORT("operator new(size_t) was called");
}

void operator delete(void *p) {
    AMS_ABORT("operator delete(void *) was called");
}

void *__libnx_alloc(size_t size) {
    AMS_ABORT("__libnx_alloc was called");
}

void *__libnx_aligned_alloc(size_t alignment, size_t size) {
    AMS_ABORT("__libnx_aligned_alloc was called");
}

void __libnx_free(void *mem) {
    AMS_ABORT("__libnx_free was called");
}

int main(int argc, char **argv) {
    /* Set thread name. */
    os::SetThreadNamePointer(os::GetCurrentThread(), "ams.sdb.MainThread");
    AMS_ASSERT(os::GetThreadPriority(os::GetCurrentThread()) == 21);

    /* Create pdm server thread. */
    R_ABORT_UNLESS((os::CreateThread(std::addressof(g_pdm_server_thread), PdmServerThread, nullptr, g_pdm_server_thread_stack, sizeof(g_pdm_server_thread_stack), 21)));
    os::SetThreadNamePointer(std::addressof(g_pdm_server_thread), "ams.pdmSrv.IpcServer");
    os::StartThread(std::addressof(g_pdm_server_thread));

    /* Create avm server thread. */
    R_ABORT_UNLESS((os::CreateThread(std::addressof(g_avm_server_thread), AvmServerThread, nullptr, g_avm_server_thread_stack, sizeof(g_avm_server_thread_stack), -1)));
    os::SetThreadNamePointer(std::addressof(g_avm_server_thread), "ams.avm.IpcServer");
    os::StartThread(std::addressof(g_avm_server_thread));

    /* Create mii server thread. */
    R_ABORT_UNLESS((os::CreateThread(std::addressof(g_mii_server_thread), MiiServerThread, nullptr, g_mii_server_thread_stack, sizeof(g_mii_server_thread_stack), 21)));
    os::SetThreadNamePointer(std::addressof(g_mii_server_thread), "ams.mii.Main");
    os::StartThread(std::addressof(g_mii_server_thread));

    /* Run pl's server manager in main thread. */
    pl::StartAndLoopProcess();
 
    return 0;
}