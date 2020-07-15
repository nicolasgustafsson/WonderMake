

template<class TRepresentation, class TRatio>
inline constexpr SRotation<TRepresentation, TRatio>::SRotation(const TRepresentation aRotation) noexcept
    : myRotation(aRotation)
{}

template<class TRepresentation, class TRatio>
inline [[nodiscard]] constexpr bool SRotation<TRepresentation, TRatio>::operator==(const TRepresentation aRhs) const noexcept
{
    return myRotation == aRhs;
}

template<class TRepresentation, class TRatio>
inline [[nodiscard]] constexpr std::partial_ordering SRotation<TRepresentation, TRatio>::operator<=>(const TRepresentation aRhs) const noexcept
{
    return myRotation <=> aRhs;
}

template<class TRepresentation, class TRatio>
template<class TFromRepresentation, class TFromRatio>
inline [[nodiscard]] constexpr bool SRotation<TRepresentation, TRatio>::operator==(const SRotation<TFromRepresentation, TFromRatio> aRhs) const noexcept
{
    return *this == RotationCast<SRotation<TRepresentation, TRatio>>(aRhs).myRotation;
}

template<class TRepresentation, class TRatio>
template<class TFromRepresentation, class TFromRatio>
inline [[nodiscard]] constexpr std::partial_ordering SRotation<TRepresentation, TRatio>::operator<=>(const SRotation<TFromRepresentation, TFromRatio> aRhs) const noexcept
{
    return *this <=> RotationCast<SRotation<TRepresentation, TRatio>>(aRhs).myRotation;
}

template<class TRepresentation, class TRatio>
inline constexpr SRotation<TRepresentation, TRatio>& SRotation<TRepresentation, TRatio>::operator=(const TRepresentation aRhs) noexcept
{
    myRotation = aRhs;
    return *this;
}

template<class TRepresentation, class TRatio>
inline constexpr SRotation<TRepresentation, TRatio>& SRotation<TRepresentation, TRatio>::operator+=(const TRepresentation aRhs) noexcept
{
    myRotation += aRhs;
    return *this;
}

template<class TRepresentation, class TRatio>
inline constexpr SRotation<TRepresentation, TRatio>& SRotation<TRepresentation, TRatio>::operator-=(const TRepresentation aRhs) noexcept
{
    myRotation -= aRhs;
    return *this;
}

template<class TRepresentation, class TRatio>
inline constexpr SRotation<TRepresentation, TRatio>& SRotation<TRepresentation, TRatio>::operator=(const SRotation aRhs) noexcept
{
    myRotation = aRhs.myRotation;
    return *this;
}

template<class TRepresentation, class TRatio>
inline constexpr SRotation<TRepresentation, TRatio>& SRotation<TRepresentation, TRatio>::operator+=(const SRotation aRhs) noexcept
{
    myRotation += aRhs.myRotation;
    return *this;
}

template<class TRepresentation, class TRatio>
inline constexpr SRotation<TRepresentation, TRatio>& SRotation<TRepresentation, TRatio>::operator-=(const SRotation aRhs) noexcept
{
    myRotation -= aRhs.myRotation;
    return *this;
}

template<class TRepresentation, class TRatio>
inline [[nodiscard]] constexpr SRotation<TRepresentation, TRatio> operator+(SRotation<TRepresentation, TRatio> aLhs, const SRotation<TRepresentation, TRatio> aRhs) noexcept
{
    return (aLhs += aRhs);
}

template<class TRepresentation, class TRatio>
inline [[nodiscard]] constexpr SRotation<TRepresentation, TRatio> operator-(SRotation<TRepresentation, TRatio> aLhs, const SRotation<TRepresentation, TRatio> aRhs) noexcept
{
    return (aLhs -= aRhs);
}

template<class TRepresentation, class TRatio>
inline [[nodiscard]] constexpr TRepresentation SRotation<TRepresentation, TRatio>::Rotation() const noexcept
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
