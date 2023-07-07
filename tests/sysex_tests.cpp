//
// Copyright (c) 2023 Native Instruments
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <gtest/gtest.h>

#include <midi/sysex.h>

#include "sysex_tests.h"

#include "sysex7_test_data.h"

//-----------------------------------------------

class sysex : public ::testing::Test
{
  public:
    bool equal(const std::vector<midi::universal_packet>& p,
               const std::vector<midi::data_message>&     d,
               const std::string&                         description)
    {
        using namespace midi;

        EXPECT_EQ(p.size(), d.size()) << description;
        if (p.size() == d.size())
        {
            for (auto n = 0u; n < p.size(); ++n)
            {
                const auto& m = static_cast<const universal_packet&>(d[n]);
                if (p[n] != m)
                {
                    EXPECT_TRUE((p[n] == m)) << description;
                    return false;
                }
            }
        }

        return true;
    }
};

//-----------------------------------------------

size_t   s_num_sysex_data_allocations{ 0 };
unsigned num_sysex_data_allocations()
{
    return s_num_sysex_data_allocations;
}

//-----------------------------------------------

#if NIMIDI2_CUSTOM_SYSEX_DATA_ALLOCATOR

midi::sysex::data_allocator::value_type* midi::sysex::data_allocator::allocate(std::size_t n)
{
    ++s_num_sysex_data_allocations;
    return new value_type[n];
}

void midi::sysex::data_allocator::deallocate(value_type* p, std::size_t) noexcept
{
    return delete[](p);
}

#endif // NIMIDI2_CUSTOM_SYSEX_DATA_ALLOCATOR

//-----------------------------------------------

