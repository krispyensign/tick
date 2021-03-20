#pragma once
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

using ranges::views::filter, ranges::views::transform, ranges::to,
    ranges::all_of;
