/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#ifndef __TEXTMODEL_H__
#define __TEXTMODEL_H__

#include <vector>
#include <string>
#include <algorithm>

#include "Paragraph.h"
#include "TextKind.h"
#include "TextMark.h"
#include "RowMemoryAllocator.h"

class Paragraph;
class TreeParagraph;

class TextModel {
	
public:
	enum Kind {
		PLAIN_TEXT_MODEL,
		TREE_MODEL,
	};

	TextModel() MODEL_SECTION;
	virtual ~TextModel() MODEL_SECTION;
	virtual Kind kind() const MODEL_SECTION = 0;

	size_t paragraphsNumber() const MODEL_SECTION;
	Paragraph *operator[] (size_t index) MODEL_SECTION;
	const Paragraph *operator[] (size_t index) const MODEL_SECTION;
	const std::vector<TextMark> &marks() const MODEL_SECTION;

	virtual void search(const std::string &text, size_t startIndex, size_t endIndex, bool ignoreCase) const MODEL_SECTION;
	virtual void selectParagraph(size_t index) const MODEL_SECTION;

	TextMark firstMark() const MODEL_SECTION;
	TextMark lastMark() const MODEL_SECTION;
	TextMark nextMark(TextMark position) const MODEL_SECTION;
	TextMark previousMark(TextMark position) const MODEL_SECTION;

	void addControl(TextKind textKind, bool isStart) MODEL_SECTION;
	void addControl(const ForcedControlEntry &entry) MODEL_SECTION;
	void addHyperlinkControl(TextKind textKind, const std::string &label) MODEL_SECTION;
	void addText(const std::string &text) MODEL_SECTION;
	void addText(const std::vector<std::string> &text) MODEL_SECTION;
	void addImage(const std::string &id, const ImageMap &imageMap) MODEL_SECTION;

protected:
	void addParagraphInternal(Paragraph *paragraph) MODEL_SECTION;
	
private:
	std::vector<Paragraph*> myParagraphs;
	mutable std::vector<TextMark> myMarks;
	mutable RowMemoryAllocator myAllocator;
};

class PlainTextModel : public TextModel {

public:
	Kind kind() const MODEL_SECTION;
	void createParagraph(Paragraph::Kind kind) MODEL_SECTION;
	void createParagraphWithReference(long reference) MODEL_SECTION;
};

class TreeModel : public TextModel {

public:
	TreeModel() MODEL_SECTION;
	~TreeModel() MODEL_SECTION;
	Kind kind() const MODEL_SECTION;

	TreeParagraph *createParagraph(TreeParagraph *parent = 0) MODEL_SECTION;
	
	void search(const std::string &text, size_t startIndex, size_t endIndex, bool ignoreCase) const MODEL_SECTION;
	void selectParagraph(size_t index) const MODEL_SECTION;

private:
	TreeParagraph *myRoot;
};

inline size_t TextModel::paragraphsNumber() const { return myParagraphs.size(); }
inline const std::vector<TextMark> &TextModel::marks() const { return myMarks; }

inline Paragraph *TextModel::operator[] (size_t index) {
	return myParagraphs[std::min(myParagraphs.size() - 1, index)];
}

inline const Paragraph *TextModel::operator[] (size_t index) const {
	return myParagraphs[std::min(myParagraphs.size() - 1, index)];
}

inline TextModel::Kind PlainTextModel::kind() const { return PLAIN_TEXT_MODEL; }

inline TextModel::Kind TreeModel::kind() const { return TREE_MODEL; }

#endif /* __TEXTMODEL_H__ */
