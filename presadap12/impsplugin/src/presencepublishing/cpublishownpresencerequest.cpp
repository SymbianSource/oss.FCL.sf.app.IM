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
* Description:  IMPS Protocol implementation for Presence Framework
*
*/


#include <ximpprotocolconnectionhost.h>
#include "CPublishOwnPresencerequest.h"
#include "impsconnectionmanagercontrolif.h"
#include "impsdebugprint.h"
#include "CImpsPluginAccessHandler.h"
#include "CImpsPluginPureDataHandler.h"

#include "impsplugintags.h"
#include "CPEngXMLParser.h"
#include "MPEngXMLParser.h"
#include "CPEngXmlSerializer.h"
#include "MPEngXmlSerializer.h"
#include "CImpsPluginConnectionManager.h"
#include "PEngXMLTags.h"

#include <ximpobjectfactory.h>
#include <ximpobjectcollection.h>
#include <protocolpresencepublishingdatahost.h>

#include <presentitygroupinfo.h>
#include <presenceinfofieldvaluetext.h>
#include <presenceinfofieldvalueenum.h>
#include <presentitygroupmemberinfo.h>
#include <ximpidentity.h>
#include <presenceinfo.h>
#include <personpresenceinfo.h>
#include <servicepresenceinfo.h>
#include <devicepresenceinfo.h>
#include <presenceinfofield.h>
#include <presenceinfofieldcollection.h>
#include "PImpsAdapXMLTools.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPublishOwnPresencerequest::CPublishOwnPresencerequest
// ---------------------------------------------------------------------------
//
CPublishOwnPresencerequest::CPublishOwnPresencerequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CPublishOwnPresencerequest Start" ) );
    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "::CPublishOwnPresencerequest End" ) );
    }


// ---------------------------------------------------------------------------
// CPublishOwnPresencerequest::ConstructL
// ---------------------------------------------------------------------------
//
void CPublishOwnPresencerequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CPublishOwnPresencerequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CPublishOwnPresencerequest::NewL
// ---------------------------------------------------------------------------
//
CPublishOwnPresencerequest* CPublishOwnPresencerequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CPublishOwnPresencerequest::NewL Start" ) );

    CPublishOwnPresencerequest* self = new( ELeave )
    CPublishOwnPresencerequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CPublishOwnPresencerequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CPublishOwnPresencerequest::NewLC
// ---------------------------------------------------------------------------
//
CPublishOwnPresencerequest* CPublishOwnPresencerequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CPublishOwnPresencerequest::NewLC Start" ) );

    CPublishOwnPresencerequest* self =
        CPublishOwnPresencerequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CPublishOwnPresencerequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CPublishOwnPresencerequest::~CPublishOwnPresencerequest
// ---------------------------------------------------------------------------
//
CPublishOwnPresencerequest::~CPublishOwnPresencerequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CPublishOwnPresencerequest Start" ) );

    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "::~CPublishOwnPresencerequest End" ) );
    }


