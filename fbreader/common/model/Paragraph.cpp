/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "Paragraph.h"

Paragraph::~Paragraph() {
	for (std::vector<ParagraphEntry*>::const_iterator it = myEntries.begin(); it != myEntries.end(); it++) {
		delete *it;
	}
}

void Paragraph::addNonConstText(std::string &text) {
	if (myEntries.empty() || (myEntries.back()->entryKind() != ParagraphEntry::TEXT_ENTRY)) {
		myEntries.push_back(new TextEntry(text));
	} else {
		((TextEntry*)myEntries.back())->addText(text);
	}
}

void Paragraph::addText(const std::string &text) {
	if (myEntries.empty() || (myEntries.back()->entryKind() != ParagraphEntry::TEXT_ENTRY)) {
		myEntries.push_back(new TextEntry());
	}
	((TextEntry*)myEntries.back())->addText(text);
}

void Paragraph::addText(const std::vector<std::string> &text) {
	if (myEntries.empty() || (myEntries.back()->entryKind() != ParagraphEntry::TEXT_ENTRY)) {
		myEntries.push_back(new TextEntry());
	}
	((TextEntry*)myEntries.back())->addText(text);
}

void TextEntry::addText(const std::string &text) {
	myText += text;
}

const Image *ImageEntry::image() const {
	ImageMap::const_iterator i = myMap.find(myId);
	return (i != myMap.end()) ? (*i).second : 0;
}

TreeParagraph::TreeParagraph(TreeParagraph *parent) : Paragraph(TREE_PARAGRAPH), myIsOpen(false), myParent(parent) {
	if (parent != 0) {
		parent->addChild(this);
		myDepth = parent->myDepth + 1;
	} else {
		myDepth = 0;
	}
}

void TreeParagraph::openTree() {
	for (TreeParagraph *p = parent(); p != 0; p = p->parent()) {
		p->open(true);
	}
}
