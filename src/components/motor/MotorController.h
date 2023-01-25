#pragma once

#include <FreeRTOS.h>
#include <timers.h>
#include <cstdint>

namespace Pinetime {
  namespace Controllers {

    class MotorController {
    public:
      MotorController() = default;

      void Init();
      void RunForDuration(uint16_t motorDuration);
      void RunPeriodic(uint16_t motorDuration, uint16_t idleDuration, uint8_t count);
      void StartRinging();
      void StopRinging();

    private:
      static void Ring(TimerHandle_t xTimer);
      static void StopMotor(TimerHandle_t xTimer);
      TimerHandle_t shortVib;
      TimerHandle_t longVib;

      static void PeriodicRing(TimerHandle_t xTimer);
      TimerHandle_t periodicVib;
      uint8_t periodicVibCount;
      uint16_t periodicVibDuration;
      uint16_t periodicVibIdleDuration;
    };
  }
}
