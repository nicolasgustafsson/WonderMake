

template<class TRep, class TRatio>
inline constexpr SRotation<TRep, TRatio>::SRotation(const TRep aRotation) noexcept
    : myRotation(aRotation)
{}

template<class TRep, class TRatio>
inline [[nodiscard]] constexpr bool SRotation<TRep, TRatio>::operator==(const TRep aRhs) const noexcept
{
    return myRotation == aRhs;
}

template<class TRep, class TRatio>
inline [[nodiscard]] constexpr std::partial_ordering SRotation<TRep, TRatio>::operator<=>(const TRep aRhs) const noexcept
{
    return myRotation <=> aRhs;
}

template<class TRep, class TRatio>
template<class TFromRepresentation, class TFromRatio>
inline [[nodiscard]] constexpr bool SRotation<TRep, TRatio>::operator==(const SRotation<TFromRepresentation, TFromRatio> aRhs) const noexcept
{
    return *this == RotationCast<SRotation<TRep, TRatio>>(aRhs).myRotation;
}

template<class TRep, class TRatio>
template<class TFromRepresentation, class TFromRatio>
inline [[nodiscard]] constexpr std::partial_ordering SRotation<TRep, TRatio>::operator<=>(const SRotation<TFromRepresentation, TFromRatio> aRhs) const noexcept
{
    return *this <=> RotationCast<SRotation<TRep, TRatio>>(aRhs).myRotation;
}

template<class TRep, class TRatio>
inline constexpr SRotation<TRep, TRatio>& SRotation<TRep, TRatio>::operator=(const TRep aRhs) noexcept
{
    myRotation = aRhs;
    return *this;
}

template<class TRep, class TRatio>
inline constexpr SRotation<TRep, TRatio>& SRotation<TRep, TRatio>::operator+=(const TRep aRhs) noexcept
{
    myRotation += aRhs;
    return *this;
}

template<class TRep, class TRatio>
inline constexpr SRotation<TRep, TRatio>& SRotation<TRep, TRatio>::operator-=(const TRep aRhs) noexcept
{
    myRotation -= aRhs;
    return *this;
}

template<class TRep, class TRatio>
inline constexpr SRotation<TRep, TRatio>& SRotation<TRep, TRatio>::operator=(const SRotation aRhs) noexcept
{
    myRotation = aRhs.myRotation;
    return *this;
}

template<class TRep, class TRatio>
inline constexpr SRotation<TRep, TRatio>& SRotation<TRep, TRatio>::operator+=(const SRotation aRhs) noexcept
{
    myRotation += aRhs.myRotation;
    return *this;
}

template<class TRep, class TRatio>
inline constexpr SRotation<TRep, TRatio>& SRotation<TRep, TRatio>::operator-=(const SRotation aRhs) noexcept
{
    myRotation -= aRhs.myRotation;
    return *this;
}

template<class TRep, class TRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator+(SRotation<TRep, TRatio> aLhs, const SRotation<TRep, TRatio> aRhs) noexcept
{
    return (aLhs += aRhs);
}

template<class TRep, class TRatio>
inline [[nodiscard]] constexpr SRotation<TRep, TRatio> operator-(SRotation<TRep, TRatio> aLhs, const SRotation<TRep, TRatio> aRhs) noexcept
{
    return (aLhs -= aRhs);
}

template<class TRep, class TRatio>
inline [[nodiscard]] constexpr TRep SRotation<TRep, TRatio>::Rotation() const noexcept
{
    return myRotation;
}

template <class TToRotation, class FromTRepresentation, class TFromRatio>
requires std::_Is_specialization_v<TToRotation, SRotation>
[[nodiscard]] constexpr TToRotation RotationCast(const SRotation<FromTRepresentation, TFromRatio> aRotation) noexcept
{
    using _CF = std::ratio_divide<typename TToRotation::Ratio, TFromRatio>;

    using ToRepresentation = typename TToRotation::Representation;
    using CommonRepresentation = std::common_type_t<ToRepresentation, FromTRepresentation, f64>;

    constexpr auto num = _CF::num;
    constexpr auto den = _CF::den;

    constexpr bool numIsOne = num == 1;
    constexpr bool denIsOne = den == 1;

    if (denIsOne)
    {
        if (numIsOne)
        {
            return static_cast<TToRotation>(static_cast<ToRepresentation>(aRotation.Rotation()));
        }
        else
        {
            return static_cast<TToRotation>(static_cast<ToRepresentation>(static_cast<CommonRepresentation>(aRotation.Rotation()) * static_cast<CommonRepresentation>(num)));
        }
    }
    else
    {
        if (numIsOne)
        {
            return static_cast<TToRotation>(static_cast<ToRepresentation>(static_cast<CommonRepresentation>(aRotation.Rotation()) / static_cast<CommonRepresentation>(den)));
        }
        else
        {
            return static_cast<TToRotation>(static_cast<ToRepresentation>(static_cast<CommonRepresentation>(aRotation.Rotation()) * static_cast<CommonRepresentation>(num) / static_cast<CommonRepresentation>(den)));
        }
    }
}
