/**
 * @file main.c
 *
 */
/* Copyright (C) 2016-2021 by Arjan van Vught mailto:info@raspberrypi-dmx.nl
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <netinet/in.h>
#include <assert.h>

#include "hardware.h"
#include "networkesp8266.h"
#include "ledblink.h"

#include "console.h"
#include "display.h"

#include "e131bridge.h"
#include "e131params.h"

// DMX output
#include "dmxparams.h"
#include "dmxsend.h"
#ifndef H3
 // Monitor Output
 #include "dmxmonitor.h"
#endif
// Pixel Controller
#include "pixeldmxconfiguration.h"
#include "pixeltype.h"
#include "lightset.h"
#include "ws28xxdmxparams.h"

#if defined(ORANGE_PI)
# include "spiflashinstall.h"
# include "spiflashstore.h"
# include "storee131.h"
# include "storedmxsend.h"
# include "storews28xxdmx.h"
#endif

#include "software_version.h"

using namespace lightset;

constexpr char NETWORK_INIT[] = "Network init ...";
constexpr char BRIDGE_PARMAS[] = "Setting Bridge parameters ...";
constexpr char START_BRIDGE[] = "Starting the Bridge ...";
constexpr char BRIDGE_STARTED[] = "Bridge started";

extern "C" {

void notmain(void) {
	Hardware hw;
	NetworkESP8266 nw;
	LedBlink lb;
	Display display(DisplayType::SSD1306);

#if defined (ORANGE_PI)
	SpiFlashInstall spiFlashInstall;
	SpiFlashStore spiFlashStore;

	StoreE131 storeE131;
	StoreDmxSend storeDmxSend;
	StoreWS28xxDmx storeWS28xxDmx;

	E131Params e131params((E131ParamsStore *)&storeE131);
#else
	E131Params e131params;
#endif

	if (e131params.Load()) {
		e131params.Dump();
	}

	const auto tOutputType = e131params.GetOutputType();

	uint8_t nHwTextLength;
	printf("[V%s] %s Compiled on %s at %s\n", SOFTWARE_VERSION, hw.GetBoardName(nHwTextLength), __DATE__, __TIME__);

	console_puts("WiFi sACN E1.31 ");
	console_set_fg_color(tOutputType == OutputType::DMX ? CONSOLE_GREEN : CONSOLE_WHITE);
	console_puts("DMX Output");
	console_set_fg_color(CONSOLE_WHITE);
#ifndef H3
	console_puts(" / ");
	console_set_fg_color(tOutputType == OutputType::MONITOR ? CONSOLE_GREEN : CONSOLE_WHITE);
	console_puts("Real-time DMX Monitor");
	console_set_fg_color(CONSOLE_WHITE);
#endif
	console_puts(" / ");
	console_set_fg_color(tOutputType == OutputType::SPI ? CONSOLE_GREEN : CONSOLE_WHITE);
	console_puts("Pixel controller {4 Universes}");
	console_set_fg_color(CONSOLE_WHITE);
#ifdef H3
	console_putc('\n');
#endif

#ifndef H3
	DMXMonitor monitor;
#endif

	console_set_top_row(3);

	hw.SetLed(hardware::LedStatus::ON);

	console_status(CONSOLE_YELLOW, NETWORK_INIT);
	display.TextStatus(NETWORK_INIT);

	nw.Init();

	console_status(CONSOLE_YELLOW, BRIDGE_PARMAS);
	display.TextStatus(BRIDGE_PARMAS);

	E131Bridge bridge;
	e131params.Set(&bridge);

	const auto nStartUniverse = e131params.GetUniverse();

	bridge.SetUniverse(0, e131::PortDir::OUTPUT, nStartUniverse);
	bridge.SetDirectUpdate(false);

	DMXSend dmx;
	LightSet *pSpi = nullptr;

	if (tOutputType == OutputType::SPI) {
		PixelDmxConfiguration pixelDmxConfiguration;

#if defined (ORANGE_PI)
		WS28xxDmxParams ws28xxparms(new StoreWS28xxDmx);
#else
		WS28xxDmxParams ws28xxparms;
#endif

		if (ws28xxparms.Load()) {
			ws28xxparms.Set(&pixelDmxConfiguration);
			ws28xxparms.Dump();
		}

		auto *pWS28xxDmx = new WS28xxDmx(pixelDmxConfiguration);
		assert(pWS28xxDmx != nullptr);
		pSpi = pWS28xxDmx;

		display.Printf(7, "%s:%d G%d", PixelType::GetType(pixelDmxConfiguration.GetType()), pixelDmxConfiguration.GetCount(), pixelDmxConfiguration.GetGroupingCount());

		const auto nUniverses = pWS28xxDmx->GetUniverses();
		bridge.SetDirectUpdate(nUniverses != 1);

		for (uint8_t nPortIndex = 1; nPortIndex < nUniverses; nPortIndex++) {
			bridge.SetUniverse(nPortIndex, e131::PortDir::OUTPUT, static_cast<uint16_t>(nStartUniverse + nPortIndex));
		}

		bridge.SetOutput(pSpi);
	}
#ifndef H3
	else if (tOutputType == OutputType::MONITOR) {
		// There is support for HEX output only
		bridge.SetOutput(&monitor);
		monitor.Cls();
		console_set_top_row(20);
	}
#endif
	else {
#if defined (ORANGE_PI)
		DMXParams dmxparams((DMXParamsStore *)&storeDmxSend);
#else
		DMXParams dmxparams;
#endif
		if (dmxparams.Load()) {
			dmxparams.Dump();
			dmxparams.Set(&dmx);
		}

		bridge.SetOutput(&dmx);
	}

	bridge.Print();

	if (tOutputType == OutputType::SPI) {
		assert(pSpi != 0);
		pSpi->Print();
	} else if (tOutputType == OutputType::MONITOR) {
		// Nothing to do
	} else {
		dmx.Print();
	}

	for (uint8_t i = 0; i < 7 ; i++) {
		display.ClearLine(i);
	}

	display.Write(1, "WiFi sACN E1.31 ");

	switch (tOutputType) {
	case OutputType::SPI:
		display.PutString("Pixel");
		break;
#ifndef H3
	case OutputType::MONITOR:
		display.PutString("Monitor");
		break;
#endif
	default:
		display.PutString("DMX");
		break;
	}

	if (nw.GetOpmode() == WIFI_STA) {
		display.Printf(2, "S: %s", nw.GetSsid());
	} else {
		display.Printf(2, "AP (%s)\n", nw.IsApOpen() ? "Open" : "WPA_WPA2_PSK");
	}

	display.Printf(3, "IP: " IPSTR "", IP2STR(Network::Get()->GetIp()));

	if (nw.IsDhcpKnown()) {
		if (nw.IsDhcpUsed()) {
			display.PutString(" D");
		} else {
			display.PutString(" S");
		}
	}

	display.Printf(4, "N: " IPSTR "", IP2STR(Network::Get()->GetNetmask()));
	display.Printf(5, "U: %d", nStartUniverse);
	display.Printf(6, "Active ports: %d", bridge.GetActiveOutputPorts());

	console_status(CONSOLE_YELLOW, START_BRIDGE);
	display.TextStatus(START_BRIDGE);

	bridge.Start();

	console_status(CONSOLE_GREEN, BRIDGE_STARTED);
	display.TextStatus(BRIDGE_STARTED);

#if defined (ORANGE_PI)
	while (spiFlashStore.Flash())
		;
#endif

	hw.WatchdogInit();

	for (;;) {
		hw.WatchdogFeed();
		bridge.Run();
		lb.Run();
	}
}

}
