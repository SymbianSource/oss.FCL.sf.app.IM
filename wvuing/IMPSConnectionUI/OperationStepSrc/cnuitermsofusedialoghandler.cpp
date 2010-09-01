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


#include "cnuitermsofusedialoghandler.h"

#include <cimpssapsettingsstore.h>
#include <cimpssapsettings.h>
#include <centralrepository.h>  // for KMaxUnicodeStringLength

#include "mcnuiuifacade.h"

// Difference between KMaxUnicodeStringLength and CIMPSSAPSettings
// opaque data length, see cimpssapsettings.h SetOpaqueDesC16 method description.
const TInt KSapSettingsDataBufferLength = 5;
// Collation level 1 to ignore case
const TInt KCollationLevel = 1;

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Finds aId from aIdList. aIdList contains dot separated ids.
// Returns KErrNone if aId is found, KErrNotFound otherwise.
// ---------------------------------------------------------------------------
//
TInt FindId( const TDesC& aId, const TDesC& aIdList )
    {
    // Get pointer descriptor to work with
    TPtrC ptr( aIdList );
    // Locate first dot
    TInt dotPos = ptr.FindC( KIMDot );

    while ( dotPos != KErrNotFound )
        {
        // Get current id from list for comparison
        TPtrC curId( ptr.Left( dotPos ) );

        if ( curId.CompareC( aId, KCollationLevel, NULL ) == 0 )
            {
            // Match found, return
            return KErrNone;
            }
        else
            {
            // Move pointer to next id in list
            TInt len = ptr.Length() - dotPos - 1;   // -1 to get rid of dot
            if ( len < 0 )
                {
                // This should never happen but let's check
                // just to be sure to avoid panics
                return KErrNotFound;
                }
            ptr.Set( ptr.Right( len ) );
            // Locate next dot
            dotPos = ptr.FindC( KIMDot );
            }
        }

    // Match not found
    return KErrNotFound;
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CnUiTermsOfUseDialogHandler::HandleTermsOfUseDialogL
// ---------------------------------------------------------------------------
//
TInt CnUiTermsOfUseDialogHandler::HandleTermsOfUseDialogL(
    CIMPSSAPSettings& aSap,
    MCnUiUiFacade& aUi,
    CIMPSSAPSettingsStore& aSapStore )
    {
    TInt sapError = KErrNone;
    TPtrC16 acceptedIds;

    // Get SAP settings from SAP store to work with
    CIMPSSAPSettings* tmpSap = CIMPSSAPSettings::NewLC();
    TBool tmpSapOwned = ETrue;
    TRAPD( err, aSapStore.GetSAPL( aSap.Uid(), tmpSap ) );
    if ( err != KErrNone )
        {
        // In case of error use aSap directly
        CleanupStack::PopAndDestroy( tmpSap );
        tmpSap = NULL;
        tmpSap = &aSap;
        tmpSapOwned = EFalse;
        }

    // Get pointer descriptor to ids that have accepted ToU from SAP
    sapError = tmpSap->GetOpaqueDesC16( KIMToUAccepted, acceptedIds );

    // Ignore KErrNotFound errors (means that
    // this is the first login attempt to this SAP)
    if ( sapError != KErrNotFound )
        {
        User::LeaveIfError( sapError );
        }

    // Get current user id from aSap
    const TDesC& currentId( aSap.SAPUserId() );

    // Check if ToU has already been accepted for current id
    TInt retVal = FindId( currentId, acceptedIds );

    if ( retVal == KErrNotFound )
        {
        // Show Terms of Use, return KErrCancel if user does not accept them,
        // otherwise continue
        if ( !aUi.DisplayTermsOfUseAndQueryL() )
            {
            // User canceled ToU query, login cancelled
            // note is already shown inside DisplayTermsOfUseAndQueryL
            // method, just return KErrCancel
            if ( tmpSapOwned )
                {
                // Cleanup needed
                CleanupStack::PopAndDestroy( tmpSap );
                }
            return KErrCancel;
            }
        else
            {
            // ToU accepted, set it shown
            // for this user id
            HBufC* newAcceptedIds = HBufC::NewLC(
                                        acceptedIds.Length() +
                                        KIMDot().Length() +
                                        currentId.Length() );
            TPtr ids( newAcceptedIds->Des() );

            // Append previous user ids if any
            if ( acceptedIds.Length() > 0 )
                {
                ids.Append( acceptedIds );
                }
            // Append current id
            ids.Append( currentId );
            // Append dot as separator
            ids.Append( KIMDot );

            // Check key+value length
            TInt keyLen = KIMToUAccepted().Length();
            TInt newLen = ids.Length() + keyLen;
            TInt maxLen = NCentralRepositoryConstants::KMaxUnicodeStringLength -
                          KSapSettingsDataBufferLength;

            // if key+value length too big, delete
            // previous ids from start of list
            // until length is small enough
            while ( newLen > maxLen )
                {
                // Find first dot
                TInt dotPos = ids.FindC( KIMDot );
                if ( dotPos == KErrNotFound )
                    {
                    // No dot => data corrupted, all we can do
                    // is to delete previous key+value
                    // pair and set new one with only current id
                    tmpSap->DeleteOpaque( KIMToUAccepted );
                    ids.Zero();
                    ids.Append( currentId );
                    newLen = ids.Length() + keyLen;
                    }
                else
                    {
                    // Delete first id+dot from list
                    ids.Delete( 0, dotPos + 1 );
                    newLen = ids.Length() + keyLen;
                    }
                }

            // Set new ids to SAP
            sapError = tmpSap->SetOpaqueDesC16(
                           KIMToUAccepted, *newAcceptedIds );
            // Leave if errors
            User::LeaveIfError( sapError );

            // Cleanup
            CleanupStack::PopAndDestroy( newAcceptedIds );

            // Update SAP to store if it was succesfully
            // loaded from store
            if ( tmpSapOwned )
                {
                aSapStore.UpdateOldSAPL( tmpSap, aSap.Uid() );
                }
            }
        }

    if ( tmpSapOwned )
        {
        // Cleanup needed
        CleanupStack::PopAndDestroy( tmpSap );
        }

    return KErrNone;
    }
