
#include "pal2/pal_core.h"

#define MAX_RESULT 4

typedef struct ResultInfo
{
    PalResultCode code;
    PalResultSource source;
    uint32_t nativeCode;
} ResultInfo;

int main(void)
{
    palLog(nullptr, "Running result test...");

    PalResult results[MAX_RESULT] = {0};

    /**
     * We use dummy native code. This is fine because we are not formatting
     * the result value.
     */
    ResultInfo resultInfos[MAX_RESULT] = {
        {PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED, PAL_RESULT_SOURCE_NONE, 0},
        {PAL_RESULT_CODE_INVALID_HANDLE, PAL_RESULT_SOURCE_D3D12, 122},
        {PAL_RESULT_CODE_INVALID_ARGUMENT, PAL_RESULT_SOURCE_D3D12, 12882},
        {PAL_RESULT_CODE_DEVICE_LOST, PAL_RESULT_SOURCE_VULKAN, 122266}
    };

    for (int i = 0; i < MAX_RESULT; i++) {
        ResultInfo* info = &resultInfos[i];
        results[i] = palMakeResult(info->code, info->source, info->nativeCode);
    }

    for (int i = 0; i < MAX_RESULT; i++) {
        ResultInfo* info = &resultInfos[i];
        PalResult result = results[i];

        PalResultCode code = palGetResultCode(result);
        PalResultSource source = palGetResultSource(result);
        uint32_t nativeCode = palGetResultNativeCode(result);

        if (code != info->code     || 
            source != info->source || 
            nativeCode != info->nativeCode) {
            palLog(nullptr, "Result values do not match");
            return -1;
        }
    }

    return 0;
}