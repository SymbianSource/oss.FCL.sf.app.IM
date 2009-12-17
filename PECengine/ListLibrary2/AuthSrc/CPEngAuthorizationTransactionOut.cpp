/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Authorization out transaction handler.
*
*/

// INCLUDE FILES
#include "CPEngAuthorizationTransactionOut.h"

#include "CPEngAuthorizationResponse.h"
#include "MPEngAuthorizationEngine.h"

#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include "MPEngPresenceAttrManager.h"
#include "PEngAttrLibFactory.h"

#include "CPEngTransactionStatus.h"
#include "PEngWVPresenceErrors2.h"
#include "PresenceDebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionOut::CPEngAuthorizationTransactionOut()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationTransactionOut::CPEngAuthorizationTransactionOut(
    MPEngAuthorizationEngine& aAuthEngine,
    MPEngPresenceAttrManager& aAttrManager,
    MPEngXMLParser& aXMLParser,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
        : iAuthEngine( aAuthEngine ),
        iAttributeManager( aAttrManager ),
        iXMLParser( aXMLParser ),
        iCSPVersion( aCSPVersion ),
        iTransCompleted( EFalse ),
        iOperationId( aOperationId )
    {
    PENG_DP( D_PENG_LIT( "CPEngAuthorizationTransactionOut::CPEngAuthorizationTransactionOut()" ) );
    iAuthEngine.LockForNetworkPublishing();
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionOut::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransactionOut::ConstructL()
    {
    iTransactionStatus =  CPEngTransactionStatus::NewL();
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionOut::NewLC()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationTransactionOut* CPEngAuthorizationTransactionOut::NewLC(
    MPEngAuthorizationEngine& aAuthEngine,
    MPEngPresenceAttrManager& aAttrManager,
    MPEngXMLParser& aXMLParser,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
    {
    CPEngAuthorizationTransactionOut* self =
        new ( ELeave ) CPEngAuthorizationTransactionOut(
        aAuthEngine,
        aAttrManager,
        aXMLParser,
        aCSPVersion,
        aOperationId );
    CleanupStack::PushL( self );
    self->ConstructL( );

    return self;
    }


// Destructor
CPEngAuthorizationTransactionOut::~CPEngAuthorizationTransactionOut()
    {
    iAuthEngine.UnLockForNetworkPublishing();
    delete iResponse;
    delete iTransactionStatus;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionOut::RequestL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransactionOut::RequestL( TDes8& aSendBuffer )
    {
    aSendBuffer.Zero();
    MPEngXMLSerializer* xmlSerializer = CreateXmlSerializerL( aSendBuffer );
    CleanupClosePushL( *xmlSerializer );


    // <TransactionContent xmlns="http://www.wireless -village.org/TRC1.1">
    if ( iCSPVersion == EWVCspV11 )
        {
        xmlSerializer->StartTagL(  KTransactionContent );
        xmlSerializer->AttributeL( KXmlXmlns, KTransactionContentNS );
        }
    else
        {
        xmlSerializer->StartTagL( KTransactionContent );
        xmlSerializer->AttributeL( KXmlXmlns, KTransactionContentNS_CSP12 );
        }


    switch ( iResponse->AuthorizationStatus() )
        {
        case MPEngAuthorizationRequest::EPEngAuthAccepted: // fall through
        case MPEngAuthorizationRequest::EPEngAuthDenied:
            {
            AppendXMLPresenceAuthUserL( *xmlSerializer );
            break;
            }

        case MPEngAuthorizationRequest::EPEngAuthCanceled:
            {
            AppendXMLCancelAuthorizationL( *xmlSerializer );
            }

        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }


    // end of transaction
    // </TransactionContent>
    xmlSerializer->EndTagL( KTransactionContent );
    CleanupStack::PopAndDestroy( ); // xmlSerializer
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionOut::LastRunningTransactionHandler()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransactionOut::LastRunningTransactionHandler()
    {
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionOut::ProcessResponseL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransactionOut::ProcessResponseL(
    const TDesC8& aResponse,
    TRequestStatus& /* aStatus */ )
    {
    PENG_DP( D_PENG_LIT( "CPEngAuthorizationTransactionOut::ProcessResponseL()" ) );


    // check if we have some answer from the server
    TInt err ( iXMLParser.ParseResultL( aResponse,
                                        iOperationId,
                                        *iTransactionStatus ) );
    // how did transaction go
    switch ( err )
        {
        case KErrNone: // transaction OK
            {
            // Authorization response went fine, store it
            iAuthEngine.UpdateAuthorizationStateL( *iResponse,
                                                   ( iCSPVersion == EWVCspV12 ) );
            break;
            }

        case KPEngNwErrUnrespondedPresenceRequest:
        case KPEngNwErrUnknownUser:
        case KPEngNwErrBadParameter:
            {
            // unknown user id,or bad param, remove Authorization request
            iAuthEngine.RemoveAuthorizationRequestL( *iResponse );
            break;
            }

        default:
            {
            }
        }

    iTransCompleted = ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionOut::CancelProcessing()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransactionOut::CancelProcessing( )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionOut::NewTransactionHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransactionOut::NewTransactionHandlersL(
    RPointerArray<MPEngOutgoingTransactionHandler>& /* aTransactionsArray*/ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionOut::TransactionCompleted()
// -----------------------------------------------------------------------------
//
TBool CPEngAuthorizationTransactionOut::TransactionCompleted()
    {
    return iTransCompleted;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionOut::TransactionResult()
// -----------------------------------------------------------------------------
//
CPEngTransactionStatus*  CPEngAuthorizationTransactionOut::TransactionResult()
    {
    CPEngTransactionStatus* temp = iTransactionStatus;
    iTransactionStatus = NULL;
    return temp;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionOut::ReleaseHandler()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransactionOut::ReleaseHandler()
    {
    iTransCompleted = ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionOut::SetAuthResponse()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransactionOut::SetAuthResponse(
    CPEngAuthorizationResponse* aResponse )
    {
    delete iResponse;
    iResponse = aResponse;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionOut::AppendXMLPresenceAuthUserL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransactionOut::AppendXMLPresenceAuthUserL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    // <PresenceAuth-User>
    aXmlSerializer.StartTagL( KPresenceAuthUser );
    AppendXMLUserIdL( aXmlSerializer );

    // <Acceptance>
    aXmlSerializer.StartTagL( KAcceptance );


    if ( iResponse->AuthorizationStatus() == MPEngAuthorizationRequest::EPEngAuthAccepted )
        {
        aXmlSerializer.RawValueL( KXMLValueTrue );
        }
    else
        {
        aXmlSerializer.RawValueL( KXMLValueFalse );
        }
    // </Acceptance>
    aXmlSerializer.EndTagL( KAcceptance );


    // if CSP 1.2 we also append list of attributes to accept
    if ( iCSPVersion == EWVCspV12 )
        {
        const RArray<TUint32>& attributes = iResponse->AuthorizedAttributes();
        TInt count( attributes.Count() );
        // xmlns="Name space"
        // append attribute's namespace according to attributes on the list
        // Append Attribute name space
        TPtrC8 attributeName;
        TPtrC8 attributeNameSpace;
        User::LeaveIfError(
            iAttributeManager.GetAttributeXmlNameAndNameSpace( attributes[0],
                                                               attributeName,
                                                               attributeNameSpace ) );

        // <PresenceSubList>
        aXmlSerializer.StartTagL( KPresenceSubList );
        aXmlSerializer.AttributeL( KXmlXmlns, attributeNameSpace );

        // Append attributes one by one
        for ( TInt x( 0 ) ; x < count ; x++ )
            {
            User::LeaveIfError(
                iAttributeManager.GetAttributeXmlNameAndNameSpace( attributes[ x ],
                                                                   attributeName,
                                                                   attributeNameSpace ) );

            aXmlSerializer.StartTagL( attributeName
                                    ).EndTagL( attributeName );
            }

        // </PresenceSubList>
        aXmlSerializer.EndTagL( KPresenceSubList );
        }

    // </PresenceAuth-User>
    aXmlSerializer.EndTagL( KPresenceAuthUser );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionOut::AppendXMLCancelAuthorizationL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransactionOut::AppendXMLCancelAuthorizationL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    //  <CancelAuth-Request>
    aXmlSerializer.StartTagL( KCancelAuthRequest );

    AppendXMLUserIdL( aXmlSerializer );

    // </CancelAuth-Request>
    aXmlSerializer.EndTagL( KCancelAuthRequest );
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionOut::AppendXMLUserIdL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransactionOut::AppendXMLUserIdL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    aXmlSerializer.StartTagL( KUserIDXMLTag
                            ).WvAddressL( iResponse->Id()
                                        ).EndTagL( KUserIDXMLTag );
    }

//  End of File










































