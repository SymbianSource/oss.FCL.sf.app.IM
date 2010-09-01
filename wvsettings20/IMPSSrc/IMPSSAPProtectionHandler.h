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
* Description:  SAP settings protection level handler.
*
*/

#ifndef __IMPSSAPPROTECTIONHANDLER_H
#define __IMPSSAPPROTECTIONHANDLER_H

//  INCLUDES
#include <e32base.h>
#include "impssapsettings.h"



// ENUMERATIONS
enum TIMPSSAPOperation
    {
    EOperationSetSAPName = KIMPSSAPProtectionNULLOperation + 1,
    EOperationSetSAPAddress,
    EOperationSetSAPPort,
    EOperationSetSAPUserId,
    EOperationSetSAPUserPassword,
    EOperationSetHTTPProxyAddress,
    EOperationSetHTTPProxyPort,
    EOperationSetHTTPProxyUserId,
    EOperationSetHTTPProxyUserPassword,
    EOperationSetAP,
    EOperationSetClientId,
    EOperationSetServerAuthenticationName,
    EOperationSetServerAuthenticationPassword,
    EOperationSetServerAcceptedContentType,
    EOperationDeleteSAP,
    EOperationSetProtection,
    EOperationSetHighLevelServices,
    EOperationSetAuthorizationMode
    };


// CLASS DECLARATION
/**
 *SAP protection handler.
 *
 * @lib IMPSSAPSettingsStore.dll
 * @since 3.0
 */
NONSHARABLE_CLASS( IMPSSAPProtectionHandler )
    {
public: // new functions

    /**
     * Validates is the requested operation allowd on
     * given protection level.
     *
     * @param aProtection The used protection level.
     * @param aRequestedOperation The requested operation.
     * @return KErrNone if operation is allowed. If operation
     * isn't allowed returns KErrAccessDenied.
     *
     * @since 3.0
     */
    static TInt ValidateOperation( TIMPSSAPProtection aProtection,
                                   TInt aRequestedOperation );

    /**
     * Validates is the requested operation allowd on
     * given protection level. Returns if operation is allowed.
     * If operation isn't allowed leaves with KErrAccessDenied.
     *
     * @param aProtection The used protection level.
     * @param aRequestedOperation The requested operation.
     *
     * @since 3.0
     */
    static void ValidateOperationL( TIMPSSAPProtection aProtection,
                                    TInt aRequestedOperation );

    /**
     * Checks is passed integer OK protection level.
     * See TIMPSSAPProtection enumeration for currently
     * supported protection levels.
     *
     * Leaves with KErrArgument if protection level
     * isn't supported.
     * @param aProtectionEnumeration The protection level
     * enumeration to chechk.
     *
     * @since 3.0
     */
    static void ChechkIsProperProtectionLevelL( TInt32 aProtectionEnumeration );

private:  // protection level helpers

    /**
     * Checks if the requested operation is allowed on the
     * "Brand Protection" mode. All other operations are
     * allowed except following prohibited operations:
     *  - setting SAP name
     *  - setting SAP address
     *  - deleting SAP
     * @param aRequestedOperation Requested operation to chechk.
     *        One of enumerations from TIMPSSAPOperation.
     * @return ETrue if operation is allowed, else EFalse.
     */
    static TInt IsAllowedBrandProtectionOperation( TInt aRequestedOperation );

    };

#endif      // __IMPSSAPPROTECTIONHANDLER_H

//  End of File
