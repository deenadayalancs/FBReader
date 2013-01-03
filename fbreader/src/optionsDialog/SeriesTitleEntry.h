/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __SERIESTITLEENTRY_H__
#define __SERIESTITLEENTRY_H__

#include <set>
#include <vector>
#include <string>

#include <ZLOptionEntry.h>

class OptionsDialog;

class SeriesTitleEntry : public ZLComboOptionEntry {

public:
	SeriesTitleEntry(OptionsDialog &dialog);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

	bool useOnValueEdited() const;
	void onValueEdited(const std::string &value);
	void onValueSelected(int index);

private:
	OptionsDialog &myInfoDialog;
	std::set<std::string> myOriginalValuesSet;
	mutable std::vector<std::string> myValues;
};

#endif /* __SERIESTITLEENTRY_H__ */
