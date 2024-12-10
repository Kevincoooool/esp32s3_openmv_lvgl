#ifndef __FIR_VERSION_H__
#define __FIR_VERSION__

void initNVS();
void save_firmware_version(int version);
int get_firmware_version(void);
int check_firmware_version(int version); 
#endif

