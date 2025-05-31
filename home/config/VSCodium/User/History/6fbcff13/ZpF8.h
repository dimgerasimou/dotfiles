#ifndef CONFIG_H
#define CONFIG_H

/* battery.c */
#ifdef BATTERY_C
#define POWER_MANAGEMENT
const char battery_device_path[] = "/sys/class/power_supply/BAT1";  /* check if this exists and match */
#endif


#endif /* CONFIG_H */