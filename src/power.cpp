#include <power.h>

void PowerDeepSleepTimer(int seconds)
{
    esp_sleep_enable_timer_wakeup(seconds * 1000000);
    esp_deep_sleep_start();
}

void PowerLightSleepTimer(int seconds)
{
    esp_sleep_enable_timer_wakeup(seconds * 1000000);
    esp_light_sleep_start();
}