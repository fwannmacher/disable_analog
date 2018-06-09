#include <psp2/kernel/modulemgr.h>
#include <psp2/ctrl.h>
#include <taihen.h>

static SceUID hooks[2];

int disable_analog(int port, tai_hook_ref_t ref_hook, SceCtrlData* ctrl, int count)
{
    int ret;

    if (ref_hook == 0)
        ret = 1;
    else
    {
        ret = TAI_CONTINUE(int, ref_hook, port, ctrl, count);
        ctrl->lx = 128;
        ctrl->ly = 128;
        ctrl->rx = 128;
        ctrl->ry = 128;
    }

    return ret;
}

static tai_hook_ref_t ref_hook_peek;
static int peek_analog_patched(int port, SceCtrlData* ctrl, int count)
{
    return disable_analog(port, ref_hook_peek, ctrl, count);
}   

static tai_hook_ref_t ref_hook_read;
static int read_analog_patched(int port, SceCtrlData* ctrl, int count)
{
    return disable_analog(port, ref_hook_read, ctrl, count);
}   

void _start() __attribute__ ((weak, alias("module_start")));

int module_start(SceSize argc, const void *args)
{
    hooks[0] = taiHookFunctionImport(
        &ref_hook_peek,
        TAI_MAIN_MODULE,
        TAI_ANY_LIBRARY,
        0xA9C3CED6,
        peek_analog_patched);

    hooks[1] = taiHookFunctionImport(
        &ref_hook_read,
        TAI_MAIN_MODULE,
        TAI_ANY_LIBRARY,
        0x67E7AB83,
        read_analog_patched);

    return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args)
{
    if(hooks[0] >= 0)
        taiHookRelease(hooks[0], ref_hook_peek);

    if(hooks[1] >= 0)
        taiHookRelease(hooks[1], ref_hook_read);

    return SCE_KERNEL_STOP_SUCCESS;
}