TEST_F(sysex, sysex_constructor)
{
    using namespace midi;

    // empty
    {
        midi::sysex sx;

        EXPECT_EQ(0u, sx.manufacturerID);
        EXPECT_EQ(0u, sx.data.size());
        EXPECT_EQ(0u, sx.total_data_size());
        EXPECT_TRUE(sx.empty());
        EXPECT_TRUE(sx.is_7bit());
        EXPECT_FALSE(sx.is_8bit());
    }

    // one byte manufacturer only
    {
        midi::sysex sx{ manufacturer::moog };

        EXPECT_EQ(0x040000u, sx.manufacturerID);
        EXPECT_EQ(0u, sx.data.size());
        EXPECT_EQ(1u, sx.total_data_size());
        EXPECT_FALSE(sx.empty());
        EXPECT_TRUE(sx.is_7bit());
        EXPECT_FALSE(sx.is_8bit());
    }

    // three byte manufacturer only
    {
        midi::sysex sx{ manufacturer::native_instruments };

        EXPECT_EQ(0x002109u, sx.manufacturerID);
        EXPECT_EQ(0u, sx.data.size());
        EXPECT_EQ(3u, sx.total_data_size());
        EXPECT_FALSE(sx.empty());
        EXPECT_TRUE(sx.is_7bit());
        EXPECT_FALSE(sx.is_8bit());
    }

    // one byte manufacturer plus 7bit data
    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        midi::sysex sx{ manufacturer::casio, { 1, 2, 3, 4 } };

        EXPECT_EQ(0x440000u, sx.manufacturerID);
        EXPECT_EQ(4u, sx.data.size());
        EXPECT_EQ(5u, sx.total_data_size());
        EXPECT_FALSE(sx.empty());
        EXPECT_TRUE(sx.is_7bit());
        EXPECT_FALSE(sx.is_8bit());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    // three byte manufacturer plus 8bit data
    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        midi::sysex sx{ manufacturer::atari, { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF } };

        EXPECT_EQ(0x000058u, sx.manufacturerID);
        EXPECT_EQ(6u, sx.data.size());
        EXPECT_EQ(9u, sx.total_data_size());
        EXPECT_FALSE(sx.empty());
        EXPECT_FALSE(sx.is_7bit());
        EXPECT_TRUE(sx.is_8bit());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(sysex, equality)
{
    using namespace midi;

    midi::sysex a{ manufacturer::digidesign };
    midi::sysex b{ manufacturer::digidesign, { 1, 2, 3 } };
    midi::sysex c{ manufacturer::doepfer, { 1, 2, 3, 4 } };
    midi::sysex d{ manufacturer::doepfer, { 1, 2, 3, 4 } };

    EXPECT_EQ(a, a);
    EXPECT_NE(a, b);
    EXPECT_NE(a, c);
    EXPECT_NE(a, d);
    EXPECT_NE(b, a);
    EXPECT_EQ(b, b);
    EXPECT_NE(b, d);
    EXPECT_NE(b, d);
    EXPECT_NE(c, a);
    EXPECT_NE(c, b);
    EXPECT_EQ(c, c);
    EXPECT_EQ(c, d);
    EXPECT_NE(d, a);
    EXPECT_NE(d, b);
    EXPECT_EQ(d, c);
    EXPECT_EQ(d, d);
}

//-----------------------------------------------

TEST_F(sysex, assignment)
{
    using namespace midi;

    midi::sysex a{ manufacturer::digidesign };
    midi::sysex b{ manufacturer::digidesign, { 1, 2, 3 } };

    EXPECT_NE(a, b);
    a = b;
    EXPECT_EQ(a, b);
}

//-----------------------------------------------

TEST_F(sysex, sysex7)
{
    using namespace midi;

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        midi::sysex7 sx{ manufacturer::native_instruments, { 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

        EXPECT_TRUE(sx.is_valid());
        EXPECT_TRUE(sx.is_7bit());
        EXPECT_FALSE(sx.is_8bit());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        midi::sysex7 sx{ manufacturer::native_instruments, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xAB } };

        EXPECT_FALSE(sx.is_valid());
        EXPECT_FALSE(sx.is_7bit());
        EXPECT_TRUE(sx.is_8bit());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(sysex, sysex7_add_device_identity)
{
    using namespace midi;

    {
        midi::sysex7 sx{ 0x123456, { 0x04, 0x01, 0x90, 0x22, 77 } };

        const auto pos = sx.data.size();

        const auto identity = device_identity{ midi::manufacturer::native_instruments, 0x1730, 49, 0x00010005 };

        sx.add_device_identity(identity);

        EXPECT_EQ(pos + 11u, sx.data.size());

        EXPECT_EQ(0x00, sx.data[pos + 0]);
        EXPECT_EQ(0x21, sx.data[pos + 1]);
        EXPECT_EQ(0x09, sx.data[pos + 2]);
        EXPECT_EQ(0x30, sx.data[pos + 3]);
        EXPECT_EQ(0x17, sx.data[pos + 4]);
        EXPECT_EQ(49, sx.data[pos + 5]);
        EXPECT_EQ(0x00, sx.data[pos + 6]);
        EXPECT_EQ(0x05, sx.data[pos + 7]);
        EXPECT_EQ(0x00, sx.data[pos + 8]);
        EXPECT_EQ(0x01, sx.data[pos + 9]);
        EXPECT_EQ(0x00, sx.data[pos + 10]);
    }

    {
        midi::sysex7 sx{ 0x443322, { 0x04, 0x10, 99, 22, 77 } };

        const auto pos = sx.data.size();

        auto identity = device_identity{ midi::manufacturer::roland, 0x0807, 0x1234, 0x00500102 };

        sx.add_device_identity(identity);

        EXPECT_EQ(pos + 11u, sx.data.size());

        EXPECT_EQ(0x41, sx.data[pos + 0]);
        EXPECT_EQ(0x00, sx.data[pos + 1]);
        EXPECT_EQ(0x00, sx.data[pos + 2]);
        EXPECT_EQ(0x07, sx.data[pos + 3]);
        EXPECT_EQ(0x08, sx.data[pos + 4]);
        EXPECT_EQ(0x34, sx.data[pos + 5]);
        EXPECT_EQ(0x12, sx.data[pos + 6]);
        EXPECT_EQ(0x02, sx.data[pos + 7]);
        EXPECT_EQ(0x01, sx.data[pos + 8]);
        EXPECT_EQ(0x50, sx.data[pos + 9]);
        EXPECT_EQ(0x00, sx.data[pos + 10]);

        sx.add_device_identity(identity);

        EXPECT_EQ(pos + 22u, sx.data.size());

        EXPECT_EQ(0x41, sx.data[pos + 11]);
        EXPECT_EQ(0x00, sx.data[pos + 12]);
        EXPECT_EQ(0x00, sx.data[pos + 13]);
        EXPECT_EQ(0x07, sx.data[pos + 14]);
        EXPECT_EQ(0x08, sx.data[pos + 15]);
        EXPECT_EQ(0x34, sx.data[pos + 16]);
        EXPECT_EQ(0x12, sx.data[pos + 17]);
        EXPECT_EQ(0x02, sx.data[pos + 18]);
        EXPECT_EQ(0x01, sx.data[pos + 19]);
        EXPECT_EQ(0x50, sx.data[pos + 20]);
        EXPECT_EQ(0x00, sx.data[pos + 21]);
    }
}

//-----------------------------------------------

TEST_F(sysex, sysex7_make_device_identity)
{
    using namespace midi;

    {
        midi::sysex7 sx{ 0x56, { 0x04, 1, 9, 9, 22, 77 } };

        const auto pos = sx.data.size();

        const auto identity = device_identity{ midi::manufacturer::native_instruments, 0x1730, 49, 0x00010005 };

        sx.add_device_identity(identity);

        const auto i = sx.make_device_identity(pos);
        EXPECT_EQ(0, memcmp(&identity, &i, sizeof(device_identity)));
    }

    {
        midi::sysex7 sx{ 0x7, { 0x04, 0x19, 9, 0x22, 77 } };

        const auto pos = sx.data.size();

        auto identity = device_identity{ midi::manufacturer::roland, 0x0807, 0x1234, 0x00500102 };

        sx.add_device_identity(identity);
        sx.add_device_identity(identity);

        const auto i1 = sx.make_device_identity(pos);
        EXPECT_EQ(0, memcmp(&identity, &i1, sizeof(device_identity)));

        const auto i2 = sx.make_device_identity(pos + 11);
        EXPECT_EQ(0, memcmp(&identity, &i2, sizeof(device_identity)));
    }
};

//-----------------------------------------------

TEST_F(sysex, send_sysex7)
{
    using namespace midi;

    for (const auto& entry : sysex7_test_cases)
    {
        auto it = entry.packets.begin();
        send_sysex7(entry.sysex, entry.packets[0].group(), [&](const data_message& p) {
            EXPECT_EQ(p, *it++) << entry.description;
        });
        EXPECT_EQ(it, entry.packets.end()) << entry.description;
    }
}

//-----------------------------------------------

TEST_F(sysex, as_sysex7_packets)
{
    using namespace midi;

    for (const auto& entry : sysex7_test_cases)
    {
        EXPECT_TRUE(equal(entry.packets, as_sysex7_packets(entry.sysex, entry.packets[0].group()), entry.description))
          << entry.description;
    }
}

//-----------------------------------------------