// ---------------------------------------------------------------------------
// CPublishOwnPresencerequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CPublishOwnPresencerequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CPublishOwnPresencerequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CPublishOwnPresencerequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CPublishOwnPresencerequest::RunL()
// ---------------------------------------------------------------------------
//
void CPublishOwnPresencerequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CPublishOwnPresencerequest::RunL Start" ) );

    User::LeaveIfError( iStatus.Int() );

    iResponse = iConnMan.DataHandler().ResponseL( iSendId );

    TPtrC8 resultBlock( NULL, 0 );
    TInt wvErrorCode = KErrNone;

    //Look for mandatory parts
    if ( iParser->DecodeL( *iResponse, KResultXMLTag, ETrue ) )
        {
        resultBlock.Set( iParser->ResultL() );
        if ( iParser->DecodeL( resultBlock, KCodeXMLTag, EFalse ) )
            {
            TPtrC8 cspStatusCodeBlock( iParser->ResultL() );
            TInt cspStatusInt;
            TLex8 lexer( cspStatusCodeBlock );
            lexer.SkipSpace();
            TInt lexErr = lexer.Val( cspStatusInt );
            if ( lexErr == KErrNone )
                {
                wvErrorCode = cspStatusInt;
                }
            }
        }

    // from server notification extract the own presence details
    // and update the data in PrFw.

    IMPS_DP( D_IMPS_LIT( "PublishOwnPresencerequest Returns %d" ), wvErrorCode );
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CPublishOwnPresencerequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CPublishOwnPresencerequest::RunError
// ---------------------------------------------------------------------------
//
TInt CPublishOwnPresencerequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CPublishOwnPresencerequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CPublishOwnPresencerequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CPublishOwnPresencerequest::PublishOwnPresenceL
// ---------------------------------------------------------------------------
//
void CPublishOwnPresencerequest::PublishOwnPresenceL( const MPresenceInfo& aPresence )
    {
    IMPS_DP( D_IMPS_LIT( "CPublishOwnPresencerequest::PublishOwnPresenceL Start" ) );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    // extract device and service specific count of fields from the info
    TInt devicecount = aPresence.DevicePresenceCount();
    TInt servicecount = aPresence.ServicePresenceCount();
    TInt personcount = aPresence.PersonPresence()->Fields().FieldCount();
    IMPS_DP( D_IMPS_LIT( "PublishOwnPresenceL  Info :device  %d service %d " ),
             devicecount, servicecount );

    // how many person pres attributes are set?

    IMPS_DP( D_IMPS_LIT( "PublishOwnPresenceL  Info: person  %d " ), personcount );


    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KUpdatePresenceRequest );
    serializer->StartTagL( KPresenceSubList );
    serializer->AttributeL( KXmlXmlns, KPresenceSubListAttributesNS_CSP12 );
    ExtractPersonPresenceAttributesL( aPresence, serializer );
    ExtractDevicePresenceAttributesL( aPresence, serializer );
    ExtractServicePresenceAttributesL( aPresence, serializer );

    // extract service presence attributes as below


    serializer->EndTagL( KPresenceSubList );
    serializer->EndTagL( KUpdatePresenceRequest );
    serializer->EndTagL( KTransactionContent );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    IMPS_DP( D_IMPS_LIT( " PublishOwnPresenceL SendData Request id %d " ), iSendId );
    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "CPublishOwnPresencerequest::PublishOwnPresenceL End" ) );
    }

