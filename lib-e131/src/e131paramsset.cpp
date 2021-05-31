/**
 * @file e131paramsset.cpp
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

#if !defined(__clang__)	// Needed for compiling on MacOS
# pragma GCC push_options
# pragma GCC optimize ("Os")
#endif

#include <cstdint>
#include <cassert>

#include "e131params.h"
#include "e131.h"

using namespace e131;

void E131Params::Set(E131Bridge *pE131Bridge) {
	assert(pE131Bridge != nullptr);

	if (m_tE131Params.nSetList == 0) {
		return;
	}

	for (uint8_t i = 0; i < E131_PARAMS::MAX_PORTS; i++) {
		if (isMaskSet(E131ParamsMask::MERGE_MODE_A << i)) {
			pE131Bridge->SetMergeMode(i, static_cast<Merge>(m_tE131Params.nMergeModePort[i]));
		} else {
			pE131Bridge->SetMergeMode(i, static_cast<Merge>(m_tE131Params.nMergeMode));
		}
	}

	if (isMaskSet(E131ParamsMask::NETWORK_TIMEOUT)) {
		pE131Bridge->SetDisableNetworkDataLossTimeout(m_tE131Params.nNetworkTimeout < NETWORK_DATA_LOSS_TIMEOUT_SECONDS);
	}

	if (isMaskSet(E131ParamsMask::DISABLE_MERGE_TIMEOUT)) {
		pE131Bridge->SetDisableMergeTimeout(true);
	}

	if (isMaskSet(E131ParamsMask::ENABLE_NO_CHANGE_OUTPUT)) {
		pE131Bridge->SetDirectUpdate(true);
	}

	if (isMaskSet(E131ParamsMask::PRIORITY)) {
		pE131Bridge->SetPriority(m_tE131Params.nPriority);
	}
}
