// ApiHook.h
// Declaration and implementation of functions for hooking APIs in DLLs

#ifndef __APIHOOK_H
#define __APIHOOK_H

enum eAddressing
{
   eByName,
   eByOrdinal
};

struct cHookDescriptor
{
   eAddressing m_addr;
   LPCTSTR m_szModule,
      m_szFunction;
   DWORD m_dwOrdinal,
      m_pNewFunction,
      m_pOldFunction;
};

// Functions to aid hooking
#define MakePtr( cast, ptr, AddValue ) (cast)( (DWORD)(ptr)+(DWORD)(AddValue))

PIMAGE_IMPORT_DESCRIPTOR getNamedImportDescriptor( HMODULE hModule, LPCSTR szImportMod )
{
    PIMAGE_DOS_HEADER pDOSHeader = reinterpret_cast< PIMAGE_DOS_HEADER >( hModule );

    // Get the PE header.
    PIMAGE_NT_HEADERS pNTHeader = MakePtr( PIMAGE_NT_HEADERS, pDOSHeader, pDOSHeader->e_lfanew );

    // If there is no imports section, leave now.
    if( pNTHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_IMPORT ].VirtualAddress == NULL )
        return NULL;

    // Get the pointer to the imports section.
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = MakePtr ( PIMAGE_IMPORT_DESCRIPTOR, pDOSHeader, 
       pNTHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_IMPORT ].VirtualAddress );

    // Loop through the import module descriptors looking for the
    //  module whose name matches szImportMod.
    while( pImportDesc->Name != NULL )
    {
        PSTR szCurrMod = MakePtr( PSTR, pDOSHeader, pImportDesc->Name );
        if( stricmp( szCurrMod, szImportMod ) == 0 )
            // Found it.
            break;

        // Look at the next one.
        pImportDesc ++;
    }

    // If the name is NULL, then the module is not imported.
    if ( pImportDesc->Name == NULL )
        return ( NULL ) ;

    // All OK, Jumpmaster!
    return pImportDesc;

}

bool hookFunctions( cHookDescriptor *pHook, DWORD nCount, bool bHook )
{
   HMODULE hProcess = ::GetModuleHandle( NULL );
   for( cHookDescriptor *i = pHook; i != pHook + nCount; ++ i )
   {
      // Get the specific import descriptor.
      PIMAGE_IMPORT_DESCRIPTOR pImportDesc = getNamedImportDescriptor( hProcess, i->m_szModule );

      if ( pImportDesc == NULL )
         continue;

      // Get the original thunk information for this DLL.  I cannot use
      //  the thunk information stored in the pImportDesc->FirstThunk
      //  because the that is the array that the loader has already
      //  bashed to fix up all the imports.  This pointer gives us acess
      //  to the function names.
      PIMAGE_THUNK_DATA pOrigThunk = MakePtr( PIMAGE_THUNK_DATA, hProcess, pImportDesc->OriginalFirstThunk );

      // Get the array pointed to by the pImportDesc->FirstThunk.  This is
      //  where I will do the actual bash.
      PIMAGE_THUNK_DATA pRealThunk = MakePtr( PIMAGE_THUNK_DATA, hProcess, pImportDesc->FirstThunk );

      // Loop through and look for the one that matches the name.
      for( ; pOrigThunk->u1.Function != NULL; ++ pOrigThunk, ++ pRealThunk )
      {
         if( i->m_addr == eByName )
         {
            if( pOrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG )
               // Only look at those that are imported by name, not ordinal.
               continue;

            // Look get the name of this imported function.
            PIMAGE_IMPORT_BY_NAME pByName = MakePtr( PIMAGE_IMPORT_BY_NAME, hProcess, pOrigThunk->u1.AddressOfData );

            // If the name starts with NULL, then just skip out now.
            if( pByName->Name[ 0 ] == '\0' )
               continue;

            if ( ::_tcsicmp( reinterpret_cast< char * >( pByName->Name ), i->m_szFunction ) != 0 )
               // This name dosen't match
               continue;
         }
         else
         {
            if( !( pOrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG ) )
               // The import must be by ordinal
               continue;

            if( ( pOrigThunk->u1.Ordinal & ~IMAGE_ORDINAL_FLAG ) != i->m_dwOrdinal )
               // Ordinal does not match
               continue;
         }

         // I found it.  Now I need to change the protection to
         //  writable before I do the blast.  Note that I am now
         //  blasting into the real thunk area!
         MEMORY_BASIC_INFORMATION mbi_thunk;

         ::VirtualQuery( pRealThunk, &mbi_thunk, sizeof ( MEMORY_BASIC_INFORMATION ) );
         ::VirtualProtect( mbi_thunk.BaseAddress, mbi_thunk.RegionSize, PAGE_READWRITE, &mbi_thunk.Protect );

         // Save the original address if requested.
         if( bHook )
         {
            i->m_pOldFunction = (ULONG) pRealThunk->u1.Function;
            pRealThunk->u1.Function = (ULONG *) i->m_pNewFunction;
         }
         else if( i->m_pOldFunction != NULL )
            pRealThunk->u1.Function = (ULONG *) i->m_pOldFunction;

         DWORD dwOldProtect;

         ::VirtualProtect( mbi_thunk.BaseAddress, mbi_thunk.RegionSize, mbi_thunk.Protect, &dwOldProtect );

         break;
      }
   }

   return true;
}

#endif
