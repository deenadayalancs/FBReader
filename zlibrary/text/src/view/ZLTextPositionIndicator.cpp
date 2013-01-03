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

#include <algorithm>

#include <ZLTime.h>
#include <ZLStringUtil.h>
#include <ZLTextStyle.h>

#include "ZLTextPositionIndicator.h"

/*
 * Calculates m0 * m1 / d
 *   We assume m0 <= d or m1 <= d so result is small enough for std::size_t
 */
static std::size_t muldiv(std::size_t m0, std::size_t m1, std::size_t d) {
	static const std::size_t HALF = 1 << sizeof(std::size_t) / 2;
	if ((m0 < HALF) && (m1 < HALF)) {
		return m0 * m1 / d;
	}

	if (m0 > d) {
		std::size_t swap = m0;
		m0 = m1;
		m1 = swap;
	}

	std::size_t result = 0;
	std::size_t remainder = 0;

	while ((m0 != 0) && (m1 != 0) && ((m0 >= HALF) || (m1 >= HALF))) {
		const std::size_t mul = (d - 1) / m0 + 1;
		const std::size_t rem = m0 * (m1 % mul);
		if (remainder + rem < remainder) {
			++result;
		}
		remainder += rem;
		m1 /= mul;
		result += m1;
		m0 = m0 * mul - d;
	}
	const std::size_t product = m0 * m1;
	result += product / d;
	if (remainder + product % d < remainder) {
		++result;
	}
	return result;
}

ZLTextPositionIndicatorInfo::ZLTextPositionIndicatorInfo() {
}

ZLTextPositionIndicatorInfo::~ZLTextPositionIndicatorInfo() {
}

ZLTextView::PositionIndicator::PositionIndicator(ZLTextView &textView, const ZLTextPositionIndicatorInfo &info) : myTextView(textView), myInfo(info), myExtraWidth(0) {
}

ZLTextView::PositionIndicator::~PositionIndicator() {
}

const ZLTextView &ZLTextView::PositionIndicator::textView() const {
	return myTextView;
}

ZLPaintContext &ZLTextView::PositionIndicator::context() const {
	return myTextView.textArea().context();
}

int ZLTextView::PositionIndicator::bottom() const {
	return context().height() - myTextView.bottomMargin() - 1;
}

int ZLTextView::PositionIndicator::top() const {
	return bottom() - myInfo.height() + 1;
}

int ZLTextView::PositionIndicator::left() const {
	return myTextView.textArea().hOffset();
}

int ZLTextView::PositionIndicator::right() const {
	return myTextView.textArea().hOffset() + myTextView.textArea().width() - myExtraWidth - 1;
}

const std::vector<std::size_t> &ZLTextView::PositionIndicator::textSize() const {
	return myTextView.myTextSize;
}

std::size_t ZLTextView::startTextIndex() const {
	std::vector<std::size_t>::const_iterator i = nextBreakIterator();
	return (i != myTextBreaks.begin()) ? *(i - 1) + 1 : 0;
}

std::size_t ZLTextView::endTextIndex() const {
	std::vector<std::size_t>::const_iterator i = nextBreakIterator();
	return (i != myTextBreaks.end()) ? *i : textArea().model()->paragraphsNumber();
}

void ZLTextView::PositionIndicator::drawExtraText(const std::string &text) {
	context().setFont(myTextView.baseStyle()->fontFamily(), myInfo.fontSize(), false, false);
	context().setColor(myTextView.color());

	int width = context().stringWidth(text.data(), text.length(), false);
	context().drawString(right() - width, bottom() - 2, text.data(), text.length(), false);
	myExtraWidth += text.length() * context().stringWidth("0", 1, false) + context().spaceWidth();
}

std::size_t ZLTextView::sizeOfTextBeforeParagraph(std::size_t paragraphIndex) const {
	if (textArea().model()->kind() == ZLTextModel::TREE_MODEL) {
		ZLTextWordCursor cursor = textArea().startCursor();
		if (cursor.isNull()) {
			cursor = textArea().endCursor();
		}
		if (!cursor.isNull()) {
			const ZLTextTreeModel &treeModel = (const ZLTextTreeModel&)*textArea().model();
			std::size_t sum = 0;
			for (std::size_t i = 0; i < paragraphIndex; ++i) {
				const ZLTextTreeParagraph *para = (const ZLTextTreeParagraph*)treeModel[i];
				if (para->parent()->isOpen()) {
					sum += sizeOfParagraph(i);
				}
			}
			return sum;
		}
	}
	return myTextSize[paragraphIndex] - myTextSize[startTextIndex()];
}

std::size_t ZLTextView::sizeOfParagraph(std::size_t paragraphIndex) const {
	return myTextSize[paragraphIndex + 1] - myTextSize[paragraphIndex];
}

