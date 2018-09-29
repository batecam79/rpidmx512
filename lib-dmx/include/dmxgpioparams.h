/**
 * @file gpioparams.h
 *
 */
/* Copyright (C) 2018 by Arjan van Vught mailto:info@raspberrypi-dmx.nl
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

#ifndef DMXGPIOPARAMS_H_
#define DMXGPIOPARAMS_H_

#include <stdint.h>

#if defined(H3)
 #if defined(ORANGE_PI_ONE)
  #define DMX_MAX_OUT		4
 #else
  #define DMX_MAX_OUT		2
 #endif
#else
 #define DMX_MAX_OUT		1
#endif

class DmxGpioParams {
public:
	DmxGpioParams(void);
	~DmxGpioParams(void);

	void Dump(void);

	uint8_t GetDataDirection(bool &isSet) const;
	uint8_t GetDataDirection(bool &isSet, uint8_t out) const;

private:
	bool isMaskSet(uint16_t mask) const;

public:
    static void staticCallbackFunction(void *p, const char *s);

private:
    void callbackFunction(const char *pLine);

private:
    uint32_t m_nSetList;
    uint8_t m_nDmxDataDirection;
    uint8_t m_nDmxDataDirectionOut[DMX_MAX_OUT];
};

#endif /* DMXGPIOPARAMS_H_ */
