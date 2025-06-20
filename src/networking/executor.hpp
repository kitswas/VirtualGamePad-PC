#pragma once

#include "../../VGP_Data_Exchange/C/Colfer.h"
#include "../simulation/simulate.hpp"

struct ParseResult
{
	enum class FailureReason
	{
		None,
		IncompleteData,
		SchemaMismatch,
		DataTooLarge
	};
	vgp_data_exchange_gamepad_reading reading;
	size_t bytes_consumed;
	bool success;
	FailureReason failure_reason = FailureReason::None;
};

ParseResult parse_gamepad_state(const char *data, size_t len);
bool inject_gamepad_state(vgp_data_exchange_gamepad_reading const &reading);
