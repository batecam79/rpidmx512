/**
 * @file main.cpp
 *
 */
/* Copyright (C) 2019-2023 by Arjan van Vught mailto:info@orangepi-dmx.nl
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
#include <cstdio>

#include "hardware.h"
#include "network.h"
#include "networkconst.h"

#include "mdns.h"

#include "ntpclient.h"

#include "console.h"
#include "h3/showsystime.h"

#include "displayudf.h"
#include "displayudfparams.h"
#include "displayhandler.h"

#include "artnetnode.h"
#include "artnetparams.h"
#include "artnetmsgconst.h"

#include "timecode.h"

#include "dmxmonitor.h"

#include "remoteconfig.h"
#include "remoteconfigparams.h"

#include "flashcodeinstall.h"
#include "configstore.h"


#include "firmwareversion.h"
#include "software_version.h"

namespace artnetnode {
namespace configstore {
uint32_t DMXPORT_OFFSET = 0;
}  // namespace configstore
}  // namespace artnetnode

void Hardware::RebootHandler() { }

void main() {
	Hardware hw;
	DisplayUdf display;
	ConfigStore configStore;
	display.TextStatus(NetworkConst::MSG_NETWORK_INIT, Display7SegmentMessage::INFO_NETWORK_INIT, CONSOLE_YELLOW);
	Network nw;
	MDNS mDns;
	display.TextStatus(NetworkConst::MSG_NETWORK_STARTED, Display7SegmentMessage::INFO_NONE, CONSOLE_GREEN);
	FirmwareVersion fw(SOFTWARE_VERSION, __DATE__, __TIME__);
	FlashCodeInstall spiFlashInstall;

	console_clear();

	fw.Print();

	console_puts("Art-Net 4 Node ");
	console_set_fg_color(CONSOLE_GREEN);
	console_puts("Real-time DMX Monitor");
	console_set_fg_color(CONSOLE_WHITE);
	console_set_top_row(2);

	nw.Print();

	NtpClient ntpClient;
	ntpClient.Start();
	ntpClient.Print();

	ShowSystime showSystime;

	display.TextStatus(ArtNetMsgConst::PARAMS, Display7SegmentMessage::INFO_NODE_PARMAMS, CONSOLE_YELLOW);

	ArtNetNode node;

	ArtNetParams artnetParams;
	artnetParams.Load();
	artnetParams.Set();

	node.SetUniverse(0, lightset::PortDir::OUTPUT, artnetParams.GetUniverse(0));

	TimeCode timecode;
	timecode.Start();
	node.SetTimeCodeHandler(&timecode);

	DMXMonitor monitor;
	// There is support for HEX output only
	node.SetOutput(&monitor);
	monitor.Cls();
	console_set_top_row(20);
	console_clear_top_row();

	node.Print();

	display.SetTitle("Eth Art-Net 4 Monitor");
	display.Set(2, displayudf::Labels::IP);
	display.Set(3, displayudf::Labels::VERSION);
	display.Set(4, displayudf::Labels::UNIVERSE_PORT_A);
	display.Set(5, displayudf::Labels::AP);

	DisplayUdfParams displayUdfParams;
	displayUdfParams.Load();
	displayUdfParams.Set(&display);

	display.Show(&node);

	RemoteConfig remoteConfig(remoteconfig::Node::ARTNET, remoteconfig::Output::MONITOR, 1);

	RemoteConfigParams remoteConfigParams;
	remoteConfigParams.Load();
	remoteConfigParams.Set(&remoteConfig);

	while (configStore.Flash())
		;

	mDns.Print();

	display.TextStatus(ArtNetMsgConst::START, Display7SegmentMessage::INFO_NODE_START, CONSOLE_YELLOW);

	node.Start();

	display.TextStatus(ArtNetMsgConst::STARTED, Display7SegmentMessage::INFO_NODE_STARTED, CONSOLE_GREEN);

	hw.WatchdogInit();

	for (;;) {
		hw.WatchdogFeed();
		nw.Run();
		node.Run();
		remoteConfig.Run();
		configStore.Flash();
		ntpClient.Run();
		showSystime.Run();
		mDns.Run();
		display.Run();
		hw.Run();
	}
}
