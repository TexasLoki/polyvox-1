/******************************************************************************
This file is part of a voxel plugin for OGRE
Copyright (C) 2006  David Williams

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
******************************************************************************/

#include "Volume.h"
#include "VolumeIterator.h"

#include "OgreLogManager.h"
#include "OgreStringConverter.h"
#include "OgreVector3.h"

namespace Ogre
{
	VolumeIterator::VolumeIterator(Volume& volume)
		:mVolume(volume)
		,mXRegionFirst(0)
		,mYRegionFirst(0)
		,mZRegionFirst(0)
		,mXRegionLast(OGRE_VOLUME_SIDE_LENGTH-1)
		,mYRegionLast(OGRE_VOLUME_SIDE_LENGTH-1)
		,mZRegionLast(OGRE_VOLUME_SIDE_LENGTH-1)
		,mXRegionFirstBlock(0)
		,mYRegionFirstBlock(0)
		,mZRegionFirstBlock(0)
		,mXRegionLastBlock(OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS-1)
		,mYRegionLastBlock(OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS-1)
		,mZRegionLastBlock(OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS-1)
		,mXPosInVolume(0)
		,mYPosInVolume(0)
		,mZPosInVolume(0)
		,mXBlock(0)
		,mYBlock(0)
		,mZBlock(0)
		,mXPosInBlock(0)
		,mYPosInBlock(0)
		,mZPosInBlock(0)
		,mIsValidForRegion(true)
		,mCurrentVoxel(volume.mBlocks[0]->mData)
		//,mCurrentBlock(volume->mBlocks[0])
		,mVoxelIndexInBlock(0)
		,mBlockIndexInVolume(0)
	{
	}

	VolumeIterator::~VolumeIterator()
	{
	}	

	void VolumeIterator::setVoxel(uchar value)
	{
		Block* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];

		/*if(!currentBlock.unique())
		{
			//LogManager::getSingleton().logMessage("Copying Block");
			Block* copy(new Block(*currentBlock));
			currentBlock = copy;

			mCurrentVoxel = currentBlock->mData + mVoxelIndexInBlock;
		}*/

