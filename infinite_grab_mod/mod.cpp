// Infinite Grab mod (Deku Baba).
//
// In daAlink_c::procDkCaught, Link's grab counter (mProcVar0.field_0x3008) drops by 1 every frame
// and by extra when the player mashes the escape button or rotates the stick. When it goes below 0,
// Link is released. This mod cancels the per-frame countdown, so only player input can free Link.

#include "global.h"

#include "d/actor/d_a_alink.h"
#include "mods/service.hpp"
#include "mods/svc/hook.h"
#include "mods/svc/hook.hpp"
#include "mods/svc/log.h"

DEFINE_MOD();
IMPORT_SERVICE(HookService, svc_hook);
IMPORT_SERVICE(LogService, svc_log);

namespace {

DEFINE_HOOK(&daAlink_c::procDkCaught, DkCaughtProc);

// Runs just before procDkCaught each frame. The original then does "field_0x3008--", so adding 1
// here makes the time-based drop a net zero. Escape-button and stick-rotation drops still apply.
HookAction on_dk_caught_pre(ModContext*, void*, void*, void*) {
    daAlink_c* link = daAlink_getAlinkActorClass();
    if (link != nullptr) {
        link->mProcVar0.field_0x3008 += 1;
    }
    return HOOK_CONTINUE;
}

}  // namespace

extern "C" {

MOD_EXPORT ModResult mod_initialize(ModError* error) {
    if (mods::hook::add_pre<DkCaughtProc>(on_dk_caught_pre) != MOD_OK) {
        return mods::set_error(error, MOD_ERROR, "failed to hook daAlink_c::procDkCaught");
    }
    svc_log->info(mod_ctx, "infinite_grab_mod initialized");
    return MOD_OK;
}

MOD_EXPORT ModResult mod_update(ModError*) {
    return MOD_OK;
}

MOD_EXPORT ModResult mod_shutdown(ModError*) {
    svc_log->info(mod_ctx, "infinite_grab_mod unloaded");
    return MOD_OK;
}
}
