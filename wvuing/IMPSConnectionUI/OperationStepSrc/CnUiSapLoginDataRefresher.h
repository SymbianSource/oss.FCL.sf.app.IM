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
* Description:  Login data refresher.
*
*/

#ifndef __CNUISAPLOGINDATAREFRESHER_H
#define __CNUISAPLOGINDATAREFRESHER_H

// INCLUDE FILES
#include <E32std.h>

//FORWARD DECLARATION
class CIMPSSAPSettings;
class MCnUiUiFacade;


// CLASS DECLARATION
/**
 * Login data refresher.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CnUiSapLoginDataRefresher )
    {
public: // New methods

    /**
     * Login data refresher.
     * Refreshes the user login data to SAP using the
     * UI facade.
     *
     * @since 2.1
     * @param aUi The UI to use.
     * @param aSap The SAP for which to refresh.
     * @param aReEdi EFalse if this is initial edit,
     * else this is a re-edit (ETrue).
     * @return ETrue if user accepted the login dat edit.
     * EFalse if user canceled the edit.
     */
    TBool static RefreshLoginDataL( MCnUiUiFacade& aUi,
                                    CIMPSSAPSettings& aSap,
                                    TBool aReEdit );


private: // prohibited constructor / destructor
    CnUiSapLoginDataRefresher();
    ~CnUiSapLoginDataRefresher();
    };


#endif //__CNUISAPLOGINDATAREFRESHER_H

//  End of File

