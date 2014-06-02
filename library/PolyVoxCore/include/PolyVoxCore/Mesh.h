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

#ifndef __PolyVox_Mesh_H__
#define __PolyVox_Mesh_H__

#include "Impl/TypeDef.h"

#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/Vertex.h" //Should probably do away with this on in the future...

#include <algorithm>
#include <cstdlib>
#include <list>
#include <memory>
#include <set>
#include <vector>

namespace PolyVox
{
	class LodRecord
	{
	public:
		int beginIndex;
		int endIndex; //Let's put it just past the end STL style
	};

	template <typename _VertexType>
	class Mesh
	{
	public:

		typedef _VertexType VertexType;

	   Mesh();
	   ~Mesh();	   

	   const std::vector<uint32_t>& getIndices(void) const;
	   uint32_t getNoOfIndices(void) const;
	   uint32_t getNoOfNonUniformTrianges(void) const;
	   uint32_t getNoOfUniformTrianges(void) const;
	   uint32_t getNoOfVertices(void) const;	   
	   std::vector<VertexType>& getRawVertexData(void); //FIXME - this should be removed
	   const std::vector<VertexType>& getVertices(void) const;

	   void addTriangle(uint32_t index0, uint32_t index1, uint32_t index2);
	   void addTriangleCubic(uint32_t index0, uint32_t index1, uint32_t index2);
	   uint32_t addVertex(const VertexType& vertex);
	   void clear(void);
	   bool isEmpty(void) const;

	   int noOfDegenerateTris(void);
	   void removeDegenerateTris(void);
	   void removeUnusedVertices(void);

	   Region m_Region;
	
	public:		
		std::vector<uint32_t> m_vecTriangleIndices;
		std::vector<VertexType> m_vecVertices;

		std::vector<LodRecord> m_vecLodRecords;
	};

	template <typename MeshType>
	Mesh< Vertex< typename MeshType::VertexType::DataType > > decode(const MeshType& mesh)
	{
		Mesh< Vertex< typename MeshType::VertexType::DataType > > result;
		result.m_vecVertices.resize(mesh.m_vecVertices.size());

		for(uint32_t ct = 0; ct < mesh.m_vecVertices.size(); ct++)
		{
			result.m_vecVertices[ct] = decode(mesh.m_vecVertices[ct]);
		}

		result.m_vecTriangleIndices = mesh.m_vecTriangleIndices;

		result.m_Region = mesh.m_Region;

		return result;
	}
}

#include "PolyVoxCore/Mesh.inl"

#endif /* __Mesh_H__ */
