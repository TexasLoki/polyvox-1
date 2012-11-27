/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/

#ifndef __PolyVox_Region_H__
#define __PolyVox_Region_H__

#include "Impl/TypeDef.h"

#include "PolyVoxCore/Vector.h"

namespace PolyVox
{
	/// Represents a part of a Volume.
	////////////////////////////////////////////////////////////////////////////////
	/// Many operations in PolyVox are constrained to only part of a volume. For example, when running the surface extractors 
	/// it is unlikely that you will want to run it on the whole volume at once, as this will give a very large mesh which may 
	/// be too much to render. Instead you will probably want to run a surface extractor a number of times on different parts 
	/// of the volume, there by giving a number of meshes which can be culled and rendered seperately.
	///
	/// The Region class is used to define these parts (regions) of the volume. Essentially it consists of an upper and lower 
	/// bound which specify the range of voxels positions considered to be part of the region. Note that these bounds are 
	/// <em>inclusive</em>.
	///
	/// As well as the expected set of getters and setters, this class also provide utility functions for increasing and decresing
	/// the size of the Region, shifting the Region in 3D space, testing whether it contains a given position, enlarging it so that
	/// it does contain a given position, croppng it to another Region, and various other utility functions.
	///
	/// \Note The dimensions of a region can be measured either in voxels or in cells. See the manual for more information 
	/// about these definitions.
	///
	////////////////////////////////////////////////////////////////////////////////
#ifdef SWIG
	class Region
#else
	class POLYVOX_API Region
#endif
	{
	public:

		/// A Region with the lower corner set as low as possible and the upper corner set as high as possible.
		static const Region MaxRegion;
		/// A Region with the lower corner set as high as possible and the upper corner set as low as possible.
		static const Region InvertedRegion;

		/// Constructor
		Region();
		/// Constructor
		Region(const Vector3DInt32& v3dLowerCorner, const Vector3DInt32& v3dUpperCorner);
		/// Constructor
		Region(int32_t iLowerX, int32_t iLowerY, int32_t iLowerZ, int32_t iUpperX, int32_t iUpperY, int32_t iUpperZ);

		/// Equality Operator.
        bool operator==(const Region& rhs) const;
		/// Inequality Operator.
        bool operator!=(const Region& rhs) const;

		/// Gets the 'x' position of the lower corner.
		int32_t getLowerX(void) const;
		/// Gets the 'y' position of the lower corner.
		int32_t getLowerY(void) const;
		/// Gets the 'z' position of the lower corner.
		int32_t getLowerZ(void) const;
		/// Gets the 'x' position of the upper corner.
		int32_t getUpperX(void) const;
		/// Gets the 'y' position of the upper corner.
		int32_t getUpperY(void) const;
		/// Gets the 'z' position of the upper corner.
		int32_t getUpperZ(void) const;

		/// Gets the position of the lower corner.
		Vector3DInt32 getLowerCorner(void) const;
		/// Gets the position of the upper corner.
		Vector3DInt32 getUpperCorner(void) const;

		/// Gets the width of the region measured in voxels.
		int32_t getWidthInVoxels(void) const;
		/// Gets the height of the region measured in voxels.
		int32_t getHeightInVoxels(void) const;
		/// Gets the depth of the region measured in voxels.
		int32_t getDepthInVoxels(void) const;
		/// Gets the dimensions of the region measured in voxels.
		Vector3DInt32 getDimensionsInVoxels(void) const;

		/// Gets the width of the region measured in cells.
		int32_t getWidthInCells(void) const;
		/// Gets the height of the region measured in cells.
		int32_t getHeightInCells(void) const;
		/// Gets the depth of the region measured in cells.
		int32_t getDepthInCells(void) const;
		/// Gets the dimensions of the region measured in cells.
		Vector3DInt32 getDimensionsInCells(void) const;

		/// Sets the 'x' position of the lower corner.
		void setLowerX(int32_t iX);
		/// Sets the 'y' position of the lower corner.
		void setLowerY(int32_t iY);
		/// Sets the 'z' position of the lower corner.
		void setLowerZ(int32_t iZ);
		/// Sets the 'x' position of the upper corner.
		void setUpperX(int32_t iX);
		/// Sets the 'y' position of the upper corner.
		void setUpperY(int32_t iY);
		/// Sets the 'z' position of the upper corner.
		void setUpperZ(int32_t iZ);

		/// Sets the position of the lower corner.
		void setLowerCorner(const Vector3DInt32& v3dLowerCorner);
		/// Sets the position of the upper corner.
		void setUpperCorner(const Vector3DInt32& v3dUpperCorner);

		/// Tests whether the given point is contained in this Region.
		bool containsPoint(const Vector3DFloat& pos, float boundary = 0.0f) const;
		/// Tests whether the given point is contained in this Region.
		bool containsPoint(const Vector3DInt32& pos, uint8_t boundary = 0) const;
		/// Tests whether the given position is contained in the 'x' range of this Region.
		bool containsPointInX(float pos, float boundary = 0.0f) const;
		/// Tests whether the given position is contained in the 'x' range of this Region.
		bool containsPointInX(int32_t pos, uint8_t boundary = 0) const;
		/// Tests whether the given position is contained in the 'y' range of this Region.
		bool containsPointInY(float pos, float boundary = 0.0f) const;
		/// Tests whether the given position is contained in the 'y' range of this Region.
		bool containsPointInY(int32_t pos, uint8_t boundary = 0) const;
		/// Tests whether the given position is contained in the 'z' range of this Region.
		bool containsPointInZ(float pos, float boundary = 0.0f) const;
		/// Tests whether the given position is contained in the 'z' range of this Region.
		bool containsPointInZ(int32_t pos, uint8_t boundary = 0) const;

		/// Enlarges the Region so that it contains the specified position.
		void accumulate(int32_t iX, int32_t iY, int32_t iZ);
		/// Enlarges the Region so that it contains the specified position.
		void accumulate(const Vector3DInt32& v3dPos);
		/// Enlarges the Region so that it contains the specified Region.
		void accumulate(const Region& reg);

		/// Crops the extents of this Region accoring to another Region.
		void cropTo(const Region& other);

		/// Grows this region by the amount specified.
		void grow(int32_t iAmount);
		/// Grows this region by the amounts specified.
		void grow(int32_t iAmountX, int32_t iAmountY, int32_t iAmountZ);
		/// Grows this region by the amounts specified.
		void grow(const Vector3DInt32& v3dAmount);

		/// Tests whether all components of the upper corner are at least
		/// as great as the corresponding components of the lower corner.
		bool isValid(void) const;

		/// Moves the Region by the amount specified.
		void shift(int32_t iAmountX, int32_t iAmountY, int32_t iAmountZ);
		/// Moves the Region by the amount specified.
		void shift(const Vector3DInt32& v3dAmount);
		/// Moves the lower corner of the Region by the amount specified.
		void shiftLowerCorner(int32_t iAmountX, int32_t iAmountY, int32_t iAmountZ);
		/// Moves the lower corner of the Region by the amount specified.
		void shiftLowerCorner(const Vector3DInt32& v3dAmount);
		/// Moves the upper corner of the Region by the amount specified.
		void shiftUpperCorner(int32_t iAmountX, int32_t iAmountY, int32_t iAmountZ);
		/// Moves the upper corner of the Region by the amount specified.
		void shiftUpperCorner(const Vector3DInt32& v3dAmount);		

		/// Shrinks this region by the amount specified.
		void shrink(int32_t iAmount);
		/// Shrinks this region by the amounts specified.
		void shrink(int32_t iAmountX, int32_t iAmountY, int32_t iAmountZ);
		/// Shrinks this region by the amounts specified.
		void shrink(const Vector3DInt32& v3dAmount);

	private:
		int32_t m_iLowerX;
		int32_t m_iLowerY;
		int32_t m_iLowerZ;
		int32_t m_iUpperX;
		int32_t m_iUpperY;
		int32_t m_iUpperZ;
	};

