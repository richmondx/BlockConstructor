// Copyright 2015-2016 Vagen Ayrapetyan. All Rights Reserved.

#pragma once


#include "Object.h"
#include "BlockConstructorData.generated.h"

// Camera State Type
UENUM(BlueprintType)
enum class ETypeOfOptimization : uint8
{
	Horizontal UMETA(DisplayName = "Horizontal, Flat Terrain (Fast)"),
	Volumetic UMETA(DisplayName = "Volumetic (Slow, Efficient)"),
};


struct ConstructorPosition 
{
public:
	uint16 X;
	uint16 Y;
	uint16 Z;
	ConstructorPosition() {}
	ConstructorPosition(const uint16& GlobalPosition) :X(GlobalPosition), Y(GlobalPosition), Z(GlobalPosition)	{}

	ConstructorPosition(const uint16& newX, const uint16& newY, const uint16& newZ) :X(newX), Y(newY), Z(newZ){}

	friend inline bool operator==(const ConstructorPosition& lhs, const ConstructorPosition& rhs) 
	{                           
		return ((lhs.X == rhs.X) && (lhs.Y == rhs.Y) && (lhs.Z == rhs.Z));
	}
	FString ToString()const 
	{
		FString PrintText = TEXT("x=") + FString::FromInt(X) + TEXT("  y=") + FString::FromInt(Y) + TEXT("   z=") + FString::FromInt(Z);
		return PrintText;
	}
	/*
	inline const TCHAR* ToString()const 
	{
	//	TEXT("MyCharacter's Health is %d"), MyCharacter->Health
		//return (*(TEXT("x="),X)+ FString("y=%i", Y)+ FString("z=%i", Z)));
		return *(TEXT("x=") + FString::FromInt((int32)X) + TEXT("y=") + FString::FromInt((int32)Y) + TEXT("  z=") + FString::FromInt((int32)Z));
	}
	
	*/
};

FORCEINLINE FArchive& operator<<(FArchive &Ar, ConstructorPosition& ThePosition){
	Ar << ThePosition.X<< ThePosition.Y<< ThePosition.Z;
	return Ar;
}

struct SimpleBlockData 
{
public:
	ConstructorPosition Position;
	uint32 ArrayPosition;
	SimpleBlockData() {}
	SimpleBlockData(const ConstructorPosition& newPosition, const uint32 & newArrayPosition):Position(newPosition),ArrayPosition(newArrayPosition)	{}
	
};

FORCEINLINE FArchive& operator<<(FArchive &Ar, SimpleBlockData& TheBlock){
	Ar << TheBlock.Position<< TheBlock.ArrayPosition;
	return Ar;
}


struct MegaBlockCoreData 
{
public:
	uint16 X1;
	uint16 X2;
	uint16 Y1;
	uint16 Y2;
	uint16 Z1;
	uint16 Z2;


	MegaBlockCoreData() {}
	MegaBlockCoreData(const uint16& newZ1, const uint16 & newZ2,const uint16 & newX1, const uint16 & newX2,const uint16 & newY1, const uint16 & newY2)
		:Z1(newZ1), Z2(newZ2), X1(newX1), X2(newX2), Y1(newY1), Y2(newY2){}

	bool IsValid()const 
	{
		return (X2>=X1 && Y2>=Y1 && Z2>=Z1);
	}

	bool IsSingleBlock()const
	{
		return (X1 == X2 && Y1 == Y2 &&	Z1 == Z2);
	}
};
FORCEINLINE FArchive& operator<<(FArchive &Ar, MegaBlockCoreData& TheBlock){
	Ar 	<< TheBlock.Z1 << TheBlock.Z2
		<< TheBlock.X1 << TheBlock.X2
		<< TheBlock.Y1 << TheBlock.Y2;
	return Ar;
}


class MegaBlockMetaData:public MegaBlockCoreData
{
public:
	uint32 BlockNumber=0;

	MegaBlockMetaData() {}

	MegaBlockMetaData(const uint32 & newBlockNumber,const uint16 & newZ1, const uint16& newZ2, const uint16& newX1, const uint16&  newX2, const uint16 &  newY1, const uint16& newY2)
		:MegaBlockCoreData(newZ1,newZ2,newX1,newX2,newY1,newY2), BlockNumber(newBlockNumber){}
};




struct MegaBlockData:public MegaBlockCoreData
{
public:

	FVector Location=FVector::ZeroVector;
	uint32 ArrayPosition;

	MegaBlockData() {}

	MegaBlockData(const MegaBlockCoreData& NewData)
		:MegaBlockCoreData(NewData.Z1,NewData.Z2,NewData.X1, NewData.X2, NewData.Y1, NewData.Y2){}

