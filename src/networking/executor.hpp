#pragma once

#include "../../VGP_Data_Exchange/C/Colfer.h"
#include "../simulation/simulate.hpp"




vgp_data_exchange_gamepad_reading parse_gamepad_state(const char *data, size_t len);
bool inject_gamepad_state(vgp_data_exchange_gamepad_reading reading);
