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

#ifndef TPENGCNTLSTMDESCARRAYADAPTER_H__
#define TPENGCNTLSTMDESCARRAYADAPTER_H__

//  INCLUDES
#include <E32Std.h>
#include <BaDesCa.h>
#include <MPEngContactList2.h>


// CLASS DECLARATION
/**
 * Contact list to MDesCArray adapter.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( TPEngCntLstMDesCArrayAdapter ): public MDesCArray
    {
public: //Constructor & destructor

    /**
     * C++ constructor.
     * Takes the contact list and view type to adapt.
     */
    TPEngCntLstMDesCArrayAdapter( const MPEngContactList2& aList,
    TPEngContactListView aContactListView );


public: //From MDesCArray

    TInt MdcaCount() const;
    TPtrC MdcaPoint( TInt aIndex ) const;


private: //Data

    //OWN: Adapted contact list
    const MPEngContactList2&    iList;

    //OWN: Adapted contact list view
    TPEngContactListView    iContactListView;
    };


#endif      //TPENGCNTLSTMDESCARRAYADAPTER_H__
//  End of File