// ---------------------------------------------------------------------------
// CPublishOwnPresencerequest::ExtractPersonPresenceAttributes
// ---------------------------------------------------------------------------
//
void CPublishOwnPresencerequest::ExtractPersonPresenceAttributesL(
    const MPresenceInfo& aPresence,
    MPEngXMLSerializer* aSerializer )
    {
    IMPS_DP( D_IMPS_LIT( "::ExtractPersonPresenceAttributes End" ) );


    const MPersonPresenceInfo* personPresence = aPresence.PersonPresence();

    const MPresenceInfoFieldCollection& presenceFields = personPresence->Fields();
    RPointerArray< HBufC > clientinfoattribute;
    TInt fieldCount = presenceFields.FieldCount();
    TInt count( 0 );
    TInt clintinfo[5];

    for ( TInt bx( 0 ); bx < fieldCount; bx++ )
        {
        const MPresenceInfoField& presenceField = presenceFields.FieldAt( bx );

        TBuf8<32> temp = presenceField.FieldType();

        // iterate through each of the field for the values
        IMPS_DP( D_IMPS_LIT( "ExtractPersonPresenceAttributes:%S " ), &presenceField.FieldType() );


        if ( presenceField.FieldType().Compare( KUserAvailabilityXMLTag ) == 0 )
            {
            const MPresenceInfoFieldValueEnum* availability =
                TXIMPGetInterface< const MPresenceInfoFieldValueEnum >::From(
                    presenceField.FieldValue(),
                    MXIMPBase::EPanicIfUnknown );

            // now manufacture the message for the online status
            aSerializer->StartTagL( KUserAvailabilityXMLTag );
            aSerializer->StartTagL( KQualifierXMLTag
                                  ).NarrowTextL( KXMLValueTrue
                                               ).EndTagL( KQualifierXMLTag );
            if ( availability->Value() == 2 )
                {
                aSerializer->StartTagL( KPresenceValueXMLTag
                                      ).NarrowTextL( KAvailableXMLTag
                                                   ).EndTagL( KPresenceValueXMLTag );
                }
            else if ( availability->Value() == 1 )
                {
                aSerializer->StartTagL( KPresenceValueXMLTag
                                      ).NarrowTextL( KNotAvailableXMLTag
                                                   ).EndTagL( KPresenceValueXMLTag );
                }
            aSerializer->EndTagL( KUserAvailabilityXMLTag );

            }

        // publishing online status
        if ( presenceField.FieldType().Compare( KOnlineStatusXMLTag ) == 0 )
            {
            const MPresenceInfoFieldValueText* onlinestatus =
                TXIMPGetInterface< const MPresenceInfoFieldValueText >::From(
                    presenceField.FieldValue(),
                    MXIMPBase::EPanicIfUnknown );

            // now manufacture the message for the status text

            aSerializer->StartTagL( KOnlineStatusXMLTag );
            aSerializer->StartTagL( KQualifierXMLTag
                                  ).NarrowTextL( KXMLValueTrue
                                               ).EndTagL( KQualifierXMLTag );
            aSerializer->StartTagL( KPresenceValueXMLTag
                                  ).NarrowTextL( KXMLValueTrue
                                               ).EndTagL( KPresenceValueXMLTag );
            aSerializer->EndTagL( KOnlineStatusXMLTag );

            }

        // publishing status text


        if ( presenceField.FieldType().Compare( KStatusTextXMLTag ) == 0 )
            {
            const MPresenceInfoFieldValueText* statustext =
                TXIMPGetInterface< const MPresenceInfoFieldValueText >::From(
                    presenceField.FieldValue(),
                    MXIMPBase::EPanicIfUnknown );

            // now manufacture the message for the status text

            aSerializer->StartTagL( KStatusTextXMLTag );
            aSerializer->StartTagL( KQualifierXMLTag
                                  ).NarrowTextL( KXMLValueTrue
                                               ).EndTagL( KQualifierXMLTag );
            aSerializer->StartTagL( KPresenceValueXMLTag
                                  ).UnicodeTextL( statustext->TextValue()
                                                ).EndTagL( KPresenceValueXMLTag );
            aSerializer->EndTagL( KStatusTextXMLTag );

            }
        // end of status text


        // publishing status mood
        if ( presenceField.FieldType().Compare( KStatusMood ) == 0 )
            {
            const MPresenceInfoFieldValueText* statusmood =
                TXIMPGetInterface< const MPresenceInfoFieldValueText >::From(
                    presenceField.FieldValue(),
                    MXIMPBase::EPanicIfUnknown );

            // now manufacture the message for the status Mood

            aSerializer->StartTagL( KStatusMoodXMLTag );
            aSerializer->StartTagL( KQualifierXMLTag
                                  ).NarrowTextL( KXMLValueTrue
                                               ).EndTagL( KQualifierXMLTag );
            aSerializer->StartTagL( KPresenceValueXMLTag
                                  ).UnicodeTextL( statusmood->TextValue()
                                                ).EndTagL( KPresenceValueXMLTag );
            aSerializer->EndTagL( KStatusMoodXMLTag );

            }
        //end of status tag

        //publishing Alias

        if ( presenceField.FieldType().Compare( KAliasXMLTag ) == 0 )
            {
            const MPresenceInfoFieldValueText* alias =
                TXIMPGetInterface< const MPresenceInfoFieldValueText >::From(
                    presenceField.FieldValue(),
                    MXIMPBase::EPanicIfUnknown );

            // now manufacture the message for the  alias

            aSerializer->StartTagL( KAliasXMLTag );
            aSerializer->StartTagL( KQualifierXMLTag
                                  ).NarrowTextL( KXMLValueTrue
                                               ).EndTagL( KQualifierXMLTag );
            aSerializer->StartTagL( KPresenceValueXMLTag
                                  ).UnicodeTextL( alias->TextValue()
                                                ).EndTagL( KPresenceValueXMLTag );
            aSerializer->EndTagL( KAliasXMLTag );

            }
        //end of Alias

        //publishing preferred language
        if ( presenceField.FieldType().Compare( KPreferredLanguage ) == 0 )
            {
            const MPresenceInfoFieldValueText* preferredlanguage =
                TXIMPGetInterface< const MPresenceInfoFieldValueText >::From(
                    presenceField.FieldValue(),
                    MXIMPBase::EPanicIfUnknown );


            // now manufacture the message for the language

            aSerializer->StartTagL( KPreferredLanguage );
            aSerializer->StartTagL( KQualifierXMLTag
                                  ).NarrowTextL( KXMLValueTrue
                                               ).EndTagL( KQualifierXMLTag );
            aSerializer->StartTagL( KPresenceValueXMLTag
                                  ).UnicodeTextL( preferredlanguage->TextValue()
                                                ).EndTagL( KPresenceValueXMLTag );
            aSerializer->EndTagL( KPreferredLanguage );

            }

        //end of preferred language

        if ( presenceField.FieldType().Compare( KStatusContentXMLTag ) == 0 )
            {
            const MPresenceInfoFieldValueText* statuscontent =
                TXIMPGetInterface< const MPresenceInfoFieldValueText >::From(
                    presenceField.FieldValue(),
                    MXIMPBase::EPanicIfUnknown );

            // now manufacture the message for the status content

            aSerializer->StartTagL( KStatusContentXMLTag );
            aSerializer->StartTagL( KQualifierXMLTag
                                  ).NarrowTextL( KXMLValueTrue
                                               ).EndTagL( KQualifierXMLTag );
            aSerializer->StartTagL( KPresenceValueXMLTag
                                  ).UnicodeTextL( statuscontent->TextValue()
                                                ).EndTagL( KPresenceValueXMLTag );
            aSerializer->EndTagL( KStatusContentXMLTag );

            }

        if ( presenceField.FieldType().Compare( KLinkXMLTag ) == 0 )
            {
            const MPresenceInfoFieldValueText* link =
                TXIMPGetInterface< const MPresenceInfoFieldValueText >::From(
                    presenceField.FieldValue(),
                    MXIMPBase::EPanicIfUnknown );
            // now manufacture the message for the status content

            aSerializer->StartTagL( KInfoLinkXMLTag );
            aSerializer->StartTagL( KInflink );
            aSerializer->StartTagL( KQualifierXMLTag
                                  ).NarrowTextL( KXMLValueTrue
                                               ).EndTagL( KQualifierXMLTag );
            aSerializer->StartTagL( KLinkXMLTag
                                  ).UnicodeTextL( link->TextValue()
                                                ).EndTagL( KLinkXMLTag );
            aSerializer->EndTagL( KInflink );
            aSerializer->EndTagL( KInfoLinkXMLTag );

            }
        if ( presenceField.FieldType().Compare( KTextXMLTag ) == 0 )
            {
            const MPresenceInfoFieldValueText* text =
                TXIMPGetInterface< const MPresenceInfoFieldValueText >::From(
                    presenceField.FieldValue(),
                    MXIMPBase::EPanicIfUnknown );
            // now manufacture the message for the status content

            aSerializer->StartTagL( KInfoLinkXMLTag );
            aSerializer->StartTagL( KInflink );
            aSerializer->StartTagL( KQualifierXMLTag
                                  ).NarrowTextL( KXMLValueTrue
                                               ).EndTagL( KQualifierXMLTag );
            aSerializer->StartTagL( KTextXMLTag
                                  ).UnicodeTextL( text->TextValue()
                                                ).EndTagL( KTextXMLTag );
            aSerializer->EndTagL( KInflink );
            aSerializer->EndTagL( KInfoLinkXMLTag );

            }
        if ( presenceField.FieldType().Compare( KContentTypeXMLTag ) == 0 )
            {
            const MPresenceInfoFieldValueText* text =
                TXIMPGetInterface< const MPresenceInfoFieldValueText >::From(
                    presenceField.FieldValue(),
                    MXIMPBase::EPanicIfUnknown );
            // now manufacture the message for the status content

            aSerializer->StartTagL( KInfoLinkXMLTag );
            aSerializer->StartTagL( KInflink );
            aSerializer->StartTagL( KQualifierXMLTag
                                  ).NarrowTextL( KXMLValueTrue
                                               ).EndTagL( KQualifierXMLTag );
            aSerializer->StartTagL( KContentTypeXMLTag
                                  ).UnicodeTextL( text->TextValue()
                                                ).EndTagL( KContentTypeXMLTag );
            aSerializer->EndTagL( KInflink );
            aSerializer->EndTagL( KInfoLinkXMLTag );

            }
        // publish client info
        if ( ( presenceField.FieldType().Compare( KClientTypeXMLTag ) == 0 ) ||
             ( presenceField.FieldType().Compare( KClientLanguageXMLTag ) == 0 ) ||
             ( presenceField.FieldType().Compare( KDevManufacturerXMLTag ) == 0 ) ||
             ( presenceField.FieldType().Compare( KDeviceModelXMLTag ) == 0 ) )
            {

            //    TDesC8 temp = presenceField.FieldType();
            //  clientinfoattribute.AppendL(*temp);
            clintinfo[count++] = bx;



            }

        // publish common capability

        if ( presenceField.FieldType().Compare( KCommCapStatusXMLTag ) == 0 )
            {

            aSerializer->StartTagL( KCommCapXMLTag );
            aSerializer->StartTagL( KCommCXMLTag );
            aSerializer->StartTagL( KCommCapXMLTagCap ).RawValueL( KCommCapIMXMLTag ).EndTagL( KCommCapXMLTagCap );


            const MPresenceInfoFieldValueText* status =
                TXIMPGetInterface< const MPresenceInfoFieldValueText >::From(
                    presenceField.FieldValue(),
                    MXIMPBase::EPanicIfUnknown );
            aSerializer->StartTagL( KCommCapStatusXMLTag
                                  ).UnicodeTextL( status->TextValue()
                                                ).EndTagL( KCommCapStatusXMLTag );
            aSerializer->EndTagL( KCommCapXMLTag );
            aSerializer->EndTagL( KCommCXMLTag );


            }

        }

    if ( count > 0 )
        {

        aSerializer->StartTagL( KClientInfoXMLTag );
        aSerializer->StartTagL( KQualifierXMLTag
                              ).NarrowTextL( KXMLValueTrue
                                           ).EndTagL( KQualifierXMLTag );

        for ( TInt ix( 0 );ix < count;ix++ )

            {
            const MPresenceInfoField& presenceField = presenceFields.FieldAt( clintinfo[ix]  );
            // putting client type

            if ( presenceField.FieldType().Compare( KClientTypeXMLTag ) == 0 )
                {
                const MPresenceInfoFieldValueText* clienttype =
                    TXIMPGetInterface< const MPresenceInfoFieldValueText >::From(
                        presenceField.FieldValue(),
                        MXIMPBase::EPanicIfUnknown );
                aSerializer->StartTagL( KClientTypeXMLTag
                                      ).UnicodeTextL( clienttype->TextValue()
                                                    ).EndTagL( KClientTypeXMLTag );

                }

            //client langauage
            if ( presenceField.FieldType().Compare( KClientLanguageXMLTag ) == 0 )
                {
                const MPresenceInfoFieldValueText* clientlangauage =
                    TXIMPGetInterface< const MPresenceInfoFieldValueText >::From(
                        presenceField.FieldValue(),
                        MXIMPBase::EPanicIfUnknown );
                aSerializer->StartTagL( KClientLanguageXMLTag
                                      ).UnicodeTextL( clientlangauage->TextValue()
                                                    ).EndTagL( KClientLanguageXMLTag );

                }

            // device manufacturer
            if ( presenceField.FieldType().Compare( KDevManufacturerXMLTag ) == 0 )
                {
                const MPresenceInfoFieldValueText* devmanu =
                    TXIMPGetInterface< const MPresenceInfoFieldValueText >::From(
                        presenceField.FieldValue(),
                        MXIMPBase::EPanicIfUnknown );
                aSerializer->StartTagL( KDevManufacturerXMLTag
                                      ).UnicodeTextL( devmanu->TextValue()
                                                    ).EndTagL( KDevManufacturerXMLTag );

                }

            // device modal

            if ( presenceField.FieldType().Compare( KDeviceModelXMLTag ) == 0 )
                {
                const MPresenceInfoFieldValueText* devmodal =
                    TXIMPGetInterface< const MPresenceInfoFieldValueText >::From(
                        presenceField.FieldValue(),
                        MXIMPBase::EPanicIfUnknown );
                aSerializer->StartTagL( KDeviceModelXMLTag
                                      ).UnicodeTextL( devmodal->TextValue()
                                                    ).EndTagL( KDeviceModelXMLTag );

                }
            // now manufacture the message for the status Mood
            }

        aSerializer->EndTagL( KClientInfoXMLTag );
        }

    IMPS_DP( D_IMPS_LIT( "::ExtractPersonPresenceAttributes End" ) );
    }

