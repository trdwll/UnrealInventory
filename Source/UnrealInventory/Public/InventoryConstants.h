#pragma once

namespace UnrealInventory
{
	namespace Items
	{
		static const FLinearColor CommonColor = FLinearColor::Gray;
		static const FLinearColor UncommonColor = FLinearColor::Green;
		static const FLinearColor RareColor = FLinearColor::Blue;
		static const FLinearColor EpicColor = FLinearColor(FColor::Purple);
		static const FLinearColor LegendaryColor = FLinearColor(FColor::Orange);

		static constexpr float MaxWeight = 300.0f;
		static constexpr int32 MaxItems = 200;

		static constexpr int32 DefaultArrayItemReserveSize = 20;
	};

	namespace Layout
	{
		static constexpr uint8 MaxItemsPerRow = 4;
	};

	static constexpr int32 TemporaryArrayReserveSize = 10;
};