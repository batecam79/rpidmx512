/**
 * @file dmxsendparamssave.cpp
 *
 */
/* Copyright (C) 2019-2021 by Arjan van Vught mailto:info@orangepi-dmx.nl
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
#include <cstring>
#include <cassert>

#include "dmxparams.h"
#include "dmxsendconst.h"

#include "propertiesbuilder.h"

#include "debug.h"

void DMXParams::Builder(const struct TDMXParams *ptDMXParams, char *pBuffer, uint32_t nLength, uint32_t& nSize) {
	DEBUG_ENTRY

	assert(pBuffer != nullptr);

	if (ptDMXParams != nullptr) {
		memcpy(&m_tDMXParams, ptDMXParams, sizeof(struct TDMXParams));
	} else {
		m_pDMXParamsStore->Copy(&m_tDMXParams);
	}

	PropertiesBuilder builder(DMXSendConst::PARAMS_FILE_NAME, pBuffer, nLength);

	builder.Add(DMXSendConst::PARAMS_BREAK_TIME, m_tDMXParams.nBreakTime, isMaskSet(DmxSendParamsMask::BREAK_TIME));
	builder.Add(DMXSendConst::PARAMS_MAB_TIME, m_tDMXParams.nMabTime, isMaskSet(DmxSendParamsMask::MAB_TIME));
	builder.Add(DMXSendConst::PARAMS_REFRESH_RATE, m_tDMXParams.nRefreshRate, isMaskSet(DmxSendParamsMask::REFRESH_RATE));

	nSize = builder.GetSize();

	DEBUG_PRINTF("nSize=%d", nSize);
	DEBUG_EXIT
}

void DMXParams::Save(char *pBuffer, uint32_t nLength, uint32_t& nSize) {
	DEBUG_ENTRY

	assert(pBuffer != nullptr);

	if (m_pDMXParamsStore == nullptr) {
		nSize = 0;
		DEBUG_EXIT
		return;
	}

	return Builder(nullptr, pBuffer, nLength, nSize);
}
