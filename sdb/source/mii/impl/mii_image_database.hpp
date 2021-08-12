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
#pragma once
#include <stratosphere.hpp>

namespace ams::mii::impl {

    constexpr u32 EntryCountMax = 9;
    constexpr u32 InvalidCount = UINT32_MAX;

    constexpr u8 InvalidImageDataIndex = UINT8_MAX;

    struct CreateId {
        util::Uuid id;

        bool IsValid() const {
            if(this->IsNil()) {
                return false;
            }
            return (this->id.data[8] & 0xC0) == 0x80;
        }

        bool IsNil() const {
            return this->id == util::InvalidUuid;
        }

        friend bool operator==(const CreateId &lhs, const CreateId &rhs) {
            return lhs.IsValid() && rhs.IsValid() && (lhs.id == rhs.id);
        }

        friend bool operator!=(const CreateId &lhs, const CreateId &rhs) {
            return !(lhs == rhs);
        }
    };

    struct ImageDatabaseEntry {
        u8 unk_1;
        u8 image_data_index;
        u8 unk_3;
        u8 unk_4;
        util::Uuid image_id;
        CreateId create_id;
        char16_t name[10];
    };
    static_assert(sizeof(ImageDatabaseEntry) == 0x38);

    struct ImageDatabaseHeader {
        ImageDatabaseEntry entries[EntryCountMax];
        u32 entry_count;
    };
    static_assert(sizeof(ImageDatabaseHeader) == 0x1FC);

    struct ImageDatabaseAttribute {
        util::Uuid image_id;
        impl::CreateId create_id;
        u8 unk_1;
        u8 unk_2;
        u8 unk_3;
        u8 unk_4;
        char16_t name[11];
    };
    static_assert(sizeof(ImageDatabaseAttribute) == 0x3A);

    constexpr size_t ImageDataSize = 0x40000;
    constexpr size_t ImageDatabaseSize = sizeof(ImageDatabaseHeader) + ImageDataSize * EntryCountMax;

    class ImageDatabase {
        private:
            ImageDatabaseHeader db_header;

            Result DoInitialize(bool *out_flag, bool flag);
            Result WriteToFile();
            Result ForceWriteToFile();
            u32 ComputeEntryCountMax();
        public:
            ImageDatabase();

            Result Initialize(bool *out_flag, bool flag);
            bool IsInitialized();
            Result ForceInitialize(bool *out_flag, bool flag);
            Result Reload();

            inline u32 GetCount() {
                return this->db_header.entry_count;
            }

            inline bool IsEmpty() {
                return this->db_header.entry_count == 0;
            }

            inline bool IsFull() {
                return this->db_header.entry_count == EntryCountMax;
            }

            void GetAttribute(ImageDatabaseAttribute *out_attr, int index);
            Result LoadImage(u8 *out_img_buf, size_t out_img_buf_size, const util::Uuid &image_id);
    };

}