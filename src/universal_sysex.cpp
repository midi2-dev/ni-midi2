#include <midi/universal_sysex.h>

//--------------------------------------------------------------------------

namespace midi::universal_sysex {

//-----------------------------------------------

identity_reply::identity_reply(
  manufacturer_t sysex_id, uint14_t family, uint14_t family_member, uint28_t revision, uint7_t device_id)
  : message(manufacturer::universal_non_realtime, { device_id, 0x06, subtype::identity_reply })
{
    if (sysex_id < 0x10000) // 3 byte manufacturer SysEx ID?
    {
        data.reserve(14);
        data.push_back(0x00);
        data.push_back((sysex_id >> 8) & 0x7F);
        data.push_back(sysex_id & 0x7F);
    }
    else
    {
        data.reserve(12);
        data.push_back((sysex_id >> 16) & 0x7F);
    }

    add_uint14(family);
    add_uint14(family_member);
    add_uint28(revision);
}

//-----------------------------------------------

identity_reply::identity_reply(const device_identity& identity)
  : identity_reply(identity.manufacturer, identity.family, identity.model, identity.revision)
{
}

//--------------------------------------------------------------------------

} // namespace midi::universal_sysex

//--------------------------------------------------------------------------
