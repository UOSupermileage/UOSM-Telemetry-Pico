//
// Created by Jeremy Cote on 2025-01-15.
//

#include "BackupRegister.h"

#ifdef STM
extern RTC_HandleTypeDef hrtc;
#endif

void Backup_SetFaultStatus(FaultStatus status) {
#ifdef STM
    // Enable write access to backup region
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, status);
#endif
}

FaultStatus Backup_GetFaultStatus() {
#ifdef STM
    // TODO: Confirm that implicitly casting from backup register to FaultStatus is valid
    return HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0);
#else
    return 0;
#endif
}

void Backup_SaveData(BackupData reg, uint32_t data) {
#ifdef STM
    HAL_RTCEx_BKUPWrite(&hrtc, reg, data);
#endif
}

uint32_t Backup_GetData(BackupData reg) {
#ifdef STM
    return HAL_RTCEx_BKUPRead(&hrtc, reg);
#else
    return 0;
#endif
}

void Backup_Unlock() {
#ifdef STM
    HAL_PWR_EnableBkUpAccess();
#endif
}

void Backup_Lock() {
#ifdef STM
    HAL_PWR_DisableBkUpAccess();
#endif
}