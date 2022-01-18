#include "WonderMakeTestsCommon/GTestInclude.h"

#include "WonderMakeBase/MemoryUnit.h"

TEST(MemoryUnitTest, is_converts_between_ratios)
{
	using MemB = MemoryUnit<EMemoryRatio::B, u32>;
	using MemKB = MemoryUnit<EMemoryRatio::KB, u32>;
	using MemKiB = MemoryUnit<EMemoryRatio::KiB, u32>;

	constexpr MemB mu0_B			= 0;
	constexpr MemB mu1000_B			= 1000;
	constexpr MemB mu1024_B			= 1024;
	constexpr MemB mu1000000_B		= 1000 * 1000;
	constexpr MemB mu1048576_B		= 1024 * 1024;

	EXPECT_EQ(mu0_B.To<EMemoryRatio::B>(),				0);
	EXPECT_EQ(mu1000_B.To<EMemoryRatio::B>(),			1000);
	EXPECT_EQ(mu1024_B.To<EMemoryRatio::B>(),			1024);
	EXPECT_EQ(mu1000000_B.To<EMemoryRatio::B>(),		1000000);
	EXPECT_EQ(mu1048576_B.To<EMemoryRatio::B>(),		1048576);
	
	EXPECT_EQ(mu0_B.To<EMemoryRatio::KB>(),				0);
	EXPECT_EQ(mu1000_B.To<EMemoryRatio::KB>(),			1);
	EXPECT_EQ(mu1024_B.To<EMemoryRatio::KB>(),			1);
	EXPECT_EQ(mu1000000_B.To<EMemoryRatio::KB>(),		1000);
	EXPECT_EQ(mu1048576_B.To<EMemoryRatio::KB>(),		1048);

	EXPECT_EQ(mu0_B.To<EMemoryRatio::KiB>(),			0);
	EXPECT_EQ(mu1000_B.To<EMemoryRatio::KiB>(),			0);
	EXPECT_EQ(mu1024_B.To<EMemoryRatio::KiB>(),			1);
	EXPECT_EQ(mu1000000_B.To<EMemoryRatio::KiB>(),		976);
	EXPECT_EQ(mu1048576_B.To<EMemoryRatio::KiB>(),		1024);

	EXPECT_EQ(mu0_B.To<EMemoryRatio::MB>(),				0);
	EXPECT_EQ(mu1000_B.To<EMemoryRatio::MB>(),			0);
	EXPECT_EQ(mu1024_B.To<EMemoryRatio::MB>(),			0);
	EXPECT_EQ(mu1000000_B.To<EMemoryRatio::MB>(),		1);
	EXPECT_EQ(mu1048576_B.To<EMemoryRatio::MB>(),		1);

	EXPECT_EQ(mu0_B.To<EMemoryRatio::MiB>(),			0);
	EXPECT_EQ(mu1000_B.To<EMemoryRatio::MiB>(),			0);
	EXPECT_EQ(mu1024_B.To<EMemoryRatio::MiB>(),			0);
	EXPECT_EQ(mu1000000_B.To<EMemoryRatio::MiB>(),		0);
	EXPECT_EQ(mu1048576_B.To<EMemoryRatio::MiB>(),		1);

	constexpr MemKB mu0_KB			= 0;
	constexpr MemKB mu1000_KB		= 1000;
	constexpr MemKB mu1024_KB		= 1024;
	constexpr MemKB mu1000000_KB	= 1000 * 1000;
	constexpr MemKB mu1048576_KB	= 1024 * 1024;
	
	EXPECT_EQ(mu0_KB.To<EMemoryRatio::B>(),				0);
	EXPECT_EQ(mu1000_KB.To<EMemoryRatio::B>(),			1000000);
	EXPECT_EQ(mu1024_KB.To<EMemoryRatio::B>(),			1024000);
	EXPECT_EQ(mu1000000_KB.To<EMemoryRatio::B>(),		1000000000);
	EXPECT_EQ(mu1048576_KB.To<EMemoryRatio::B>(),		1048576000);
	
	EXPECT_EQ(mu0_KB.To<EMemoryRatio::KB>(),			0);
	EXPECT_EQ(mu1000_KB.To<EMemoryRatio::KB>(),			1000);
	EXPECT_EQ(mu1024_KB.To<EMemoryRatio::KB>(),			1024);
	EXPECT_EQ(mu1000000_KB.To<EMemoryRatio::KB>(),		1000000);
	EXPECT_EQ(mu1048576_KB.To<EMemoryRatio::KB>(),		1048576);

	EXPECT_EQ(mu0_KB.To<EMemoryRatio::KiB>(),			0);
	EXPECT_EQ(mu1000_KB.To<EMemoryRatio::KiB>(),		976);
	EXPECT_EQ(mu1024_KB.To<EMemoryRatio::KiB>(),		1000);
	EXPECT_EQ(mu1000000_KB.To<EMemoryRatio::KiB>(),		976562);
	EXPECT_EQ(mu1048576_KB.To<EMemoryRatio::KiB>(),		1024000);

	EXPECT_EQ(mu0_KB.To<EMemoryRatio::MB>(),			0);
	EXPECT_EQ(mu1000_KB.To<EMemoryRatio::MB>(),			1);
	EXPECT_EQ(mu1024_KB.To<EMemoryRatio::MB>(),			1);
	EXPECT_EQ(mu1000000_KB.To<EMemoryRatio::MB>(),		1000);
	EXPECT_EQ(mu1048576_KB.To<EMemoryRatio::MB>(),		1048);

	EXPECT_EQ(mu0_KB.To<EMemoryRatio::MiB>(),			0);
	EXPECT_EQ(mu1000_KB.To<EMemoryRatio::MiB>(),		0);
	EXPECT_EQ(mu1024_KB.To<EMemoryRatio::MiB>(),		0);
	EXPECT_EQ(mu1000000_KB.To<EMemoryRatio::MiB>(),		953);
	EXPECT_EQ(mu1048576_KB.To<EMemoryRatio::MiB>(),		1000);

	constexpr MemKiB mu0_KiB		= 0;
	constexpr MemKiB mu1000_KiB		= 1000;
	constexpr MemKiB mu1024_KiB		= 1024;
	constexpr MemKiB mu1000000_KiB	= 1000 * 1000;
	constexpr MemKiB mu1048576_KiB	= 1024 * 1024;

	EXPECT_EQ(mu0_KiB.To<EMemoryRatio::B>(),			0);
	EXPECT_EQ(mu1000_KiB.To<EMemoryRatio::B>(),			1024000);
	EXPECT_EQ(mu1024_KiB.To<EMemoryRatio::B>(),			1048576);
	EXPECT_EQ(mu1000000_KiB.To<EMemoryRatio::B>(),		1024000000);
	EXPECT_EQ(mu1048576_KiB.To<EMemoryRatio::B>(),		1073741824);

	EXPECT_EQ(mu0_KiB.To<EMemoryRatio::KB>(),			0);
	EXPECT_EQ(mu1000_KiB.To<EMemoryRatio::KB>(),		1024);
	EXPECT_EQ(mu1024_KiB.To<EMemoryRatio::KB>(),		1048);
	EXPECT_EQ(mu1000000_KiB.To<EMemoryRatio::KB>(),		1024000);
	EXPECT_EQ(mu1048576_KiB.To<EMemoryRatio::KB>(),		1073741);

	EXPECT_EQ(mu0_KiB.To<EMemoryRatio::KiB>(),			0);
	EXPECT_EQ(mu1000_KiB.To<EMemoryRatio::KiB>(),		1000);
	EXPECT_EQ(mu1024_KiB.To<EMemoryRatio::KiB>(),		1024);
	EXPECT_EQ(mu1000000_KiB.To<EMemoryRatio::KiB>(),	1000000);
	EXPECT_EQ(mu1048576_KiB.To<EMemoryRatio::KiB>(),	1048576);

	EXPECT_EQ(mu0_KiB.To<EMemoryRatio::MB>(),			0);
	EXPECT_EQ(mu1000_KiB.To<EMemoryRatio::MB>(),		1);
	EXPECT_EQ(mu1024_KiB.To<EMemoryRatio::MB>(),		1);
	EXPECT_EQ(mu1000000_KiB.To<EMemoryRatio::MB>(),		1024);
	EXPECT_EQ(mu1048576_KiB.To<EMemoryRatio::MB>(),		1073);

	EXPECT_EQ(mu0_KiB.To<EMemoryRatio::MiB>(),			0);
	EXPECT_EQ(mu1000_KiB.To<EMemoryRatio::MiB>(),		0);
	EXPECT_EQ(mu1024_KiB.To<EMemoryRatio::MiB>(),		1);
	EXPECT_EQ(mu1000000_KiB.To<EMemoryRatio::MiB>(),	976);
	EXPECT_EQ(mu1048576_KiB.To<EMemoryRatio::MiB>(),	1024);
}