		*mCurrentVoxel = value;
	}

	uchar VolumeIterator::getVoxel(void)
	{
		//return getVoxelAt(mXPosInVolume,mYPosInVolume,mZPosInVolume);
		return *mCurrentVoxel;
	}

	uchar VolumeIterator::getVoxelAt(const uint xPosition, const uint yPosition, const uint zPosition) const
	{
		const uint blockX = xPosition >> OGRE_BLOCK_SIDE_LENGTH_POWER;
		const uint blockY = yPosition >> OGRE_BLOCK_SIDE_LENGTH_POWER;
		const uint blockZ = zPosition >> OGRE_BLOCK_SIDE_LENGTH_POWER;

		const uint xOffset = xPosition - (blockX << OGRE_BLOCK_SIDE_LENGTH_POWER);
		const uint yOffset = yPosition - (blockY << OGRE_BLOCK_SIDE_LENGTH_POWER);
		const uint zOffset = zPosition - (blockZ << OGRE_BLOCK_SIDE_LENGTH_POWER);

		const Block* block = mVolume.mBlocks
			[
				blockX + 
				blockY * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS + 
				blockZ * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS
			];

		return block->getVoxelAt(xOffset,yOffset,zOffset);
	}

	float VolumeIterator::getAveragedVoxelAt(const uint xPosition, const uint yPosition, const uint zPosition, uint size) const
	{
		float sum = 0.0;
		for(uint z = zPosition-size; z <= zPosition+size; ++z)
		{
			for(uint y = yPosition-size; y <= yPosition+size; ++y)
			{
				for(uint x = xPosition-size; x <= xPosition+size; ++x)
				{
					if(getVoxelAt(x,y,z) != 0)
					{
						sum += 1.0;
					}
				}
			}
		}

		uint kernelSideLength = size * 2 + 1;
		uint kernelVolume = kernelSideLength * kernelSideLength * kernelSideLength;

		sum /= static_cast<float>(kernelVolume);
		return sum;
	}

	void VolumeIterator::setVoxelAt(const uint xPosition, const uint yPosition, const uint zPosition, const uchar value)
		{
		const uint blockX = xPosition >> OGRE_BLOCK_SIDE_LENGTH_POWER;
		const uint blockY = yPosition >> OGRE_BLOCK_SIDE_LENGTH_POWER;
		const uint blockZ = zPosition >> OGRE_BLOCK_SIDE_LENGTH_POWER;

		const uint xOffset = xPosition - (blockX << OGRE_BLOCK_SIDE_LENGTH_POWER);
		const uint yOffset = yPosition - (blockY << OGRE_BLOCK_SIDE_LENGTH_POWER);
		const uint zOffset = zPosition - (blockZ << OGRE_BLOCK_SIDE_LENGTH_POWER);

		Block* block = mVolume.mBlocks
			[
				blockX + 
				blockY * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS + 
				blockZ * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS
			];

		/*if(!block.unique())
		{
			//LogManager::getSingleton().logMessage("Copying Block");
			Block* copy(new Block(*block));
			block = copy;

			mCurrentVoxel = block->mData + mVoxelIndexInBlock;
		}*/

		block->setVoxelAt(xOffset,yOffset,zOffset, value);
	}

	Vector3 VolumeIterator::getCentralDifferenceGradient(void) const
	{
		//FIXME - should this test be here?
		if((mXPosInVolume < 1) || (mXPosInVolume > OGRE_VOLUME_SIDE_LENGTH-2) ||
			(mYPosInVolume < 1) || (mYPosInVolume > OGRE_VOLUME_SIDE_LENGTH-2) ||
			(mZPosInVolume < 1) || (mZPosInVolume > OGRE_VOLUME_SIDE_LENGTH-2))
		{
			//LogManager::getSingleton().logMessage("Out of range");
			return Vector3(0.0,0.0,0.0);
		}

		//FIXME - bitwise way of doing this?
		uchar voxel1nx = peekVoxel1nx0py0pz() > 0 ? 1: 0;
		uchar voxel1px = peekVoxel1px0py0pz() > 0 ? 1: 0;

		uchar voxel1ny = peekVoxel0px1ny0pz() > 0 ? 1: 0;
		uchar voxel1py = peekVoxel0px1py0pz() > 0 ? 1: 0;

		uchar voxel1nz = peekVoxel0px0py1nz() > 0 ? 1: 0;
		uchar voxel1pz = peekVoxel0px0py1pz() > 0 ? 1: 0;

		return Vector3(int(voxel1px) - int(voxel1nx),int(voxel1py) - int(voxel1ny),int(voxel1pz) - int(voxel1nz));
	}

	Vector3 VolumeIterator::getAveragedCentralDifferenceGradient(void) const
	{
		//FIXME - should this test be here?
		if((mXPosInVolume < 2) || (mXPosInVolume > OGRE_VOLUME_SIDE_LENGTH-3) ||
			(mYPosInVolume < 2) || (mYPosInVolume > OGRE_VOLUME_SIDE_LENGTH-3) ||
			(mZPosInVolume < 2) || (mZPosInVolume > OGRE_VOLUME_SIDE_LENGTH-3))
		{
			//LogManager::getSingleton().logMessage("Out of range");
			return Vector3(0.0,0.0,0.0);
		}

		//FIXME - bitwise way of doing this?
		float voxel1nx = getAveragedVoxelAt(mXPosInVolume-1,mYPosInVolume  ,mZPosInVolume,  2);
		float voxel1px = getAveragedVoxelAt(mXPosInVolume+1,mYPosInVolume  ,mZPosInVolume,  2);

		float voxel1ny = getAveragedVoxelAt(mXPosInVolume  ,mYPosInVolume-1,mZPosInVolume,  2);
		float voxel1py = getAveragedVoxelAt(mXPosInVolume  ,mYPosInVolume+1,mZPosInVolume,  2);

		float voxel1nz = getAveragedVoxelAt(mXPosInVolume  ,mYPosInVolume  ,mZPosInVolume-1,2);
		float voxel1pz = getAveragedVoxelAt(mXPosInVolume  ,mYPosInVolume  ,mZPosInVolume+1,2);

		return Vector3(voxel1px - voxel1nx,voxel1py - voxel1ny,voxel1pz - voxel1nz);
	}

	Vector3 VolumeIterator::getSobelGradient(void) const
	{
		//FIXME - should this test be here?
		if((mXPosInVolume < 1) || (mXPosInVolume > OGRE_VOLUME_SIDE_LENGTH-2) ||
			(mYPosInVolume < 1) || (mYPosInVolume > OGRE_VOLUME_SIDE_LENGTH-2) ||
			(mZPosInVolume < 1) || (mZPosInVolume > OGRE_VOLUME_SIDE_LENGTH-2))
		{
			//LogManager::getSingleton().logMessage("Out of range");
			return Vector3(0.0,0.0,0.0);
		}

		static const int weights[3][3][3] = {  {  {2,3,2}, {3,6,3}, {2,3,2}  },  {
			{3,6,3},  {6,0,6},  {3,6,3} },  { {2,3,2},  {3,6,3},  {2,3,2} } };

			const uchar pVoxel1nx1ny1nz = peekVoxel1nx1ny1nz() > 0 ? 1: 0;
			const uchar pVoxel1nx1ny0pz = peekVoxel1nx1ny0pz() > 0 ? 1: 0;
			const uchar pVoxel1nx1ny1pz = peekVoxel1nx1ny1pz() > 0 ? 1: 0;
			const uchar pVoxel1nx0py1nz = peekVoxel1nx0py1nz() > 0 ? 1: 0;
			const uchar pVoxel1nx0py0pz = peekVoxel1nx0py0pz() > 0 ? 1: 0;
			const uchar pVoxel1nx0py1pz = peekVoxel1nx0py1pz() > 0 ? 1: 0;
			const uchar pVoxel1nx1py1nz = peekVoxel1nx1py1nz() > 0 ? 1: 0;
			const uchar pVoxel1nx1py0pz = peekVoxel1nx1py0pz() > 0 ? 1: 0;
			const uchar pVoxel1nx1py1pz = peekVoxel1nx1py1pz() > 0 ? 1: 0;

			const uchar pVoxel0px1ny1nz = peekVoxel0px1ny1nz() > 0 ? 1: 0;
			const uchar pVoxel0px1ny0pz = peekVoxel0px1ny0pz() > 0 ? 1: 0;
			const uchar pVoxel0px1ny1pz = peekVoxel0px1ny1pz() > 0 ? 1: 0;
			const uchar pVoxel0px0py1nz = peekVoxel0px0py1nz() > 0 ? 1: 0;
			//const uchar pVoxel0px0py0pz = peekVoxel0px0py0pz() > 0 ? 1: 0;
			const uchar pVoxel0px0py1pz = peekVoxel0px0py1pz() > 0 ? 1: 0;
			const uchar pVoxel0px1py1nz = peekVoxel0px1py1nz() > 0 ? 1: 0;
			const uchar pVoxel0px1py0pz = peekVoxel0px1py0pz() > 0 ? 1: 0;
			const uchar pVoxel0px1py1pz = peekVoxel0px1py1pz() > 0 ? 1: 0;

			const uchar pVoxel1px1ny1nz = peekVoxel1px1ny1nz() > 0 ? 1: 0;
			const uchar pVoxel1px1ny0pz = peekVoxel1px1ny0pz() > 0 ? 1: 0;
			const uchar pVoxel1px1ny1pz = peekVoxel1px1ny1pz() > 0 ? 1: 0;
			const uchar pVoxel1px0py1nz = peekVoxel1px0py1nz() > 0 ? 1: 0;
			const uchar pVoxel1px0py0pz = peekVoxel1px0py0pz() > 0 ? 1: 0;
			const uchar pVoxel1px0py1pz = peekVoxel1px0py1pz() > 0 ? 1: 0;
			const uchar pVoxel1px1py1nz = peekVoxel1px1py1nz() > 0 ? 1: 0;
			const uchar pVoxel1px1py0pz = peekVoxel1px1py0pz() > 0 ? 1: 0;
			const uchar pVoxel1px1py1pz = peekVoxel1px1py1pz() > 0 ? 1: 0;



			const int xGrad(- weights[0][0][0] * ( pVoxel1nx1ny1nz) -
				weights[1][0][0] * ( pVoxel1nx1ny0pz) - weights[2][0][0] *
				( pVoxel1nx1ny1pz) - weights[0][1][0] * ( pVoxel1nx0py1nz) -
				weights[1][1][0] * ( pVoxel1nx0py0pz) - weights[2][1][0] *
				( pVoxel1nx0py1pz) - weights[0][2][0] * ( pVoxel1nx1py1nz) -
				weights[1][2][0] * ( pVoxel1nx1py0pz) - weights[2][2][0] *
				( pVoxel1nx1py1pz) + weights[0][0][2] * ( pVoxel1px1ny1nz) +
				weights[1][0][2] * ( pVoxel1px1ny0pz) + weights[2][0][2] *
				( pVoxel1px1ny1pz) + weights[0][1][2] * ( pVoxel1px0py1nz) +
				weights[1][1][2] * ( pVoxel1px0py0pz) + weights[2][1][2] *
				( pVoxel1px0py1pz) + weights[0][2][2] * ( pVoxel1px1py1nz) +
				weights[1][2][2] * ( pVoxel1px1py0pz) + weights[2][2][2] *
				( pVoxel1px1py1pz));

			const int yGrad(- weights[0][0][0] * ( pVoxel1nx1ny1nz) -
				weights[1][0][0] * ( pVoxel1nx1ny0pz) - weights[2][0][0] *
				( pVoxel1nx1ny1pz) + weights[0][2][0] * ( pVoxel1nx1py1nz) +
				weights[1][2][0] * ( pVoxel1nx1py0pz) + weights[2][2][0] *
				( pVoxel1nx1py1pz) - weights[0][0][1] * ( pVoxel0px1ny1nz) -
				weights[1][0][1] * ( pVoxel0px1ny0pz) - weights[2][0][1] *
				( pVoxel0px1ny1pz) + weights[0][2][1] * ( pVoxel0px1py1nz) +
				weights[1][2][1] * ( pVoxel0px1py0pz) + weights[2][2][1] *
				( pVoxel0px1py1pz) - weights[0][0][2] * ( pVoxel1px1ny1nz) -
				weights[1][0][2] * ( pVoxel1px1ny0pz) - weights[2][0][2] *
				( pVoxel1px1ny1pz) + weights[0][2][2] * ( pVoxel1px1py1nz) +
				weights[1][2][2] * ( pVoxel1px1py0pz) + weights[2][2][2] *
				( pVoxel1px1py1pz));

			const int zGrad(- weights[0][0][0] * ( pVoxel1nx1ny1nz) +
				weights[2][0][0] * ( pVoxel1nx1ny1pz) - weights[0][1][0] *
				( pVoxel1nx0py1nz) + weights[2][1][0] * ( pVoxel1nx0py1pz) -
				weights[0][2][0] * ( pVoxel1nx1py1nz) + weights[2][2][0] *
				( pVoxel1nx1py1pz) - weights[0][0][1] * ( pVoxel0px1ny1nz) +
				weights[2][0][1] * ( pVoxel0px1ny1pz) - weights[0][1][1] *
				( pVoxel0px0py1nz) + weights[2][1][1] * ( pVoxel0px0py1pz) -
				weights[0][2][1] * ( pVoxel0px1py1nz) + weights[2][2][1] *
				( pVoxel0px1py1pz) - weights[0][0][2] * ( pVoxel1px1ny1nz) +
				weights[2][0][2] * ( pVoxel1px1ny1pz) - weights[0][1][2] *
				( pVoxel1px0py1nz) + weights[2][1][2] * ( pVoxel1px0py1pz) -
				weights[0][2][2] * ( pVoxel1px1py1nz) + weights[2][2][2] *
				( pVoxel1px1py1pz));

			return Vector3(xGrad,yGrad,zGrad);
	}

	uint VolumeIterator::getPosX(void)
	{
		return mXPosInVolume;
	}

	uint VolumeIterator::getPosY(void)
	{
		return mYPosInVolume;
	}

	uint VolumeIterator::getPosZ(void)
	{
		return mZPosInVolume;
	}

	void VolumeIterator::setPosition(uint xPos, uint yPos, uint zPos)
	{
		mXPosInVolume = xPos;
		mYPosInVolume = yPos;
		mZPosInVolume = zPos;

		mXBlock = mXPosInVolume >> OGRE_BLOCK_SIDE_LENGTH_POWER;
		mYBlock = mYPosInVolume >> OGRE_BLOCK_SIDE_LENGTH_POWER;
		mZBlock = mZPosInVolume >> OGRE_BLOCK_SIDE_LENGTH_POWER;

		mXPosInBlock = mXPosInVolume - (mXBlock << OGRE_BLOCK_SIDE_LENGTH_POWER);
		mYPosInBlock = mYPosInVolume - (mYBlock << OGRE_BLOCK_SIDE_LENGTH_POWER);
		mZPosInBlock = mZPosInVolume - (mZBlock << OGRE_BLOCK_SIDE_LENGTH_POWER);

		mBlockIndexInVolume = mXBlock + 
			mYBlock * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS + 
			mZBlock * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS;
		Block* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];

		mVoxelIndexInBlock = mXPosInBlock + 
			mYPosInBlock * OGRE_BLOCK_SIDE_LENGTH + 
			mZPosInBlock * OGRE_BLOCK_SIDE_LENGTH * OGRE_BLOCK_SIDE_LENGTH;

		mCurrentVoxel = currentBlock->mData + mVoxelIndexInBlock;
	}

	void VolumeIterator::setValidRegion(uint xFirst, uint yFirst, uint zFirst, uint xLast, uint yLast, uint zLast)
	{
		mXRegionFirst = xFirst;
		mYRegionFirst = yFirst;
		mZRegionFirst = zFirst;

		mXRegionLast = xLast;
		mYRegionLast = yLast;
		mZRegionLast = zLast;

		/*mXRegionFirst = (std::max)(0,xFirst);
		mYRegionFirst = (std::max)(0,yFirst);
		mZRegionFirst = (std::max)(0,zFirst);

		mXRegionLast = (std::min)(OGRE_VOLUME_SIDE_LENGTH-1, xLast);
		mYRegionLast = (std::min)(OGRE_VOLUME_SIDE_LENGTH-1, yLast);
		mZRegionLast = (std::min)(OGRE_VOLUME_SIDE_LENGTH-1, zLast);*/

		mXRegionFirstBlock = mXRegionFirst >> OGRE_BLOCK_SIDE_LENGTH_POWER;
		mYRegionFirstBlock = mYRegionFirst >> OGRE_BLOCK_SIDE_LENGTH_POWER;
		mZRegionFirstBlock = mZRegionFirst >> OGRE_BLOCK_SIDE_LENGTH_POWER;

		mXRegionLastBlock = mXRegionLast >> OGRE_BLOCK_SIDE_LENGTH_POWER;
		mYRegionLastBlock = mYRegionLast >> OGRE_BLOCK_SIDE_LENGTH_POWER;
		mZRegionLastBlock = mZRegionLast >> OGRE_BLOCK_SIDE_LENGTH_POWER;
	}

	void VolumeIterator::moveForwardInRegion(void)
	{
		mXPosInBlock++;
		mCurrentVoxel++;
		mXPosInVolume++;
		if((mXPosInBlock == OGRE_BLOCK_SIDE_LENGTH) || (mXPosInVolume > mXRegionLast))
		{
			mXPosInVolume = (std::max)(mXRegionFirst,mXBlock * OGRE_BLOCK_SIDE_LENGTH);
			mXPosInBlock = mXPosInVolume - (mXBlock << OGRE_BLOCK_SIDE_LENGTH_POWER);
			mVoxelIndexInBlock = mXPosInBlock + 
				mYPosInBlock * OGRE_BLOCK_SIDE_LENGTH + 
				mZPosInBlock * OGRE_BLOCK_SIDE_LENGTH * OGRE_BLOCK_SIDE_LENGTH;
			Block* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];
			mCurrentVoxel = currentBlock->mData + mVoxelIndexInBlock;

			mYPosInBlock++;
			mYPosInVolume++;
			mCurrentVoxel += OGRE_BLOCK_SIDE_LENGTH;
			if((mYPosInBlock == OGRE_BLOCK_SIDE_LENGTH) || (mYPosInVolume > mYRegionLast))
			{
				mYPosInVolume = (std::max)(mYRegionFirst,mYBlock * OGRE_BLOCK_SIDE_LENGTH);
				mYPosInBlock = mYPosInVolume - (mYBlock << OGRE_BLOCK_SIDE_LENGTH_POWER);
				mVoxelIndexInBlock = mXPosInBlock + 
					mYPosInBlock * OGRE_BLOCK_SIDE_LENGTH + 
					mZPosInBlock * OGRE_BLOCK_SIDE_LENGTH * OGRE_BLOCK_SIDE_LENGTH;
				Block* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];
				mCurrentVoxel = currentBlock->mData + mVoxelIndexInBlock;

				mZPosInBlock++;
				mZPosInVolume++;
				mCurrentVoxel += OGRE_BLOCK_SIDE_LENGTH * OGRE_BLOCK_SIDE_LENGTH;

				if((mZPosInBlock == OGRE_BLOCK_SIDE_LENGTH) || (mZPosInVolume > mZRegionLast))
				{
					//At this point we've left the current block. Find a new one...

					++mXBlock;
					++mBlockIndexInVolume;
					if(mXBlock > mXRegionLastBlock)
					{
						mXBlock = mXRegionFirstBlock;
						mBlockIndexInVolume = mXBlock + 
							mYBlock * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS + 
							mZBlock * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS;

						++mYBlock;
						mBlockIndexInVolume += OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS;
						if(mYBlock > mYRegionLastBlock)
						{
							mYBlock = mYRegionFirstBlock;
							mBlockIndexInVolume = mXBlock + 
								mYBlock * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS + 
								mZBlock * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS;

							++mZBlock;
							mBlockIndexInVolume += OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS;
							if(mZBlock > mZRegionLastBlock)
							{
								mIsValidForRegion = false;
								return;			
							}
						}
					}

					Block* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];
					//mCurrentBlock = mVolume->mBlocks[mBlockIndexInVolume];					

					mXPosInVolume = (std::max)(mXRegionFirst,mXBlock * OGRE_BLOCK_SIDE_LENGTH);					
					mYPosInVolume = (std::max)(mYRegionFirst,mYBlock * OGRE_BLOCK_SIDE_LENGTH);					
					mZPosInVolume = (std::max)(mZRegionFirst,mZBlock * OGRE_BLOCK_SIDE_LENGTH);					

					mXPosInBlock = mXPosInVolume - (mXBlock << OGRE_BLOCK_SIDE_LENGTH_POWER);
					mYPosInBlock = mYPosInVolume - (mYBlock << OGRE_BLOCK_SIDE_LENGTH_POWER);
					mZPosInBlock = mZPosInVolume - (mZBlock << OGRE_BLOCK_SIDE_LENGTH_POWER);

					mVoxelIndexInBlock = mXPosInBlock + 
						mYPosInBlock * OGRE_BLOCK_SIDE_LENGTH + 
						mZPosInBlock * OGRE_BLOCK_SIDE_LENGTH * OGRE_BLOCK_SIDE_LENGTH;

					mCurrentVoxel = currentBlock->mData + mVoxelIndexInBlock;
				}
			}
		}		
	}

	bool VolumeIterator::isValidForRegion(void)
	{
		return mIsValidForRegion;
	}

	uchar VolumeIterator::peekVoxel1nx1ny1nz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0) && (mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0) && (mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel - 1 - OGRE_BLOCK_SIDE_LENGTH - OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume-1);
	}

	uchar VolumeIterator::peekVoxel1nx1ny0pz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0) && (mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel - 1 - OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume);
	}

	uchar VolumeIterator::peekVoxel1nx1ny1pz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0) && (mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0) && (mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel - 1 - OGRE_BLOCK_SIDE_LENGTH + OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume+1);
	}

	uchar VolumeIterator::peekVoxel1nx0py1nz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0) && (mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel - 1 - OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume-1);
	}

	uchar VolumeIterator::peekVoxel1nx0py0pz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel - 1);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume);
	}

	uchar VolumeIterator::peekVoxel1nx0py1pz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0) && (mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel - 1 + OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume+1);
	}

	uchar VolumeIterator::peekVoxel1nx1py1nz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0) && (mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1) && (mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel - 1 + OGRE_BLOCK_SIDE_LENGTH - OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume-1);
	}

	uchar VolumeIterator::peekVoxel1nx1py0pz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0) && (mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel - 1 + OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume);
	}

	uchar VolumeIterator::peekVoxel1nx1py1pz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0) && (mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1) && (mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel - 1 + OGRE_BLOCK_SIDE_LENGTH + OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	uchar VolumeIterator::peekVoxel0px1ny1nz(void) const
	{
		if((mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0) && (mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel - OGRE_BLOCK_SIDE_LENGTH - OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume-1);
	}

	uchar VolumeIterator::peekVoxel0px1ny0pz(void) const
	{
		if((mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel - OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume);
	}

	uchar VolumeIterator::peekVoxel0px1ny1pz(void) const
	{
		if((mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0) && (mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel - OGRE_BLOCK_SIDE_LENGTH + OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume+1);
	}

	uchar VolumeIterator::peekVoxel0px0py1nz(void) const
	{
		if((mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel - OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume,mZPosInVolume-1);
	}

	uchar VolumeIterator::peekVoxel0px0py0pz(void) const
	{
			return *mCurrentVoxel;
	}

	uchar VolumeIterator::peekVoxel0px0py1pz(void) const
	{
		if((mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel + OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume,mZPosInVolume+1);
	}

	uchar VolumeIterator::peekVoxel0px1py1nz(void) const
	{
		if((mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1) && (mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel + OGRE_BLOCK_SIDE_LENGTH - OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume-1);
	}

	uchar VolumeIterator::peekVoxel0px1py0pz(void) const
	{
		if((mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel + OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume);
	}

	uchar VolumeIterator::peekVoxel0px1py1pz(void) const
	{
		if((mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1) && (mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel + OGRE_BLOCK_SIDE_LENGTH + OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	uchar VolumeIterator::peekVoxel1px1ny1nz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1) && (mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0) && (mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel + 1 - OGRE_BLOCK_SIDE_LENGTH - OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume-1);
	}

	uchar VolumeIterator::peekVoxel1px1ny0pz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1) && (mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel + 1 - OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume);
	}

	uchar VolumeIterator::peekVoxel1px1ny1pz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1) && (mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0) && (mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel + 1 - OGRE_BLOCK_SIDE_LENGTH + OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume+1);
	}

	uchar VolumeIterator::peekVoxel1px0py1nz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1) && (mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel + 1 - OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume-1);
	}

	uchar VolumeIterator::peekVoxel1px0py0pz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel + 1);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume);
	}

	uchar VolumeIterator::peekVoxel1px0py1pz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1) && (mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel + 1 + OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume+1);
	}

	uchar VolumeIterator::peekVoxel1px1py1nz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1) && (mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1) && (mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel + 1 + OGRE_BLOCK_SIDE_LENGTH - OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume-1);
	}

	uchar VolumeIterator::peekVoxel1px1py0pz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1) && (mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel + 1 + OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume);
	}

	uchar VolumeIterator::peekVoxel1px1py1pz(void) const
	{
		if((mXPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1) && (mYPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1) && (mZPosInVolume%OGRE_BLOCK_SIDE_LENGTH != OGRE_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel + 1 + OGRE_BLOCK_SIDE_LENGTH + OGRE_BLOCK_SIDE_LENGTH*OGRE_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume+1);
	}
}