// ---------------------------------------------------------------------------
// CPublishOwnPresencerequest::ExtractServicePresenceAttributes
// ---------------------------------------------------------------------------
//
void CPublishOwnPresencerequest::ExtractServicePresenceAttributesL(
    const MPresenceInfo& aPresence,
    MPEngXMLSerializer* aSerializer )
    {
    IMPS_DP( D_IMPS_LIT( "::ExtractServicePresenceAttributes End" ) );
    TInt servicePresences = aPresence.ServicePresenceCount();

    for ( TInt ix( 0 ); ix < servicePresences; ix++ )
        {
        const MServicePresenceInfo& servicePresence = aPresence.ServicePresenceAt( ix );
        const MPresenceInfoFieldCollection& presenceFields = servicePresence.Fields();

        TInt fieldCount = presenceFields.FieldCount();
        for ( TInt bx( 0 ); bx < fieldCount; bx++ )
            {
            const MPresenceInfoField& presenceField = presenceFields.FieldAt( bx );
            if ( presenceField.FieldType().Compare( KRegistrationXMLTag ) == 0 )
                {
                const MPresenceInfoFieldValueText* registration =
                    TXIMPGetInterface< const MPresenceInfoFieldValueText >::From( presenceField.FieldValue(),
                                                                                  MXIMPBase::EPanicIfUnknown );

                aSerializer->StartTagL( KRegistrationXMLTag );
                aSerializer->StartTagL( KQualifierXMLTag
                                      ).NarrowTextL( KXMLValueTrue
                                                   ).EndTagL( KQualifierXMLTag );
                aSerializer->StartTagL( KPresenceValueXMLTag
                                      ).UnicodeTextL( registration->TextValue()
                                                    ).EndTagL( KPresenceValueXMLTag );
                aSerializer->EndTagL( KRegistrationXMLTag );
                }
            }
        }


    IMPS_DP( D_IMPS_LIT( "::ExtractServicePresenceAttributes End" ) );
    }

