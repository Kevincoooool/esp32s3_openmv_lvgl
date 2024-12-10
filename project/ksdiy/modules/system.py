# system code
import sensor, image, time
import matatalab
import _thread

SYSTEM_THREAD_STACK_SIZE = 8 * 1024

def cdc_loop():
    print("cdc loop task start!\r\n")
    while True:
        if matatalab.is_dbg_mode():
            matatalab.cdc_debug_task()
        else:
            matatalab.cdc_task()

_thread.stack_size(SYSTEM_THREAD_STACK_SIZE)
_thread.start_new_thread(cdc_loop,())
