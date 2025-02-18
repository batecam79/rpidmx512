/**
 * @file artnetoutput.cpp
 *
 */
/* Copyright (C) 2020-2023 by Arjan van Vught mailto:info@orangepi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <cstdint>

#include "artnetoutput.h"

#include "e131bridge.h"
#include "artnetcontroller.h"

#include "debug.h"

ArtNetOutput::ArtNetOutput() {
	DEBUG_ENTRY

	for (uint32_t i = 0; i < e131bridge::MAX_PORTS; i++) {
		m_nUniverse[i] = 0;
	}

	DEBUG_EXIT
}

void ArtNetOutput::Handler() {
	DEBUG_ENTRY

	ArtNetController::Get()->HandleSync();

	DEBUG_EXIT
}

void ArtNetOutput::Start(const uint32_t nPortIndex) {
	DEBUG_ENTRY
	DEBUG_PRINTF("nPortIndex=%u", nPortIndex);

	if (nPortIndex < e131bridge::MAX_PORTS) {
		uint16_t nUniverse;

		if (E131Bridge::Get()->GetUniverse(static_cast<uint8_t>(nPortIndex), nUniverse, lightset::PortDir::OUTPUT)) {
			m_nUniverse[nPortIndex] = nUniverse;
			DEBUG_PRINTF("m_nUniverse[%u]=%d", nPortIndex, static_cast<int>(m_nUniverse[nPortIndex]));
		}
	}

	DEBUG_EXIT
}

void ArtNetOutput::Stop(const uint32_t nPortIndex) {
	DEBUG_ENTRY

	if (nPortIndex < e131bridge::MAX_PORTS) {
		uint16_t nUniverse;

		if (E131Bridge::Get()->GetUniverse(static_cast<uint8_t>(nPortIndex), nUniverse, lightset::PortDir::OUTPUT)) {
			m_nUniverse[nPortIndex] = 0;
			DEBUG_PRINTF("m_nUniverse[%d]=0", static_cast<int>(nPortIndex));
		}
	}

	DEBUG_EXIT
}

void ArtNetOutput::SetData(uint32_t nPortIndex, const uint8_t *pDmxData, uint32_t nLength, __attribute__((unused)) const bool doUpdate) {
	assert(nPortIndex < e131bridge::MAX_PORTS);

	if (m_nUniverse[nPortIndex] != 0) {
		ArtNetController::Get()->HandleDmxOut(m_nUniverse[nPortIndex], pDmxData, nLength, static_cast<uint8_t>(nPortIndex));
	}
}

void ArtNetOutput::Sync(__attribute__((unused)) uint32_t const nPortIndex) {
	//TODO Implement Sync
}

void ArtNetOutput::Sync(__attribute__((unused)) const bool doForce) {
	DEBUG_ENTRY

	//TODO Implement Sync

	DEBUG_EXIT
}