TEST(MemoryUnitTest, is_comparable)
{
	using MemB = MemoryUnit<EMemoryRatio::B, u32>;
	using MemKB = MemoryUnit<EMemoryRatio::KB, u32>;
	using MemKiB = MemoryUnit<EMemoryRatio::KiB, u32>;

	EXPECT_EQ(MemB(0), MemB(0));
	EXPECT_EQ(MemB(1234), MemB(1234));
	EXPECT_NE(MemB(1234), MemB(4567));
	EXPECT_EQ(MemKB(1024), MemKiB(1000));
	EXPECT_NE(MemB(1), MemB(0));
	EXPECT_NE(MemKB(1), MemKiB(1));

	EXPECT_EQ(MemB(0), MemKB(0));
	EXPECT_EQ(MemB(1000), MemKB(1));
	EXPECT_EQ(MemB(1024), MemKiB(1));

	EXPECT_GE(MemB(0), MemB(0));
	EXPECT_GE(MemKB(0), MemB(0));
	EXPECT_GE(MemKB(0), MemKiB(0));

	EXPECT_GE(MemB(1), MemB(0));
	EXPECT_GE(MemKB(1), MemB(0));
	EXPECT_GE(MemKB(1), MemKiB(0));

	EXPECT_GT(MemB(1), MemB(0));
	EXPECT_GT(MemKB(1), MemB(0));
	EXPECT_GT(MemKB(1), MemKiB(0));

	EXPECT_LE(MemB(0), MemB(0));
	EXPECT_LE(MemKB(0), MemB(0));
	EXPECT_LE(MemKB(0), MemKiB(0));

	EXPECT_LE(MemB(0), MemB(1));
	EXPECT_LE(MemKB(0), MemB(1));
	EXPECT_LE(MemKB(0), MemKiB(1));

	EXPECT_LT(MemB(0), MemB(1));
	EXPECT_LT(MemKB(0), MemB(1));
	EXPECT_LT(MemKB(0), MemKiB(1));
}

TEST(MemoryUnitTest, is_assignable)
{
	using MemB = MemoryUnit<EMemoryRatio::B, u32>;
	using MemKB = MemoryUnit<EMemoryRatio::KB, u32>;
	using MemKiB = MemoryUnit<EMemoryRatio::KiB, u32>;

	MemB	mu1024Bto5000B		(1024);
	MemKB	mu1024KBto5000KB	(1024);
	MemKiB	mu1024KiBto5000KiB	(1024);
	
	mu1024Bto5000B		= 5000;
	mu1024KBto5000KB	= 5000;
	mu1024KiBto5000KiB	= 5000;

	EXPECT_EQ(mu1024Bto5000B,		MemB(5000));
	EXPECT_EQ(mu1024KBto5000KB,		MemKB(5000));
	EXPECT_EQ(mu1024KiBto5000KiB,	MemKiB(5000));
}
