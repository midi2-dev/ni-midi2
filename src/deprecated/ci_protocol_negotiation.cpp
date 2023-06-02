#include <midi/deprecated/ci_protocol_negotiation.h>

#include <array>
#include <cassert>

//--------------------------------------------------------------------------

namespace midi::ci {

//-----------------------------------------------

al_message::al_message(subtype_t      subtype,
                       muid_t         source_muid,
                       muid_t         destination_muid,
                       uint7_t        authority_level,
                       const uint7_t* pData,
                       size_t         data_size,
                       uint7_t        device_id)
  : message(subtype, source_muid, destination_muid, nullptr, 0, device_id)
{
    data.reserve(1 + data_size);

    add_uint7(authority_level);

    if (pData && data_size)
    {
        data.insert(data.end(), pData, pData + data_size);
    }
}

//-----------------------------------------------

al_message::al_message(subtype_t                   subtype,
                       muid_t                      source_muid,
                       muid_t                      destination_muid,
                       uint7_t                     authority_level,
                       const std::vector<uint7_t>& d,
                       uint7_t                     device_id)
  : message(subtype, source_muid, destination_muid, nullptr, 0, device_id)
{
    data.reserve(1 + d.size());

    add_uint7(authority_level);

    data.insert(data.end(), d.begin(), d.end());
}

//-----------------------------------------------

std::vector<protocol_type> protocol_capabilities::supported_protocols() const
{
    std::vector<protocol_type> result;

    unsigned num_protocols = data[offset_of_np];
    auto     offset        = size_of_message;

    result.reserve(num_protocols);
    for (auto protocol = 0u; protocol < num_protocols; ++protocol)
    {
        protocol_type t;
        t.protocol   = data[offset++];
        t.version    = data[offset++];
        t.extensions = data[offset++];
        t.reserved1  = data[offset++];
        t.reserved2  = data[offset++];
        result.push_back(t);
    }

    return result;
}

//-----------------------------------------------

protocol_capabilities::protocol_capabilities(subtype_t            subtype,
                                             muid_t               source_muid,
                                             muid_t               destination_muid,
                                             uint7_t              authority_level,
                                             const protocol_type* protocols,
                                             uint7_t              num_protocols)
  : al_message(subtype, source_muid, destination_muid, authority_level, nullptr, 0, 0x7F)
{
    data.reserve(size_of_message + sizeof(protocol_type) * num_protocols);

    add_uint7(num_protocols);
    for (auto protocol = 0u; protocol < num_protocols; ++protocol)
    {
        add_protocol(protocols[protocol]);
    }
}

//-----------------------------------------------

protocol_capabilities::protocol_capabilities(subtype_t                         subtype,
                                             muid_t                            source_muid,
                                             muid_t                            destination_muid,
                                             uint7_t                           authority_level,
                                             const std::vector<protocol_type>& protocols)
  : al_message(subtype, source_muid, destination_muid, authority_level, nullptr, 0, 0x7F)
{
    assert(protocols.size() <= static_cast<size_t>(uint7_max));

    const auto num_protocols = static_cast<uint7_t>(protocols.size());
    data.reserve(size_of_message + sizeof(protocol_type) * num_protocols);

    add_uint7(num_protocols);
    for (auto protocol = 0u; protocol < num_protocols; ++protocol)
    {
        add_protocol(protocols[protocol]);
    }
}

//-----------------------------------------------

void protocol_capabilities::add_protocol(const protocol_type& type)
{
    add_uint7(type.protocol);
    add_uint7(type.version);
    add_uint7(type.extensions);
    add_uint7(type.reserved1);
    add_uint7(type.reserved2);
}

//-----------------------------------------------

set_new_protocol::set_new_protocol(muid_t               source_muid,
                                   muid_t               destination_muid,
                                   uint7_t              authority_level,
                                   const protocol_type& type)
  : al_message(subtype::set_new_protocol, source_muid, destination_muid, authority_level, nullptr, 0)
{
    data.reserve(size_of_message);
    add_uint7(type.protocol);
    add_uint7(type.version);
    add_uint7(type.extensions);
    add_uint7(type.reserved1);
    add_uint7(type.reserved2);
}

//-----------------------------------------------

static constexpr std::array<uint7_t, 0x30> new_protocol_test_data = {
    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
      0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F }
};

//-----------------------------------------------

bool test_new_protocol::is_valid() const
{
    if ((type() == universal_sysex::type::capability_inquiry) &&
        ((subtype() == subtype::test_new_protocol_i2r) || (subtype() == subtype::test_new_protocol_r2i)) &&
        (data.size() >= size_of_message))
    {
        for (auto i = 0u; i < 48; ++i)
        {
            if (data[offset_of_test_data + i] != i)
                return false;
        }

        return true;
    }

    return false;
}

//-----------------------------------------------

test_new_protocol::test_new_protocol(subtype_t subtype,
                                     muid_t    source_muid,
                                     muid_t    destination_muid,
                                     uint7_t   authority_level)
  : al_message(subtype,
               source_muid,
               destination_muid,
               authority_level,
               new_protocol_test_data.data(),
               new_protocol_test_data.size(),
               0x7F)
{
}

//-----------------------------------------------

confirm_new_protocol_established::confirm_new_protocol_established(muid_t  source_muid,
                                                                   muid_t  destination_muid,
                                                                   uint7_t authority_level)
  : al_message(subtype::new_protocol_established, source_muid, destination_muid, authority_level, nullptr, 0, 0x7F)
{
}

//-----------------------------------------------

} // namespace midi::ci

//--------------------------------------------------------------------------
