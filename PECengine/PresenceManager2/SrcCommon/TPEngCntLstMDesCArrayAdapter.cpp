/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Contact list to MDesCArray adapter.
*
*/

//  INCLUDES
#include "TPEngCntLstMDesCArrayAdapter.h"
#include <E32Std.h>
#include <BaDesCa.h>
#include <MPEngContactList2.h>
#include <MPEngContactItem.h>


// ================= MEMBER FUNCTIONS =======================
// C++ default constructor can NOT contain any code, that
// might leave.
//
TPEngCntLstMDesCArrayAdapter::TPEngCntLstMDesCArrayAdapter( const MPEngContactList2& aList,
                                                            TPEngContactListView aContactListView )
        : iList( aList ),
        iContactListView( aContactListView )
    {
    }


// -----------------------------------------------------------------------------
// TPEngCntLstMDesCArrayAdapter::MdcaCount()
// -----------------------------------------------------------------------------
//
TInt TPEngCntLstMDesCArrayAdapter::MdcaCount() const
    {
    return iList.Count( iContactListView );
    }

// -----------------------------------------------------------------------------
// TPEngCntLstMDesCArrayAdapter::MdcaPoint()
// -----------------------------------------------------------------------------
//
TPtrC TPEngCntLstMDesCArrayAdapter::MdcaPoint( TInt aIndex ) const
    {
    return iList.ContactItem( aIndex, iContactListView ).Id();
    }


//  End of File


