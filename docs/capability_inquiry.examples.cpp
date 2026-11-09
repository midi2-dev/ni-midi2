#include <midi/capability_inquiry.h>

#include <cassert>

void profile_id_examples()
{
    using namespace midi;

    // a Standard Defined Profile: Profile Number 0x21, Version 1, Level 1
    constexpr ci::profile_id my_profile{ 0x7E, 0x00, 0x21, 0x01, 0x01 };

    // the same profile with a different Profile Version and Profile Level
    constexpr ci::profile_id other_version{ 0x7E, 0x00, 0x21, 0x02, 0x03 };

    static_assert(!(my_profile == other_version));
    static_assert(ci::same_profile(my_profile, other_version));

    // a Manufacturer Specific Profile is compared by all five bytes
    constexpr ci::profile_id manufacturer_profile{ 0x00, 0x21, 0x09, 0x01, 0x01 };
    static_assert(!ci::same_profile(manufacturer_profile, ci::profile_id{ 0x00, 0x21, 0x09, 0x02, 0x01 }));

    // check whether an incoming Profile Details Inquiry refers to my profile
    const sysex7 sx = ci::make_profile_details_inquiry(0x1234567, 0x7654321, other_version, 0x00);

    if (auto v = ci::as<ci::profile_id_view>(sx))
    {
        assert(ci::same_profile(v->profile(), my_profile));
        // reply with the details of my profile
        // ...
    }
}

void run_capability_inquiry_examples()
{
    profile_id_examples();
}