	MegaBlockData(const MegaBlockMetaData& NewData)
		:MegaBlockCoreData(NewData.Z1, NewData.Z2, NewData.X1, NewData.X2, NewData.Y1, NewData.Y2){}

	void CalculateLocation(const uint16& GridSize)
	{
		Location.X = ((float)((float)X2 + (float)X1)) / 2 * GridSize;
		Location.Y = ((float)((float)Y2 + (float)Y1)) / 2 * GridSize;
		if (Z2>Z1)
			Location.Z = ((float)((float)Z2 + (float)Z1)) / 2 * GridSize;
		else Location.Z = (float)((float)Z1* GridSize);
	}
};
/*
FORCEINLINE FArchive& operator<<(FArchive &Ar, MegaBlockData& TheBlock)
{
	//<< TheBlock.XScale<< TheBlock.YScale<< TheBlock.ZScale
	Ar << TheBlock.Location << TheBlock.ArrayPosition;

	return Ar;
}

*/


struct BlockLayerSaveData 
{
public:
	uint8 LayerMaterialID;

	TArray<ConstructorPosition>  SimpleBlocks_Core;
	TArray<MegaBlockCoreData>    MegaBlocks_Core;

	BlockLayerSaveData() {}

	BlockLayerSaveData(uint8 newLayerMaterialID, const TArray<SimpleBlockData>& newSimpleBlocks, const TArray<MegaBlockData>& newMegaBlocks)
		:LayerMaterialID(newLayerMaterialID)
	{
		MegaBlocks_Core.SetNumUninitialized(newMegaBlocks.Num());
		for (int32 i = 0; i < newMegaBlocks.Num(); i++)
		{
			MegaBlocks_Core[i] = MegaBlockCoreData(	newMegaBlocks[i].Z1, newMegaBlocks[i].Z2, 
													newMegaBlocks[i].X1, newMegaBlocks[i].X2, 
													newMegaBlocks[i].Y1, newMegaBlocks[i].Y2 );
		}
		

		SimpleBlocks_Core.SetNumUninitialized(newSimpleBlocks.Num());
		for (int32 i = 0; i < newSimpleBlocks.Num();i++)
		{
			SimpleBlocks_Core[i] = newSimpleBlocks[i].Position;
		}
	}
};
FORCEINLINE FArchive& operator<<(FArchive &Ar, BlockLayerSaveData& TheLayerSaveData){
	Ar << TheLayerSaveData.LayerMaterialID << TheLayerSaveData.SimpleBlocks_Core << TheLayerSaveData.MegaBlocks_Core;
	return Ar;
}



struct BlockConstructorSaveData
{
public:
	TArray<BlockLayerSaveData> TheLayers;

	uint16 LevelSize = 0;
	uint16 LevelHeight = 0;
	uint16 GridSize = 0;

	BlockConstructorSaveData() {}

	BlockConstructorSaveData( const uint16&newLevelSize, const uint16& newLevelHeight, const uint16& newGridSize) :
		LevelSize(newLevelSize),
		GridSize(newGridSize),
		LevelHeight(newLevelHeight){}

	BlockConstructorSaveData(const TArray<BlockLayerSaveData> newTheLayers,const int32&newLevelSize, const int32& newLevelHeight, const float& newGridSize ):
		 TheLayers(newTheLayers),
		 LevelSize(newLevelSize),
		LevelHeight(newLevelHeight),
		GridSize(newGridSize){}
};


FORCEINLINE FArchive& operator<<(FArchive &Ar, BlockConstructorSaveData& SaveData){
	Ar << SaveData.TheLayers<<SaveData.LevelSize<<SaveData.LevelHeight<<SaveData.GridSize;
	return Ar;
}


//BlueprintType
USTRUCT()
struct FBlockMaterialIDTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32  MaterialID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterialInstance* BlockMaterial;

};




// Container of Custom Types
UCLASS()
class UBlockConstructorData: public UObject
{
	GENERATED_BODY()
public:



	UFUNCTION(BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Get Closest Block Constructor", CompactNodeTitle = "Get BlockLevel", Keywords = "GetClosest"), Category = "LevelBlockConstructor")
		static class ALevelBlockConstructor* GetClosestBlockConstructor(UObject* WorldContextObject, const FVector& ThePositon);

	// Access Object from Class
	UFUNCTION(BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Acces Object From Blueprint", CompactNodeTitle = "Access", Keywords = "access"), Category = "LevelBlockConstructor")
	static UObject* ObjectFromBlueprint(UObject* WorldContextObject, UClass* UC);
};
