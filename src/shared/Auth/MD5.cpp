/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "MD5.h"

MD5Hash::MD5Hash()
{
	md5_init(&mC);
}

MD5Hash::~MD5Hash()
{

}

void MD5Hash::UpdateData(const std::string &str)
{
	UpdateData((const uint8*)str.data(), (int)str.length());
}

void MD5Hash::UpdateData(const uint8 *dta, int len)
{
	md5_append(&mC, dta, len);
}

void MD5Hash::Finalize()
{
	md5_finish(&mC, mDigest);
}

void MD5Hash::Initialize()
{
	md5_init(&mC);
}