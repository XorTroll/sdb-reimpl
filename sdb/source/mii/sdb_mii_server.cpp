#include "sdb_mii_server.hpp"
#include "impl/mii_image_db_service.hpp"

using namespace ams;

namespace {

    ams::Result MountImageSaveData(bool create_if_not_found) {
        R_SUCCEED_IF(R_SUCCEEDED(fs::MountSystemSaveData("MiiImage", 0x8000000000001050)));
        if (!create_if_not_found) {
            return 0x87E;
        }

        /* R_TRY(fs::CreateSystemSaveData(0x8000000000001050, 0x0100000000001009, 0x24C000, 311296, 4)); */
        R_TRY(fs::MountSystemSaveData("MiiImage", 0x8000000000001050));
        return ResultSuccess();
    }

    enum PortIndex {
        PortIndex_MiiSystemService,
        PortIndex_MiiUserService,
        PortIndex_MiiImageService,
        PortIndex_Count,
    };

    /* TODO: these domain/domain object amounts work fine, but which ones does N actually use? */
    struct ServerOptions {
        static constexpr size_t PointerBufferSize = 0x400;
        static constexpr size_t MaxDomains = 0x40;
        static constexpr size_t MaxDomainObjects = 0x200;
    };

    constexpr sm::ServiceName MiiSystemServiceName = sm::ServiceName::Encode("mii:e");
    constexpr size_t MiiSystemServiceMaxSessions = 5;

    constexpr sm::ServiceName MiiUserServiceName = sm::ServiceName::Encode("mii:u");
    constexpr size_t MiiUserServiceMaxSessions = 5;

    constexpr sm::ServiceName MiiImageServiceName = sm::ServiceName::Encode("miiimg");
    constexpr size_t MiiImageServiceMaxSessions = 5;

    constexpr size_t MaxSessions = MiiSystemServiceMaxSessions + MiiUserServiceMaxSessions + MiiImageServiceMaxSessions;

    class ServerManager final : public sf::hipc::ServerManager<PortIndex_Count, ServerOptions, MaxSessions> {
        private:
            /* Database db; */
            mii::impl::ImageDatabase img_db;
            u8 heap_data[0x4000];
            lmem::HeapHandle exp_heap;
            sf::SharedPointer<mii::impl::IImageDatabaseService> image_db_session_object;
        private:
            virtual ams::Result OnNeedsToAccept(int port_index, Server *server) override;
        public:
            void Initialize();
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
            */
            case PortIndex_MiiImageService:
                return this->AcceptImpl(server, this->image_db_session_object);
            AMS_UNREACHABLE_DEFAULT_CASE();
        }
    }

    void ServerManager::Initialize() {
        this->image_db_session_object = ObjectFactory::CreateSharedEmplaced<mii::impl::IImageDatabaseService, mii::impl::ImageDatabaseService>(std::addressof(g_server_allocator), std::addressof(this->img_db));
        this->exp_heap = lmem::CreateExpHeap(this->heap_data, sizeof(this->heap_data), lmem::CreateOption_None);
    }

}

namespace ams::mii {

    void StartAndLoopProcess() {
        /* Initialize memory. */
        g_server_heap_handle = lmem::CreateExpHeap(g_server_allocator_buffer, sizeof(g_server_allocator_buffer), lmem::CreateOption_None);
        g_server_allocator.Attach(g_server_heap_handle);

        /* Mount image savedata. */
        R_ABORT_UNLESS(MountImageSaveData(true));

        /* Initialize server manager. */
        g_server_manager.Initialize();

        /* Register mii:e. */
        R_ABORT_UNLESS(g_server_manager.RegisterServer(PortIndex_MiiSystemService, MiiSystemServiceName, MiiSystemServiceMaxSessions));

        /* Register mii:u. */
        R_ABORT_UNLESS(g_server_manager.RegisterServer(PortIndex_MiiUserService, MiiUserServiceName, MiiUserServiceMaxSessions));

        /* Register miiimg. */
        R_ABORT_UNLESS(g_server_manager.RegisterServer(PortIndex_MiiImageService, MiiImageServiceName, MiiImageServiceMaxSessions));

        /* Loop forever, servicing our services. */
        g_server_manager.LoopProcess();
    }

}