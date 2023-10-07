/*
 * rdm_manufacturer_pid.cpp
 */

#undef NDEBUG

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cassert>

#include "rdm_manufacturer_pid.h"
#include "rdmhandler.h"
#include "rdm_e120.h"

#include "tlc59711.h"

#include "debug.h"

#if !defined(OUTPUT_DMX_TLC59711)
# error
# endif

namespace rdm {
using E120_MANUFACTURER_PIXEL_TYPE = ManufacturerPid<0x8500>;
using E120_MANUFACTURER_PIXEL_COUNT = ManufacturerPid<0x8501>;

struct PixelType {
    static constexpr char description[] = "Pixel type";
};

struct PixelCount {
    static constexpr char description[] = "Pixel count";
};

constexpr char PixelType::description[];
constexpr char PixelCount::description[];
}  // namespace rdm

const rdm::ParameterDescription RDMHandler::PARAMETER_DESCRIPTIONS[] = {
		  { rdm::E120_MANUFACTURER_PIXEL_TYPE::code,
		    rdm::DEVICE_DESCRIPTION_MAX_LENGTH,
			E120_DS_ASCII,
			E120_CC_GET,
			0,
			E120_UNITS_NONE,
			E120_PREFIX_NONE,
			0,
			0,
			0,
			rdm::Description<rdm::PixelType, sizeof(rdm::PixelType::description)>::value,
			rdm::pdlParameterDescription(sizeof(rdm::PixelType::description))
		  },
		  { rdm::E120_MANUFACTURER_PIXEL_COUNT::code,
			2,
			E120_DS_UNSIGNED_DWORD,
			E120_CC_GET,
			0,
			E120_UNITS_NONE,
			E120_PREFIX_NONE,
			0,
			__builtin_bswap32(4),
			__builtin_bswap32(4),
			rdm::Description<rdm::PixelCount, sizeof(rdm::PixelCount::description)>::value,
			rdm::pdlParameterDescription(sizeof(rdm::PixelCount::description))
		  }
  };

uint32_t RDMHandler::GetParameterDescriptionCount() const {
	return sizeof(RDMHandler::PARAMETER_DESCRIPTIONS) / sizeof(RDMHandler::PARAMETER_DESCRIPTIONS[0]);
}

#include "tlc59711dmx.h"
#include "tlc59711dmxparams.h"

namespace rdm {
bool handle_manufactureer_pid_get(const uint16_t nPid, __attribute__((unused)) const ManufacturerParamData *pIn, ManufacturerParamData *pOut, uint16_t& nReason) {
	switch (nPid) {
	case rdm::E120_MANUFACTURER_PIXEL_TYPE::code: {
		const auto *pString = TLC59711DmxParams::GetType(TLC59711Dmx::Get()->GetType());
		pOut->nPdl = static_cast<uint8_t>(strlen(pString));
		memcpy(pOut->pParamData, pString, pOut->nPdl);
		return true;
	}
	case rdm::E120_MANUFACTURER_PIXEL_COUNT::code: {
		const auto nCount = TLC59711Dmx::Get()->GetCount();
		pOut->nPdl = 4;
		pOut->pParamData[0] = static_cast<uint8_t>(nCount >> 24);
		pOut->pParamData[1] = static_cast<uint8_t>(nCount >> 16);
		pOut->pParamData[2] = static_cast<uint8_t>(nCount >> 8);
		pOut->pParamData[3] = static_cast<uint8_t>(nCount);
		return true;
	}
	default:
		break;
	}

	nReason = E120_NR_UNKNOWN_PID;
	return false;
}
}  // namespace rdm
