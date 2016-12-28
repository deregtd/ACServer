// CRCs.h
// Declaration of classes related to CRCs

class SubTableEntry4
{
public:
   int m_nCounter;
   DWORD *m_pXORTable,
      *m_pUnkTable;
   DWORD m_dwArg0, m_dwArg1, m_dwArg2;

   SubTableEntry4 ()
      : m_pXORTable ( new DWORD[ 256 ] ),
      m_pUnkTable ( new DWORD[ 256 ] )
   {
   }
};

class SubTableEntry3
: public SubTableEntry4
{
public:
   virtual ~SubTableEntry3 ()
   {
   }

   virtual void FillOutTables2 ( SubTableEntry4 *pST4, bool bUseKeys )
   {
      DWORD xor0 = 0x9E3779B9,
         xor1 = xor0,
         xor2 = xor1,
         xor3 = xor2,
         xor4 = xor3,
         xor5 = xor4,
         xor6 = xor5,
         xor7 = xor6,
         *pUnkTable = pST4->m_pUnkTable,
         *pXORTable = pST4->m_pXORTable;

      if ( !bUseKeys )
      {
         pST4->m_dwArg0 = 0;
         pST4->m_dwArg1 = 0;
         pST4->m_dwArg2 = 0;
      }

      for ( int local_counter = 0; local_counter < 4; ++ local_counter )
         XOR_LOOP1 ( xor0, xor1, xor2, xor3, xor4, xor5, xor6, xor7 );

      if ( bUseKeys )
      {
         for ( local_counter = 0; local_counter < 256; local_counter += 8 )
         {
            xor7 += pXORTable[ local_counter ];
            xor6 += pXORTable[ local_counter + 1 ];
            xor5 += pXORTable[ local_counter + 2 ];
            xor4 += pXORTable[ local_counter + 3 ];
            xor3 += pXORTable[ local_counter + 4 ];
            xor2 += pXORTable[ local_counter + 5 ];
            xor1 += pXORTable[ local_counter + 6 ];
            xor0 += pXORTable[ local_counter + 7 ];

            XOR_LOOP1 ( xor0, xor1, xor2, xor3, xor4, xor5, xor6, xor7 );

            pUnkTable[ local_counter ] = xor7;
            pUnkTable[ local_counter + 1 ] = xor6;
            pUnkTable[ local_counter + 2 ] = xor5;
            pUnkTable[ local_counter + 3 ] = xor4;
            pUnkTable[ local_counter + 4 ] = xor3;
            pUnkTable[ local_counter + 5 ] = xor2;
            pUnkTable[ local_counter + 6 ] = xor1;
            pUnkTable[ local_counter + 7 ] = xor0;
         }

         for ( local_counter = 0; local_counter < 256; local_counter += 8 )
         {
            xor7 += pUnkTable[ local_counter ];
            xor6 += pUnkTable[ local_counter + 1 ];
            xor5 += pUnkTable[ local_counter + 2 ];
            xor4 += pUnkTable[ local_counter + 3 ];
            xor3 += pUnkTable[ local_counter + 4 ];
            xor2 += pUnkTable[ local_counter + 5 ];
            xor1 += pUnkTable[ local_counter + 6 ];
            xor0 += pUnkTable[ local_counter + 7 ];

            XOR_LOOP1 ( xor0, xor1, xor2, xor3, xor4, xor5, xor6, xor7 );

            pUnkTable[ local_counter ] = xor7;
            pUnkTable[ local_counter + 1 ] = xor6;
            pUnkTable[ local_counter + 2 ] = xor5;
            pUnkTable[ local_counter + 3 ] = xor4;
            pUnkTable[ local_counter + 4 ] = xor3;
            pUnkTable[ local_counter + 5 ] = xor2;
            pUnkTable[ local_counter + 6 ] = xor1;
            pUnkTable[ local_counter + 7 ] = xor0;
         }
      }
      else
      {
         for ( local_counter = 0; local_counter < 256; local_counter += 8 )
         {
            XOR_LOOP1 ( xor0, xor1, xor2, xor3, xor4, xor5, xor6, xor7 );

            pUnkTable[ local_counter ] = xor7;
            pUnkTable[ local_counter + 1 ] = xor6;
            pUnkTable[ local_counter + 2 ] = xor5;
            pUnkTable[ local_counter + 3 ] = xor4;
            pUnkTable[ local_counter + 4 ] = xor3;
            pUnkTable[ local_counter + 5 ] = xor2;
            pUnkTable[ local_counter + 6 ] = xor1;
            pUnkTable[ local_counter + 7 ] = xor0;
         }
      }

      FinalInit ();

      m_nCounter = 256;
   }

   virtual void FillOutTables ( int arg0, int arg1, int arg2, DWORD *init_vals )
   {
      int temp_int;
      for ( int local_counter = 0; local_counter < 256; ++ local_counter )
      {
         if ( init_vals != NULL )
            temp_int = init_vals[ local_counter ];
         else
            temp_int = 0;

         m_pXORTable[ local_counter ] = temp_int;
      }

      m_dwArg0 = arg0;
      m_dwArg1 = arg1;
      m_dwArg2 = arg2;

      FillOutTables2 ( this, true )
   }

   DWORD Crazy_XOR1 ( DWORD *pTable, DWORD dwIndex )
   {
      return pTable[ dwIndex & 0xFF ];
   }

   DWORD Crazy_XOR0 ( DWORD inputval, DWORD &xor0, DWORD &xor1, DWORD *&xor2, DWORD &xor3, DWORD &xor4, DWORD &xor5
      DWORD &xor6, DWORD &xor7 )
   {
      xor3 = xor6;
      xor0 = xor0 ^ inputval + xor4;
      xor4 += 4;
      xor7 = xor1 + xor0 + Crazy_XOR1 ( xor2, xor6 );

      *xor2 = xor7;
   }

   virtual void FinalInit ()
   {
   }

   virtual void XOR_LOOP1 ( DWORD &xor0, DWORD &xor1, DWORD &xor2, DWORD &xor3, DWORD &xor4,
      DWORD &xor5, DWORD &xor6, DWORD &xor7 )
   {
   }
};