
#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-utility/FlagSet.h"

enum class ETestEnumNoBitwiseOperators
{
	Kalle,
	Hobbe
};

enum class ETestEnum
{
	Empty	= 0b0000,
	A		= 0b0001,
	B		= 0b0010,
	C		= 0b0100
};

template<>
struct EnableEnumBitwiseOperators<ETestEnum>
	: public std::true_type
{};

void StaticTest_EnumBitwiseOperators()
{
	class NotAnEnum;

	static_assert(!CEnumWithBitwiseOperators<NotAnEnum>);
	static_assert(!CEnumWithBitwiseOperators<ETestEnumNoBitwiseOperators>);
	static_assert(CEnumWithBitwiseOperators<ETestEnum>);

	static_assert((ETestEnum::A | ETestEnum::B) == static_cast<ETestEnum>(0b0011));
	static_assert(((ETestEnum::A | ETestEnum::B) & (ETestEnum::B | ETestEnum::C)) == static_cast<ETestEnum>(0b0010));
	static_assert(((ETestEnum::A | ETestEnum::B) ^ (ETestEnum::B | ETestEnum::C)) == static_cast<ETestEnum>(0b0101));
}

void StaticTest_FlagSet()
{
	static_assert(FlagSet<ETestEnum>() == ETestEnum::Empty);
	static_assert(FlagSet(ETestEnum::A) == ETestEnum::A);

	static_assert((FlagSet(ETestEnum::A) | ETestEnum::B) == (ETestEnum::A | ETestEnum::B));
	static_assert((FlagSet(ETestEnum::A | ETestEnum::B) & (ETestEnum::B | ETestEnum::C)) == ETestEnum::B);
	static_assert((FlagSet(ETestEnum::A | ETestEnum::B) ^ (ETestEnum::B | ETestEnum::C)) == (ETestEnum::A | ETestEnum::C));

	static_assert(FlagSet(ETestEnum::A | ETestEnum::B).TestAll(ETestEnum::A | ETestEnum::B));
	static_assert(FlagSet(ETestEnum::A | ETestEnum::B).TestAll(ETestEnum::B));
	static_assert(!FlagSet(ETestEnum::A).TestAll(ETestEnum::A | ETestEnum::B));
	static_assert(FlagSet(ETestEnum::A | ETestEnum::B).TestAny(ETestEnum::A | ETestEnum::B));
	static_assert(FlagSet(ETestEnum::A | ETestEnum::B).TestAny(ETestEnum::A));
	static_assert(!FlagSet(ETestEnum::A | ETestEnum::B).TestAny(ETestEnum::C));

	static_assert((FlagSet(ETestEnum::A) |= ETestEnum::B) == (ETestEnum::A | ETestEnum::B));
	static_assert((FlagSet(ETestEnum::A | ETestEnum::B) &= ETestEnum::B) == ETestEnum::B);
	static_assert((FlagSet(ETestEnum::A | ETestEnum::B) ^= ETestEnum::B) == ETestEnum::A);
	
	static_assert(FlagSet(ETestEnum::A).Or(ETestEnum::B) == (ETestEnum::A | ETestEnum::B));
	static_assert(FlagSet(ETestEnum::A | ETestEnum::B).And(ETestEnum::B) == ETestEnum::B);
	static_assert(FlagSet(ETestEnum::A | ETestEnum::B).Xor(ETestEnum::B) == ETestEnum::A);
}
