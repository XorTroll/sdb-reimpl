#include "mii_image_db_service.hpp"

namespace ams::mii::impl {

    namespace {

        inline Result EnsureTestModeEnabled() {
            bool test_mode_enabled = false;
            settings::fwdbg::GetSettingsItemValue(std::addressof(test_mode_enabled), sizeof(test_mode_enabled), "mii", "is_img_db_test_mode_enabled");
            if (!test_mode_enabled) {
                return 0x1987E;
            }
            else {
                return ResultSuccess();
            }
        }

    }

    Result ImageDatabaseService::Initialize(bool flag, sf::Out<bool> out_flag) {
        if(this->img_db->IsInitialized()) {
            out_flag.SetValue(false);
            return ResultSuccess();
        }
        else {
            return this->img_db->Initialize(out_flag.GetPointer(), flag);
        }
    }

    Result ImageDatabaseService::Reload() {
        return this->img_db->Reload();
    }

    Result ImageDatabaseService::GetCount(sf::Out<u32> out_count) {
        out_count.SetValue(this->img_db->GetCount());
        return ResultSuccess();
    }

    Result ImageDatabaseService::IsEmpty(sf::Out<bool> out_empty) {
        out_empty.SetValue(this->img_db->IsEmpty());
        return ResultSuccess();
    }

    Result ImageDatabaseService::IsFull(sf::Out<bool> out_full) {
        out_full.SetValue(this->img_db->IsFull());
        return ResultSuccess();
    }

    Result ImageDatabaseService::GetAttribute(int index, sf::Out<ImageDatabaseAttribute> out_attr) {
        const auto count = this->img_db->GetCount();
        if(((index & 0x80000000) == 0) && (count > index)) {
            this->img_db->GetAttribute(out_attr.GetPointer(), index);
            return ResultSuccess();
        }
        else {
            return 0x27E;
        }
    }

    Result ImageDatabaseService::LoadImage(const util::Uuid &image_id, const sf::OutBuffer &out_img_buf) {
        if(image_id == util::InvalidUuid) {
            return 0x27E;
        }
        else {
            return this->img_db->LoadImage(out_img_buf.GetPointer(), out_img_buf.GetSize(), image_id);
        }
    }

    Result ImageDatabaseService::AddOrUpdateImage(const ImageDatabaseAttribute &attr, sf::Out<u32> out_32, const sf::InBuffer &buf_1, const sf::InBuffer &buf_2) {
        return ResultSuccess();
    }
    
    Result ImageDatabaseService::DeleteImages(const sf::InBuffer &buf) {
        return ResultSuccess();
    }

    Result ImageDatabaseService::DeleteFile() {
        R_TRY(EnsureTestModeEnabled());

        /* DeleteFile */
        return ResultSuccess();
    }

    Result ImageDatabaseService::DestroyFile() {
        R_TRY(EnsureTestModeEnabled());

        /* DestroyFile */
        return ResultSuccess();
    }

    Result ImageDatabaseService::ImportFile(const sf::InBuffer &file_buf) {
        R_TRY(EnsureTestModeEnabled());

        /* ImportFile */
        return ResultSuccess();
    }

    Result ImageDatabaseService::ExportFile(sf::Out<u64> out_size, const sf::OutBuffer &out_file_buf) {
        R_TRY(EnsureTestModeEnabled());

        /* ExportFile */
        return ResultSuccess();
    }

    Result ImageDatabaseService::ForceInitialize(bool flag, sf::Out<bool> out_flag) {
        R_TRY(EnsureTestModeEnabled());

        return this->img_db->ForceInitialize(out_flag.GetPointer(), flag);
    }

}