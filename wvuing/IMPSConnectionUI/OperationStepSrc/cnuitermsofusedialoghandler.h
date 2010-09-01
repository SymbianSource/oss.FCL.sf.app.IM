/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Static helper class for handling terms of use dialog
*
*/


#ifndef _CNUITERMSOFUSEDIALOGHANDLER_H
#define _CNUITERMSOFUSEDIALOGHANDLER_H

#include <e32std.h>

class CIMPSSAPSettings;
class MCnUiUiFacade;
class CIMPSSAPSettingsStore;

/**  Key to CIMPSSAPSettings opaque value */
_LIT( KIMToUAccepted, "KIMToUAccepted" );
/**  User id separator in KIMToUAccepted list */
_LIT( KIMDot, "," );

/**
 *  Static helper class for handling terms of use dialog.
 *
 *  @lib IMPSConnectionUi.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CnUiTermsOfUseDialogHandler )
    {

public:

    /**
     * Method used to handle terms of use (ToU) dialog.
     * Checks if current user id in aSap has
     * already accepted ToU or not. If not shows
     * ToU. Shows also "Login cancelled" note if user does not
     * accept ToU.
     *
     * @since S60 v3.1
     * @param aSap Current SAP settings.
     * @param aUi MCnUiUiFacade needed for showing ToU dialog.
     * @param aSapStore SAP settings store for updating SAP settings.
     * @return KErrCancel if user cancels ToU dialog, KErrNone if
     *         user accepts or has previously accepted ToU.
     */
    TInt static HandleTermsOfUseDialogL( CIMPSSAPSettings& aSap,
                                         MCnUiUiFacade& aUi,
                                         CIMPSSAPSettingsStore& aSapStore );

protected:

    // Prohibited constructor / destructor
    CnUiTermsOfUseDialogHandler();
    ~CnUiTermsOfUseDialogHandler();

    };

#endif // _CNUITERMSOFUSEDIALOGHANDLER_H
