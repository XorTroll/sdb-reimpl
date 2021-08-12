#include "mii_image_database.hpp"

namespace ams::mii::impl {

    namespace {

        inline void InitializeHeader(ImageDatabaseHeader &header) {
            for(u32 i = 0; i < EntryCountMax; i++) {
                header.entries[i] = {};
                header.entries[i].unk_1 = 0x4;
                header.entries[i].image_data_index = InvalidImageDataIndex;
                header.entries[i].unk_3 = 0x13;
                header.entries[i].unk_4 = 0x15;
            }
            header.entry_count = 0;
        }

    }

    ImageDatabase::ImageDatabase() {
        std::memset(std::addressof(this->db_header), 0, sizeof(this->db_header));
        this->db_header.entry_count = InvalidCount;
    }

    Result ImageDatabase::DoInitialize(bool *out_flag, bool flag) {
        /* TODO: finish */
        fs::FileHandle db_file_h;
        R_TRY_CATCH(fs::OpenFile(std::addressof(db_file_h), "MiiImage:/database.dat", fs::OpenMode_Read)) {
            R_CATCH(fs::ResultPathNotFound) {
                InitializeHeader(this->db_header);
                *out_flag = true;
                return this->WriteToFile();
            }
        } R_END_TRY_CATCH;

        return ResultSuccess();
    }

    Result ImageDatabase::WriteToFile() {
        fs::FileHandle db_file_h;
        R_TRY_CATCH(fs::OpenFile(std::addressof(db_file_h), "MiiImage:/database.dat", fs::OpenMode_Write | fs::OpenMode_AllowAppend)) {
            R_CATCH(fs::ResultPathNotFound) {
                R_TRY(fs::CreateFile("MiiImage:/database.dat", ImageDatabaseSize));
            }
        } R_END_TRY_CATCH;

        R_TRY(fs::OpenFile(std::addressof(db_file_h), "MiiImage:/database.dat", fs::OpenMode_Write | fs::OpenMode_AllowAppend));
        ON_SCOPE_EXIT { fs::CloseFile(db_file_h); };

        R_TRY(fs::WriteFile(db_file_h, 0, std::addressof(this->db_header), sizeof(this->db_header), fs::WriteOption::Flush));
        R_TRY(fs::CommitSaveData("MiiImage"));

        return ResultSuccess();
    }

    Result ImageDatabase::ForceWriteToFile() {
        R_TRY(fs::DeleteFile("MiiImage:/database.dat"));
        R_TRY(this->WriteToFile());

        return ResultSuccess();
    }

    u32 ImageDatabase::ComputeEntryCountMax() {
        for(u32 i = 0; i < EntryCountMax; i++) {
            if(this->db_header.entries[i].image_id == util::InvalidUuid) {
                return i;
            }
        }
        return EntryCountMax;
    }

    Result ImageDatabase::Initialize(bool *out_flag, bool flag) {
        this->db_header.entry_count = InvalidCount;

        R_TRY(this->DoInitialize(out_flag, flag));
        return ResultSuccess();
    }

    Result ImageDatabase::ForceInitialize(bool *out_flag, bool flag) {
        const auto rc = this->DoInitialize(out_flag, flag);
        if(rc.IsFailure()) {
            if(this->db_header.entry_count != InvalidCount) {
                this->db_header.entry_count = InvalidCount;
            }
        }
        return rc;
    }

    bool ImageDatabase::IsInitialized() {
        return this->db_header.entry_count != InvalidCount;
    }

    Result ImageDatabase::Reload() {
        bool out;
        auto rc = this->DoInitialize(std::addressof(out), true);
        AMS_ASSERT(out);
        return rc;
    }

    void ImageDatabase::GetAttribute(ImageDatabaseAttribute *out_attr, int index) {
        out_attr->image_id = this->db_header.entries[index].image_id;
        out_attr->create_id = this->db_header.entries[index].create_id;
        out_attr->unk_1 = this->db_header.entries[index].unk_3;
        out_attr->unk_2 = this->db_header.entries[index].unk_4;
        out_attr->unk_3 = this->db_header.entries[index].unk_1;
        /* Copy name UTF-16. */
    }

    Result ImageDatabase::LoadImage(u8 *out_img_buf, size_t out_img_buf_size, const util::Uuid &image_id) {
        if (this->IsEmpty()) {
            return 0x87E;
        }

        int index = -1;
        for(u32 i = 0; i < EntryCountMax; i++) {
            if(this->db_header.entries[i].image_id == image_id) {
                index = i;
                break;
            }
        }
        if (index == -1) {
            return 0x87E;
        }

        auto img_data_index = this->db_header.entries[index].image_data_index;
        fs::FileHandle db_file_h;
        // R_TRY(fs::OpenFile(std::addressof(db_file_h), "MiiImage:/database.dat", ));
        ON_SCOPE_EXIT { fs::CloseFile(db_file_h); };

        R_TRY(fs::ReadFile(db_file_h, sizeof(this->db_header) + img_data_index * ImageDataSize, out_img_buf, out_img_buf_size));
        return ResultSuccess();
    }

}