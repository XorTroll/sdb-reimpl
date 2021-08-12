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
#include "sdb_pdm_server.hpp"

using namespace ams;

namespace {

    enum PortIndex {
        PortIndex_PdmNotifyService,
        PortIndex_PdmQueryService,
        PortIndex_Count,
    };

    /* TODO: these domain/domain object amounts work fine, but which ones does N actually use? */
    struct ServerOptions {
        static constexpr size_t PointerBufferSize = 0x400;
        static constexpr size_t MaxDomains = 0x40;
        static constexpr size_t MaxDomainObjects = 0x200;
    };

    constexpr sm::ServiceName PdmNotifyServiceName = sm::ServiceName::Encode("pdm:ntfy");
    constexpr size_t          PdmNotifyServiceMaxSessions = 5;

    constexpr sm::ServiceName PdmQueryServiceName = sm::ServiceName::Encode("pdm:qry");
    constexpr size_t          PdmQueryServiceMaxSessions = 6;
    
    static constexpr size_t MaxSessions = PdmNotifyServiceMaxSessions + PdmQueryServiceMaxSessions;
    
    class ServerManager final : public sf::hipc::ServerManager<PortIndex_Count, ServerOptions, MaxSessions> {
        private:
            virtual ams::Result OnNeedsToAccept(int port_index, Server *server) override;
    };

    using Allocator     = sf::ExpHeapAllocator;
    using ObjectFactory = sf::ObjectFactory<sf::ExpHeapAllocator::Policy>;

    alignas(0x40) constinit u8 g_server_allocator_buffer[4_KB];
    lmem::HeapHandle g_server_heap_handle;
    Allocator g_server_allocator;

    ServerManager g_server_manager;

    ams::Result ServerManager::OnNeedsToAccept(int port_index, Server *server) {
        switch (port_index) {
            /*
            case PortIndex_PlSystemService:
                return this->AcceptImpl(server, ObjectFactory::CreateSharedEmplaced<lm::impl::ILogService, lm::LogService>(std::addressof(g_server_allocator)));
            case PortIndex_LogGetter:
                return this->AcceptImpl(server, ObjectFactory::CreateSharedEmplaced<lm::impl::ILogGetter, lm::LogGetter>(std::addressof(g_server_allocator)));
            */
            AMS_UNREACHABLE_DEFAULT_CASE();
        }
    }

}

namespace ams::pdm {

    void StartAndLoopProcess() {
        /* Initialize memory. */
        g_server_heap_handle = lmem::CreateExpHeap(g_server_allocator_buffer, sizeof(g_server_allocator_buffer), lmem::CreateOption_None);
        g_server_allocator.Attach(g_server_heap_handle);

        /* Register pdm:ntfy. */
        /* ... */

        /* Register pdm:qry. */
        /* ... */

        /* Loop forever, servicing our services. */
        g_server_manager.LoopProcess();
    }

}