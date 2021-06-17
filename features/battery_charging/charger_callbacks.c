
#include <stdio.h>

#include "sys_charger.h"

/**
 * \brief Notification hook to be called in pre-charging state.
 *
 */
void sys_charger_ext_hook_precharging(void)
{
        printf("Pre-charging\r\n");
        fflush(stdout);

}

/**
 * \brief Notification hook to be called in charging state.
 *
 */
__WEAK void sys_charger_ext_hook_charging(void)
{
        printf("Charging\r\n");
        fflush(stdout);
}

/**
 * \brief Notification hook to be called when the end of charge (EoC) is reached.
 *
 */
__WEAK void sys_charger_ext_hook_charged(void)
{
        printf("Battery Charged\r\n");
        fflush(stdout);
}


/**
 * \brief Notification hook to be called when TDIE exceeds its defined limits.
 * \see hw_charger_charging_profile_t
 */
void sys_charger_ext_hook_tdie_error(void)
{
        printf("Chip temperature too high. Charging stopped.\r\n");
        fflush(stdout);
}

/**
 * \brief Notification hook to be called when over-voltage occurs.
 * \see hw_charger_charging_profile_t
 */
void sys_charger_ext_hook_ovp_error(void)
{
        printf("Charging stopped due to Over-Voltage\r\n");
        fflush(stdout);
}

/**
 * \brief Notification hook to be called when timeout occurs during the total
 *        charging cycle.
 * \see hw_charger_charging_profile_t
 */
void sys_charger_ext_hook_total_charge_timeout(void)
{
        printf("Charging timed-out\r\n");
        fflush(stdout);
}

/**
 * \brief Notification hook to be called when timeout occurs during the constant voltage
 *        period.
 * \see hw_charger_charging_profile_t
 */
void sys_charger_ext_hook_cv_charge_timeout(void)
{
        printf("CV phase timed-out\r\n");
        fflush(stdout);

}

/**
 * \brief Notification hook to be called when timeout occurs during the constant current
 *        period.
 * \see hw_charger_charging_profile_t
 */
void sys_charger_ext_hook_cc_charge_timeout(void)
{
        printf("CC phase timed-out\r\n");
        fflush(stdout);
}

/**
 * \brief Notification hook to be called when timeout occurs during the pre-charging
 *        period.
 * \see hw_charger_charging_profile_t
 */
void sys_charger_ext_hook_pre_charge_timeout(void)
{
        printf("Pre-Charge phase timed-out\r\n");
        fflush(stdout);
}
