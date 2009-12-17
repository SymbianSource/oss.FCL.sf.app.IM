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
* Description:  Presence notification handler.
*
*/

// INCLUDE FILES
#include "CPEngPresenceNotificationHandler.h"
#include "MPEngXMLParser.h"

#include "MPEngPresenceAdvancedAttrModel2.h"
#include "MPEngPresenceAttrManager.h"
#include "MPEngPresenceAttrTransactionEnv.h"
#include "PEngAddressUtils.h"
#include "PEngAttrModelHelper.h"
#include "PresenceDebugPrint.h"
#include "PEngWVPresenceAttributes2.h"

#include <E32Std.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngPresenceNotificationHandler::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngPresenceNotificationHandler* CPEngPresenceNotificationHandler::NewL(
    const TDesC& aDomain,
    TPEngWVCspVersion aCspVersion,
    MPEngPresenceAttrManager& aAttrManager,
    MPEngPresenceAttrTransactionEnv& aTransEnv )
    {
    CPEngPresenceNotificationHandler* self = new ( ELeave ) CPEngPresenceNotificationHandler(
        aAttrManager,
        aTransEnv,
        aCspVersion );

    CleanupStack::PushL( self );
    self->ConstructL( aDomain );
    CleanupStack::Pop(); //self

    return self;
    }