	// Functions to be inlined to to be in the header rather than the .cpp.
	// 'inline' keyword is used for the definition rather than the declaration.
	// See also http://www.parashift.com/c++-faq-lite/inline-functions.html

	////////////////////////////////////////////////////////////////////////////////
	/// \return The 'x' position of the lower corner.
	////////////////////////////////////////////////////////////////////////////////
	inline int32_t Region::getLowerX(void) const
	{
		return m_iLowerX;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The 'y' position of the lower corner.
	////////////////////////////////////////////////////////////////////////////////
	inline int32_t Region::getLowerY(void) const
	{
		return m_iLowerY;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The 'z' position of the lower corner.
	////////////////////////////////////////////////////////////////////////////////
	inline int32_t Region::getLowerZ(void) const
	{
		return m_iLowerZ;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The 'x' position of the upper corner.
	////////////////////////////////////////////////////////////////////////////////
	inline int32_t Region::getUpperX(void) const
	{
		return m_iUpperX;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The 'y' position of the upper corner.
	////////////////////////////////////////////////////////////////////////////////
	inline int32_t Region::getUpperY(void) const
	{
		return m_iUpperY;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The 'z' position of the upper corner.
	////////////////////////////////////////////////////////////////////////////////
	inline int32_t Region::getUpperZ(void) const
	{
		return m_iUpperZ;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The position of the lower corner.
	////////////////////////////////////////////////////////////////////////////////
	inline Vector3DInt32 Region::getLowerCorner(void) const
	{
		return Vector3DInt32(m_iLowerX, m_iLowerY, m_iLowerZ);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The position of the upper corner.
	////////////////////////////////////////////////////////////////////////////////
	inline Vector3DInt32 Region::getUpperCorner(void) const
	{
		return Vector3DInt32(m_iUpperX, m_iUpperY, m_iUpperZ);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The width of the region measured in voxels.
	/// \sa getWidthInCells()
	////////////////////////////////////////////////////////////////////////////////
	inline int32_t Region::getWidthInVoxels(void) const
	{
		return getWidthInCells() + 1;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The height of the region measured in voxels.
	/// \sa getHeightInCells()
	////////////////////////////////////////////////////////////////////////////////
	inline int32_t Region::getHeightInVoxels(void) const
	{
		return getHeightInCells() + 1;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The depth of the region measured in voxels.
	/// \sa getDepthInCells()
	////////////////////////////////////////////////////////////////////////////////
	inline int32_t Region::getDepthInVoxels(void) const
	{
		return getDepthInCells() + 1;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The dimensions of the region measured in voxels.
	/// \sa getDimensionsInCells()
	////////////////////////////////////////////////////////////////////////////////
	inline Vector3DInt32 Region::getDimensionsInVoxels(void) const
	{
		return getDimensionsInCells() + Vector3DInt32(1, 1, 1);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The width of the region measured in cells.
	/// \sa getWidthInVoxels()
	////////////////////////////////////////////////////////////////////////////////
	inline int32_t Region::getWidthInCells(void) const
	{
		return m_iUpperX - m_iLowerX;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The height of the region measured in cells.
	/// \sa getHeightInVoxels()
	////////////////////////////////////////////////////////////////////////////////
	inline int32_t Region::getHeightInCells(void) const
	{
		return m_iUpperY - m_iLowerY;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The depth of the region measured in cells.
	/// \sa getDepthInVoxels()
	////////////////////////////////////////////////////////////////////////////////
	inline int32_t Region::getDepthInCells(void) const
	{
		return m_iUpperZ - m_iLowerZ;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The dimensions of the region measured in cells.
	/// \sa getDimensionsInVoxels()
	////////////////////////////////////////////////////////////////////////////////
	inline Vector3DInt32 Region::getDimensionsInCells(void) const
	{
		return Vector3DInt32(getWidthInCells(), getHeightInCells(), getDepthInCells());
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param iX The new 'x' position of the lower corner.
	////////////////////////////////////////////////////////////////////////////////
	inline void Region::setLowerX(int32_t iX)
	{
		m_iLowerX = iX;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param iY The new 'y' position of the lower corner.
	////////////////////////////////////////////////////////////////////////////////
	inline void Region::setLowerY(int32_t iY)
	{
		m_iLowerY = iY;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param iZ The new 'z' position of the lower corner.
	////////////////////////////////////////////////////////////////////////////////
	inline void Region::setLowerZ(int32_t iZ)
	{
		m_iLowerZ = iZ;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param iX The new 'x' position of the upper corner.
	////////////////////////////////////////////////////////////////////////////////
	inline void Region::setUpperX(int32_t iX)
	{
		m_iUpperX = iX;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param iY The new 'y' position of the upper corner.
	////////////////////////////////////////////////////////////////////////////////
	inline void Region::setUpperY(int32_t iY)
	{
		m_iUpperY = iY;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param iZ The new 'z' position of the upper corner.
	////////////////////////////////////////////////////////////////////////////////
	inline void Region::setUpperZ(int32_t iZ)
	{
		m_iUpperZ = iZ;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dLowerCorner The new position of the lower corner.
	////////////////////////////////////////////////////////////////////////////////
	inline void Region::setLowerCorner(const Vector3DInt32& v3dLowerCorner)
	{
		m_iLowerX = v3dLowerCorner.getX();
		m_iLowerY = v3dLowerCorner.getY();
		m_iLowerZ = v3dLowerCorner.getZ();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dUpperCorner The new position of the upper corner.
	////////////////////////////////////////////////////////////////////////////////
	inline void Region::setUpperCorner(const Vector3DInt32& v3dUpperCorner)
	{
		m_iUpperX = v3dUpperCorner.getX();
		m_iUpperY = v3dUpperCorner.getY();
		m_iUpperZ = v3dUpperCorner.getZ();
	}
}

#endif
