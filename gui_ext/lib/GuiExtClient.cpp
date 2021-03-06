#define LOG_TAG "GuiExt"

#include <cutils/xlog.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include "GuiExtClient.h"
#include "IGuiExtService.h"

namespace android {

#define GUIEX_LOGV(x, ...) XLOGV("[GuiExtL] "x, ##__VA_ARGS__)
#define GUIEX_LOGD(x, ...) XLOGD("[GuiExtL] "x, ##__VA_ARGS__)
#define GUIEX_LOGI(x, ...) XLOGI("[GuiExtL] "x, ##__VA_ARGS__)
#define GUIEX_LOGW(x, ...) XLOGW("[GuiExtL] "x, ##__VA_ARGS__)
#define GUIEX_LOGE(x, ...) XLOGE("[GuiExtL] "x, ##__VA_ARGS__)

GuiExtClient::GuiExtClient()
{
    assertStateLocked();
}

status_t GuiExtClient::assertStateLocked() const
{
#ifndef MTK_DO_NOT_USE_GUI_EXT
    if (mGuiExtService == NULL) {
        mGuiExtService = checkGuiExtService();
        if (mGuiExtService == NULL) {
            return NAME_NOT_FOUND;
        }

        class DeathObserver : public IBinder::DeathRecipient {
            GuiExtClient & mGuiExtClient;
            virtual void binderDied(const wp<IBinder>& who) {
                ALOGW("GuiEx Service died [%p]", who.unsafe_get());
                mGuiExtClient.serviceDied();
            }
        public:
            DeathObserver(GuiExtClient & service) : mGuiExtClient(service) { }
        };
        mDeathObserver = new DeathObserver(*const_cast<GuiExtClient*>(this));
        mGuiExtService->asBinder()->linkToDeath(mDeathObserver);
    }

    return NO_ERROR;
#else // MTK_DO_NOT_USE_GUI_EXT
    return NO_INIT;
#endif // MTK_DO_NOT_USE_GUI_EXT
}

void GuiExtClient::serviceDied()
{
    Mutex::Autolock _l(mLock);
    GUIEX_LOGI("[%s]", __func__);

    serviceDiedLocked();

    mGuiExtService.clear();
}
};
