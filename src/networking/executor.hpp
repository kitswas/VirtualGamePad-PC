#pragma once

#include "../../VGP_Data_Exchange/C/Colfer.h"
#include "../settings/input_types.hpp"
#include "../simulation/gamepadSim.hpp"
#include "../simulation/keyboardSim.hpp"
#include "../simulation/mouseSim.hpp"

#include <memory>

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

// An executor Interface for handling gamepad state injection
class ExecutorInterface
{
  public:
	virtual ~ExecutorInterface() = default;

	virtual bool inject_gamepad_state(vgp_data_exchange_gamepad_reading const &reading) = 0;
};

class GamepadExecutor : public ExecutorInterface
{
  public:
	GamepadExecutor() = default;
	~GamepadExecutor() override = default;

	// Delete copy and move operations because GamepadInjector is non-movable
	GamepadExecutor(const GamepadExecutor &) = delete;
	GamepadExecutor &operator=(const GamepadExecutor &) = delete;
	GamepadExecutor(GamepadExecutor &&) = delete;
	GamepadExecutor &operator=(GamepadExecutor &&) = delete;

	bool inject_gamepad_state(vgp_data_exchange_gamepad_reading const &reading) override;

  private:
	GamepadInjector m_injector;
};

/**
 * @brief Executes gamepad input by converting it to keyboard and mouse actions.
 *
 * @details
 * This class takes gamepad input and converts it to equivalent keyboard and mouse
 * actions based on the active keymap profile. It handles button mappings,
 * thumbstick-to-mouse movement, and thumbstick-to-key mappings.
 */
class KeyboardMouseExecutor : public ExecutorInterface
{
  public:
	/**
	 * @brief Threshold for thumbstick input to register as button press.
	 */
	static constexpr double THRESHOLD = 0.5;

	KeyboardMouseExecutor();
	~KeyboardMouseExecutor() override = default;

	// Delete copy and move operations
	KeyboardMouseExecutor(const KeyboardMouseExecutor &) = delete;
	KeyboardMouseExecutor &operator=(const KeyboardMouseExecutor &) = delete;
	KeyboardMouseExecutor(KeyboardMouseExecutor &&) = delete;
	KeyboardMouseExecutor &operator=(KeyboardMouseExecutor &&) = delete;

	/**
	 * @brief Processes gamepad input and executes corresponding keyboard/mouse actions.
	 *
	 * @param reading The gamepad input data to process.
	 * @return true if the input was processed successfully, false otherwise.
	 */
	bool inject_gamepad_state(vgp_data_exchange_gamepad_reading const &reading) override;

  private:
	std::unique_ptr<KeyboardInjector> m_keyboardInjector;
	std::unique_ptr<MouseInjector> m_mouseInjector;

	void handleButtonDown(const ButtonInput &buttonInput);
	void handleButtonUp(const ButtonInput &buttonInput);
	void handleThumbstickInput(const ThumbstickInput &thumbstick, float x_value, float y_value,
							   double threshold);
	void handleTriggerInput(const TriggerInput &trigger, float trigger_value);
};
