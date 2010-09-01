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

// INCLUDE FILES
#include    <e32std.h>
#include    "IMPSSAPProtectionHandler.h"
#include    "IMPSSAPSettingsStoreDebugPrint.h"


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// IMPSSAPProtectionHandler::ValidateOperation()
// -----------------------------------------------------------------------------
//
TInt IMPSSAPProtectionHandler::ValidateOperation( TIMPSSAPProtection aProtection,
                                                  TInt aRequestedOperation )
    {
    TBool allowedOperation = ETrue;

    if ( ( aRequestedOperation == EOperationSetProtection ) &&
         ( aProtection != ESAPNoProtection ) )
        {
        allowedOperation = EFalse;
        }

    else
        {
        switch ( aProtection )
            {
            case ESAPBrandProtection:
                {
                allowedOperation = IsAllowedBrandProtectionOperation( aRequestedOperation );
                break;
                }


            case ESAPNoProtection:
            default:
                {
                //by default allow all operations
                break;
                }
            }
        }


    if ( !allowedOperation )
        {
        SSS_DP_TXT( "Error: SAP protection level doesn't allow operation." );
        return KErrAccessDenied;
        }

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// IMPSSAPProtectionHandler::ValidateOperationL()
// -----------------------------------------------------------------------------
//
void IMPSSAPProtectionHandler::ValidateOperationL( TIMPSSAPProtection aProtection,
                                                   TInt aRequestedOperation )
    {
    User::LeaveIfError( ValidateOperation( aProtection, aRequestedOperation ) );
    }



// -----------------------------------------------------------------------------
// IMPSSAPProtectionHandler::ChechkIsProperProtectionLevelL()
// -----------------------------------------------------------------------------
//
void IMPSSAPProtectionHandler::ChechkIsProperProtectionLevelL( TInt32 aProtectionEnumeration )
    {
    switch ( aProtectionEnumeration )
        {
        case ESAPNoProtection:
        case ESAPBrandProtection:
            {
            //supported protection levels
            break;
            }

        default:
            {
            //disallow all other arguments
            //deleting default, handle error by leaving
            User::Leave( KErrArgument );
            break;
            }
        }
    }



// -----------------------------------------------------------------------------
// IMPSSAPProtectionHandler::IsAllowedBrandProtectionOperation()
// BrandProtection protection level handler.
// -----------------------------------------------------------------------------
//
TBool IMPSSAPProtectionHandler::IsAllowedBrandProtectionOperation( TInt aRequestedOperation )
    {
    switch ( aRequestedOperation )
        {
        case EOperationSetSAPName:
            {
            //these operations are prohibited in branding protection
            return EFalse;
            }

        default:
            {
            //allow all other operations
            return ETrue;
            }
        }
    }


//  End of File
