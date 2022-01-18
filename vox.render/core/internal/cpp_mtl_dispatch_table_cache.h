//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_DispatchTableCache_h
#define cpp_mtl_DispatchTableCache_h

#include "cpp_mtl_allocator.h"
#include <Foundation/Foundation.h>

namespace cpp_mtl_Internal
{


using namespace MTL;

static const UInteger MaxDispatchTables = 10;

template <typename DispatchTableType>
class DispatchTableCache
{
    struct DispatchTableEntry
    {
        Class tableClass;
        DispatchTableType *table;
    } m_magazine[MaxDispatchTables];

    UInteger m_numTablesInMagazine;

    Allocator *m_allocator;

public:

    DispatchTableCache(Allocator & allocator) :
    m_numTablesInMagazine(0),
    m_allocator(&allocator)
    {

    }

    ~DispatchTableCache()
    {
        for(UInteger i = 0; i < m_numTablesInMagazine; i++)
        {
            destroy(*m_allocator, m_magazine[i].table );
        }
    }

    DispatchTableCache(const DispatchTableCache & rhs) = delete;
    DispatchTableCache & operator=(const DispatchTableCache & rhs) = delete;

    DispatchTableType *getTable(NSObject *objCObj)
    {
        // Attempt to find an existing table
        UInteger i;
        for(i = 0; i < m_numTablesInMagazine; i++)
        {
            if([objCObj isKindOfClass: m_magazine[i].tableClass])
            {
                return m_magazine[i].table;
            }
        }

        assert(i < MaxDispatchTables);

        // Create table and add it to list if one for the class does not exist
        m_magazine[i].table = construct<DispatchTableType>(*m_allocator, objCObj);
        m_magazine[i].tableClass = [objCObj class];
  
        m_numTablesInMagazine++;

        return m_magazine[i].table;
    }
};


} // namespace cpp_mtl_Internal

#endif // cpp_mtl_DispatchTableCache_h
