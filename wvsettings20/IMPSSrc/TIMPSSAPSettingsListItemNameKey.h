/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Key for sorting CIMPSSAPSettingsListItem's.
*
*/

#ifndef __TIMPSSAPSETTINGSLISTITEMNAMEKEY_H
#define __TIMPSSAPSETTINGSLISTITEMNAMEKEY_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION
/**
 * A key for sorting CIMPSSAPSettingsListItem's according to their name.
 *
 * @lib IMPSSAPSettingsStore.dll
 * @since 1.2
 */
NONSHARABLE_CLASS( TIMPSSAPSettingsListItemNameKey : public TKeyArrayFix )
    {
public:  // Constructors and destructor

    /**
     * C++ constructor.
     */
    TIMPSSAPSettingsListItemNameKey( TBool aAscending = ETrue );

public: //from TKeyArrayFix

    /**
     * Compares the keys of two array elements.
     * Comparison is done based the name value of CIMPSSAPSettingsListItems.
     * If aAscending is defined in constructor, comparisson is done
     * ascending, else descending.
     * @param aLeft Left index to compare.
     * @param aRight Right indexe to compare.
     *
     * @since 1.2
     */
    TInt Compare( TInt aLeft, TInt aRight ) const;

protected:  //data

    TBool                   iAscending;     ///< Sort order, owned

    };

#endif      // __TIMPSSAPSETTINGSLISTITEMNAMEKEY_H


//  End of File