std::size_t ZLTextView::sizeOfTextBeforeCursor(const ZLTextWordCursor &cursor) const {
	const std::size_t paragraphIndex = cursor.paragraphCursor().index();
	const std::size_t paragraphLength = cursor.paragraphCursor().paragraphLength();

	if (paragraphLength == 0) {
		return sizeOfTextBeforeParagraph(paragraphIndex);
	} else {
		return
			sizeOfTextBeforeParagraph(paragraphIndex) +
			muldiv(sizeOfParagraph(paragraphIndex), cursor.elementIndex(), paragraphLength);
	}
}

std::string ZLTextView::PositionIndicator::textPositionString() const {
	std::string buffer;
	ZLStringUtil::appendNumber(buffer, 1 + myTextView.sizeOfTextBeforeCursor(myTextView.textArea().endCursor()) / 2048);
	buffer += '/';
	ZLStringUtil::appendNumber(buffer, 1 + myTextView.sizeOfTextBeforeParagraph(myTextView.endTextIndex()) / 2048);

	return buffer;

	/*
	std::string buffer;

	const std::vector<std::size_t> &textSizeVector = myTextView.myTextSize;
	const std::size_t fullTextSize = textSizeVector[endTextIndex()] - textSizeVector[startTextIndex()];
	ZLStringUtil::appendNumber(buffer, 100 * sizeOfTextBeforeCursor(myTextView.textArea().endCursor()) / fullTextSize);

	return buffer + '%';
	*/
}

std::string ZLTextView::PositionIndicator::timeString() const {
	std::string buffer;
	ZLTime time;
	const short hours = time.hours();
	ZLStringUtil::appendNumber(buffer, hours / 10);
	ZLStringUtil::appendNumber(buffer, hours % 10);

	buffer += ':';

	const short minutes = time.minutes();
	ZLStringUtil::appendNumber(buffer, minutes / 10);
	ZLStringUtil::appendNumber(buffer, minutes % 10);

	return buffer;
}

void ZLTextView::PositionIndicator::draw() {
	ZLPaintContext &context = this->context();

	ZLTextWordCursor endCursor = myTextView.textArea().endCursor();
	bool isEndOfText = false;
	if (endCursor.isEndOfParagraph()) {
		isEndOfText = !endCursor.nextParagraph();
	}

	myExtraWidth = 0;
	if (myInfo.isTimeShown()) {
		drawExtraText(timeString());
	}
	if (myInfo.isTextPositionShown()) {
		drawExtraText(textPositionString());
	}

	const long bottom = this->bottom();
	const long top = this->top();
	const long left = this->left();
	const long right = this->right();

	if (left >= right) {
		return;
	}

	std::size_t fillWidth = right - left - 1;

	if (!isEndOfText) {
		fillWidth =
			muldiv(fillWidth, myTextView.sizeOfTextBeforeCursor(myTextView.textArea().endCursor()), myTextView.sizeOfTextBeforeParagraph(myTextView.endTextIndex()));
	}

	context.setColor(myTextView.color());
	context.setFillColor(myInfo.color());
	context.fillRectangle(left + 1, top + 1, left + fillWidth + 1, bottom - 1);
	context.drawLine(left, top, right, top);
	context.drawLine(left, bottom, right, bottom);
	context.drawLine(left, bottom, left, top);
	context.drawLine(right, bottom, right, top);
}

bool ZLTextView::PositionIndicator::isResponsibleFor(int x, int y) {
	x = myTextView.textArea().realX(x);
	return x >= left() && x <= right() && y >= top() && y <= bottom();
}

bool ZLTextView::PositionIndicator::onStylusPress(int x, int y) {
	x = myTextView.textArea().realX(x);

	const long bottom = this->bottom();
	const long top = this->top();
	const long left = this->left();
	const long right = this->right();

	if ((x < left) || (x > right) || (y < top) || (y > bottom)) {
		return false;
	}

	const std::vector<std::size_t> &textSizeVector = myTextView.myTextSize;
	if (textSizeVector.size() <= 1) {
		return true;
	}

	if (myTextView.textArea().endCursor().isNull()) {
		return false;
	}
	std::size_t fullTextSize = myTextView.sizeOfTextBeforeParagraph(myTextView.endTextIndex());
	std::size_t textSize = muldiv(fullTextSize, x - left + 1, right - left + 1);

	myTextView.gotoCharIndex(textSize);

	return true;
}

shared_ptr<ZLTextView::PositionIndicator> ZLTextView::createPositionIndicator(const ZLTextPositionIndicatorInfo &info) {
	return new PositionIndicator(*this, info);
}

shared_ptr<ZLTextView::PositionIndicator> ZLTextView::positionIndicator() {
	if (myPositionIndicator.isNull()) {
		shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo = this->indicatorInfo();
		if (!indicatorInfo.isNull()) {
			myPositionIndicator = createPositionIndicator(*indicatorInfo);
		}
	}
	return myPositionIndicator;
}
