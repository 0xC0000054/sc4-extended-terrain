/*
 * This file is part of sc4-extended-terrain, a DLL Plugin for SimCity 4
 * that adds support for more terrain types.
 *
 * Copyright (C) 2024, 2025 Nicholas Hayes
 *
 * sc4-extended-terrain is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * sc4-extended-terrain is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with sc4-extended-terrain.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include "cIGZFile.h"
#include "cRZAutoRefCount.h"
#include <iosfwd>
#include <limits>
#include <streambuf>
#include <vector>
#include <utility>

class GZFileStreamBufAdapter final : public std::streambuf
{
public:
	GZFileStreamBufAdapter(cRZAutoRefCount<cIGZFile>&& file);
	GZFileStreamBufAdapter(const GZFileStreamBufAdapter&) = delete;
	GZFileStreamBufAdapter(GZFileStreamBufAdapter&&) noexcept;

	GZFileStreamBufAdapter& operator=(const GZFileStreamBufAdapter&) = delete;
	GZFileStreamBufAdapter& operator=(GZFileStreamBufAdapter&&) noexcept;

private:
	std::streambuf::int_type underflow() override;

	cRZAutoRefCount<cIGZFile> file;
	std::vector<char> buffer;
};

