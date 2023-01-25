#include "components/motor/MotorController.h"
#include <hal/nrf_gpio.h>
#include "systemtask/SystemTask.h"
#include "drivers/PinMap.h"

using namespace Pinetime::Controllers;

void MotorController::Init() {
  nrf_gpio_cfg_output(PinMap::Motor);
  nrf_gpio_pin_set(PinMap::Motor);

  shortVib = xTimerCreate("shortVib", 1, pdFALSE, nullptr, StopMotor);
  longVib = xTimerCreate("longVib", pdMS_TO_TICKS(1000), pdTRUE, this, Ring);
  periodicVib = xTimerCreate("periodicVib", 1, pdTRUE, this, PeriodicRing);
}

void MotorController::Ring(TimerHandle_t xTimer) {
  auto* motorController = static_cast<MotorController*>(pvTimerGetTimerID(xTimer));
  motorController->RunForDuration(50);
}

void MotorController::PeriodicRing(TimerHandle_t xTimer) {
  auto* motorController = static_cast<MotorController*>(pvTimerGetTimerID(xTimer));

  if (motorController->periodicVibCount == 0) {
    xTimerStop(motorController->periodicVib, 0);
    nrf_gpio_pin_set(PinMap::Motor);
    return;
  }

  motorController->RunForDuration(motorController->periodicVibDuration);
  motorController->periodicVibCount--;
}

void MotorController::RunForDuration(uint16_t motorDuration) {
  if (motorDuration > 0 && xTimerChangePeriod(shortVib, pdMS_TO_TICKS(motorDuration), 0) == pdPASS && xTimerStart(shortVib, 0) == pdPASS) {
    nrf_gpio_pin_clear(PinMap::Motor);
  }
}

void MotorController::RunPeriodic(uint16_t motorDuration, uint16_t idleDuration, uint8_t count) {
  if (count == 0) {
    return;
  }

  RunForDuration(motorDuration);

  if (count == 1) {
    return;
  }

  periodicVibDuration = motorDuration;
  periodicVibIdleDuration = idleDuration;
  periodicVibCount = count - 1;

  xTimerChangePeriod(periodicVib, pdMS_TO_TICKS(motorDuration + idleDuration), 0);
  xTimerStart(periodicVib, 0);
}

void MotorController::StartRinging() {
  RunForDuration(50);
  xTimerStart(longVib, 0);
}

void MotorController::StopRinging() {
  xTimerStop(longVib, 0);
  nrf_gpio_pin_set(PinMap::Motor);
}

void MotorController::StopMotor(TimerHandle_t xTimer) {
  nrf_gpio_pin_set(PinMap::Motor);
}