// Destructor
CPEngPresenceNotificationHandler::~CPEngPresenceNotificationHandler()
    {
    iPresenceDataBlocks.Reset();
    iModels.ResetAndDestroy();
    delete iXmlParser;
    delete iDomain;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotificationHandler::CPEngPresenceNotificationHandler
// C++ default constructor can NOT contain any code, that
// might leave.
//
// Useually only one or two users presence is updated at time ==> 2 blocks
// -----------------------------------------------------------------------------
//
CPEngPresenceNotificationHandler::CPEngPresenceNotificationHandler(
    MPEngPresenceAttrManager& aAttrManager,
    MPEngPresenceAttrTransactionEnv& aTransEnv,
    TPEngWVCspVersion aCspVersion )
        : iAttributeManager( aAttrManager ),
        iTransEnv( aTransEnv ),
        iCspVersion( aCspVersion ),
        iPresenceDataBlocks( 2 )
    {
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotificationHandler::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngPresenceNotificationHandler::ConstructL( const TDesC& aDomain )
    {
    iXmlParser = CreateXMLParserL();
    iDomain = aDomain.AllocL();
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotificationHandler::ProcessRequestL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceNotificationHandler::ProcessRequestL( const TDesC8& aRequest,
                                                        TRequestStatus& /*aStatus*/ )
    {
    PENG_DP_TXT( "CPEngPresenceNotificationHandler::ProcessRequestL()" );


    if ( !iXmlParser->DecodeL( aRequest,
                               KPresenceNotificationRequest,
                               ETrue ) )
        {
        return;
        }


    if ( iXmlParser->DecodeL( iXmlParser->ResultL(),
                              KPresenceXMLTag,
                              ETrue ) )
        {
        //Place found presence data blocks to an array
        const TInt foundPresencesCount = iXmlParser->Count();
        TInt presenceIx = 0;
        for ( presenceIx = 0; presenceIx < foundPresencesCount; presenceIx++ )
            {
            iPresenceDataBlocks.AppendL( iXmlParser->ResultL( presenceIx ) );
            }


        //And process each presence notify block one by one
        for ( presenceIx = 0; presenceIx < foundPresencesCount; presenceIx++ )
            {
            TPtrC8 notifyBlock = iPresenceDataBlocks[ presenceIx ];
            TBool matchedToSubscription = ProcessNotifyBlockL( notifyBlock,
                                                               *iDomain,
                                                               iTransEnv.SubscribedPresenceIDs(),
                                                               iAttributeManager,
                                                               *iXmlParser,
                                                               iModels );
            if ( !matchedToSubscription )
                {
                //notification wasn't matched to any subscription
                //que the orphan for later processing
                iTransEnv.QueOrphanNotificationL( notifyBlock );
                }
            }

        iAttributeManager.ForceStoreBatchL( iModels );
        iModels.ResetAndDestroy();
        }
    }



// -----------------------------------------------------------------------------
// CPEngPresenceNotificationHandler::CancelProcessing()
// -----------------------------------------------------------------------------
//
void CPEngPresenceNotificationHandler::CancelProcessing()
    {
    PENG_DP_TXT( "CPEngPresenceNotificationHandler::ProcessRequestL()" );
    }




// -----------------------------------------------------------------------------
// CPEngPresenceNotificationHandler::CanHandleL()
// -----------------------------------------------------------------------------
//
TBool CPEngPresenceNotificationHandler::CanHandleL( const TDesC8& aIncomingRequest )
    {
    PENG_DP_TXT( "CPEngPresenceNotificationHandler::CanHandleL()" );

    TBool retStatus( EFalse );
    MPEngXMLParser* xmlParser = CreateXMLParserLC();
    if ( xmlParser->DecodeL( aIncomingRequest,
                             KPresenceNotificationRequest,
                             EFalse ) )
        {
        //This is a presence notification,
        retStatus = ETrue;
        }

    CleanupStack::PopAndDestroy(); //xmlParser
    PENG_DP( D_PENG_LIT( "CPEngPresenceNotificationHandler::CanHandleL() - %d" ), retStatus );

    return retStatus;
    }




// -----------------------------------------------------------------------------
// CPEngPresenceNotificationHandler::ProcessNotifyBlockL()
// -----------------------------------------------------------------------------
//
TBool CPEngPresenceNotificationHandler::ProcessNotifyBlockL(
    const TDesC8& aPresenceBlock,
    const TDesC& aDomain,
    const MDesCArray& aSubscribedPresences,
    MPEngPresenceAttrManager& aAttributeManager,
    MPEngXMLParser& aParser,
    RPointerArray< MPEngPresenceAttrModel2 >& aModels )
    {
    if ( aParser.DecodeL( aPresenceBlock, KUserIDXMLTag, EFalse ) )
        {
        //There is a presence id => unescape the id
        HBufC* nwPresenceID = aParser.ResultAsWVAddressL();
        CleanupStack::PushL( nwPresenceID );

        PENG_DP( D_PENG_LIT( "CPEngPresenceNotificationHandler - Presence notification for NW ID [%S]" ),
                 nwPresenceID );

        //Try get the presence id as it is subscribed
        TInt subscribedIx = PEngAddressUtils::FindWVAddressF( *nwPresenceID,
                                                              aSubscribedPresences,
                                                              aDomain,
                                                              PEngAddressUtils::EMatchToUniqueId );
        CleanupStack::PopAndDestroy( nwPresenceID );

        if ( subscribedIx == KErrNotFound )
            {
            //Matching subscription ID for notified presence ID not found
            PENG_DP( D_PENG_LIT( "CPEngPresenceNotificationHandler - NW ID not matched" ) );
            return EFalse;
            }


        const TDesC& subscribedPresenceID = aSubscribedPresences.MdcaPoint( subscribedIx );
        PENG_DP( D_PENG_LIT( "CPEngPresenceNotificationHandler - NW ID matches to local [%S]" ),
                 &subscribedPresenceID );


        //go through all known network attributes
        const TArray<TUint32> types = aAttributeManager.KnownAttributeTypes();
        const TInt typeCount = types.Count();

        MPEngPresenceAttrModel2* model = NULL;
        // load and store online status to notify change
        aAttributeManager.LoadAttributeLC( model,
                                           subscribedPresenceID,
                                           KUidPrAttrOnlineStatus,
                                           EPEngStorableModel );
        PEngAttrModelHelper::AddOrReplaceModelL( aModels,  *model );

        CleanupStack::Pop(); //model

        for ( TInt ii = 0; ii < typeCount; ii++ )
            {
            const TUint32 attributeType = types[ ii ];
            TPtrC8 attributeName;
            TPtrC8 attributeNameSpace;
            TInt err = aAttributeManager.GetAttributeXmlNameAndNameSpace( attributeType,
                                                                          attributeName,
                                                                          attributeNameSpace );
            if ( err == KErrNone )
                {
                if ( aParser.DecodeL( aPresenceBlock, attributeName, ETrue ) )
                    {
                    //attribute type was found from presence data block
                    //==> Update the attribute

                    PENG_DP( D_PENG_LIT( "CPEngPresenceNotificationHandler - Updating attribute [%08x]" ),
                             attributeType );

                    model = NULL;
                    aAttributeManager.LoadAttributeLC( model,
                                                       subscribedPresenceID,
                                                       attributeType,
                                                       EPEngStorableModel );

                    model->Advanced()->DecodeFromXmlL( aParser, aParser.ResultL() );

                    //Array might have already a same attribute for user
                    //==> replace existing or add the new
                    PEngAttrModelHelper::AddOrReplaceModelL( aModels, *model );
                    CleanupStack::Pop(); //model
                    }
                else
                    {
                    if ( attributeType == KUidPrAttrStatusText )
                        {

                        model = NULL;
                        aAttributeManager.LoadAttributeLC( model,
                                                           subscribedPresenceID,
                                                           KUidPrAttrStatusText,
                                                           EPEngStorableModel );

                        // model->Advanced()->DecodeFromXmlL( aParser, aParser.ResultL() );

                        //Array might have already a same attribute for user
                        //==> replace existing or add the new

                        model->SetDataDesC16L( KNullDesC, EPEngStatusText );

                        PEngAttrModelHelper::AddOrReplaceModelL( aModels, *model );
                        CleanupStack::Pop(); //model

                        }

                    }
                }


            }

        }

    //Not a notification at all or matching subscription
    //for notified presence ID was found
    //==> XML block succesfully processed
    return ETrue;
    }



//  End of File