// ---------------------------------------------------------------------------
// CPublishOwnPresencerequest::ExtractDevicePresenceAttributes
// ---------------------------------------------------------------------------
//
void CPublishOwnPresencerequest::ExtractDevicePresenceAttributesL(
    const MPresenceInfo& aPresence,
    MPEngXMLSerializer* aSerializer )
    {

    IMPS_DP( D_IMPS_LIT( "::ExtractDevicePresenceAttributes End" ) );

    TInt devicePresenceCount = aPresence.DevicePresenceCount();

    for ( TInt ix( 0 ); ix < devicePresenceCount; ix++ )
        {
        const MDevicePresenceInfo& devicePresence = aPresence.DevicePresenceAt( ix );
        const MPresenceInfoFieldCollection& presenceFields = devicePresence.Fields();

        TInt fieldCount = presenceFields.FieldCount();

        for ( TInt bx( 0 ); bx < fieldCount; bx++ )
            {
            const MPresenceInfoField& presenceField = presenceFields.FieldAt( bx );

            if ( presenceField.FieldType().Compare( KPlmn ) == 0 )
                {
                const MPresenceInfoFieldValueText* plmn =
                    TXIMPGetInterface< const MPresenceInfoFieldValueText >::From(
                        presenceField.FieldValue(),
                        MXIMPBase::EPanicIfUnknown );

                aSerializer->StartTagL( KPlmn );
                aSerializer->StartTagL( KQualifierXMLTag
                                      ).NarrowTextL( KXMLValueTrue
                                                   ).EndTagL( KQualifierXMLTag );
                aSerializer->StartTagL( KPresenceValueXMLTag
                                      ).UnicodeTextL( plmn->TextValue()
                                                    ).EndTagL( KPresenceValueXMLTag );
                aSerializer->EndTagL( KPlmn );
                }


            }
        }

    IMPS_DP( D_IMPS_LIT( "::ExtractDevicePresenceAttributes End" ) );
    }

// End of file
