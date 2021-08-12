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
#include "mii_image_database.hpp"

#define AMS_MII_I_DATABASE_SERVICE_INFO(C, H)                                                              \
    AMS_SF_METHOD_INFO(C, H,  0,   Result, Initialize,       (bool flag, sf::Out<bool> out_flag), (flag, out_flag))  \
    AMS_SF_METHOD_INFO(C, H,  10,  Result, Reload,           (), ()) \
    AMS_SF_METHOD_INFO(C, H,  11,  Result, GetCount,         (sf::Out<u32> out_count), (out_count)) \
    AMS_SF_METHOD_INFO(C, H,  12,  Result, IsEmpty,          (sf::Out<bool> out_empty), (out_empty)) \
    AMS_SF_METHOD_INFO(C, H,  13,  Result, IsFull,           (sf::Out<bool> out_full), (out_full)) \
    AMS_SF_METHOD_INFO(C, H,  14,  Result, GetAttribute,     (int index, sf::Out<ams::mii::impl::ImageDatabaseAttribute> out_attr), (index, out_attr)) \
    AMS_SF_METHOD_INFO(C, H,  15,  Result, LoadImage,        (const util::Uuid &image_id, const sf::OutBuffer &out_img_buf), (image_id, out_img_buf)) \
    AMS_SF_METHOD_INFO(C, H,  16,  Result, AddOrUpdateImage, (const ams::mii::impl::ImageDatabaseAttribute &attr, sf::Out<u32> out_32, const sf::InBuffer &buf_1, const sf::InBuffer &buf_2), (attr, out_32, buf_1, buf_2)) \
    AMS_SF_METHOD_INFO(C, H,  17,  Result, DeleteImages,     (const sf::InBuffer &buf), (buf)) \
    AMS_SF_METHOD_INFO(C, H,  100, Result, DeleteFile,       (), ()) \
    AMS_SF_METHOD_INFO(C, H,  101, Result, DestroyFile,      (), ()) \
    AMS_SF_METHOD_INFO(C, H,  102, Result, ImportFile,       (const sf::InBuffer &file_buf), (file_buf)) \
    AMS_SF_METHOD_INFO(C, H,  103, Result, ExportFile,       (sf::Out<u64> out_size, const sf::OutBuffer &out_file_buf), (out_size, out_file_buf)) \
    AMS_SF_METHOD_INFO(C, H,  104, Result, ForceInitialize,  (bool flag, sf::Out<bool> out_flag), (flag, out_flag))

AMS_SF_DEFINE_INTERFACE(ams::mii::impl, IImageDatabaseService, AMS_MII_I_DATABASE_SERVICE_INFO)

namespace ams::mii::impl {

    class ImageDatabaseService {
        private:
            impl::ImageDatabase *img_db;
        public:
            ImageDatabaseService(impl::ImageDatabase *img_db) : img_db(img_db) {}

            Result Initialize(bool flag, sf::Out<bool> out_flag);
            Result Reload();
            Result GetCount(sf::Out<u32> out_count);
            Result IsEmpty(sf::Out<bool> out_empty);
            Result IsFull(sf::Out<bool> out_full);
            Result GetAttribute(int index, sf::Out<ImageDatabaseAttribute> out_attr);
            Result LoadImage(const util::Uuid &image_id, const sf::OutBuffer &out_img_buf);
            Result AddOrUpdateImage(const ImageDatabaseAttribute &attr, sf::Out<u32> out_32, const sf::InBuffer &buf_1, const sf::InBuffer &buf_2);
            Result DeleteImages(const sf::InBuffer &buf);
            Result DeleteFile();
            Result DestroyFile();
            Result ImportFile(const sf::InBuffer &file_buf);
            Result ExportFile(sf::Out<u64> out_size, const sf::OutBuffer &out_file_buf);
            Result ForceInitialize(bool flag, sf::Out<bool> out_flag);
    };
    static_assert(IsIImageDatabaseService<ImageDatabaseService>);

}