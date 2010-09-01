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
#include "cpresencenotificationhandler.h"
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
#include <protocolpresentitygroupsdatahost.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <ximpidentity.h>
#include <protocolpresencepublishingdatahost.h>
#include <protocolpresencedatahost.h>
#include <presenceobjectfactory.h>
#include <presencegrantrequestinfo.h>


#include <personpresenceinfo.h>
#include <servicepresenceinfo.h>
#include <devicepresenceinfo.h>
#include <presenceinfo.h>

#include <presenceinfofield.h>
#include <presenceinfofieldcollection.h>
#include <presenceinfofieldvalueenum.h>
#include <presenceinfofieldvaluetext.h>
#include <ximpcontext.h>
#include <ximpclient.h>
#include <devicepresenceinfo.h>
#include <protocolpresencewatchingdatahost.h>
#include <presencegrantrequestinfo.h>
#include <protocolpresenceauthorizationdatahost.h>
#include <e32des16.h>
#include "PImpsAdapXMLTools.h"
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresenceNotificationHandler::CPresenceNotificationHandler
// ---------------------------------------------------------------------------
//
CPresenceNotificationHandler::CPresenceNotificationHandler(
    MImpsPrtPluginConnectionManager& aConnMan ) :

        CActive( EPriorityNormal ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CPresenceNotificationHandler Start" ) );
    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "::CPresenceNotificationHandler End" ) );
    }


// ---------------------------------------------------------------------------
// CPresenceNotificationHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CPresenceNotificationHandler::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CPresenceNotificationHandler::NewL
// ---------------------------------------------------------------------------
//
CPresenceNotificationHandler* CPresenceNotificationHandler::NewL(
    MImpsPrtPluginConnectionManager& aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::NewL Start" ) );

    CPresenceNotificationHandler* self = new( ELeave )
    CPresenceNotificationHandler( aConnMan );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CPresenceNotificationHandler::NewLC
// ---------------------------------------------------------------------------
//
CPresenceNotificationHandler* CPresenceNotificationHandler::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::NewLC Start" ) );

    CPresenceNotificationHandler* self =
        CPresenceNotificationHandler::NewL( aConnMan );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresenceNotificationHandler::~CPresenceNotificationHandler
// ---------------------------------------------------------------------------
//
CPresenceNotificationHandler::~CPresenceNotificationHandler()
    {
    IMPS_DP( D_IMPS_LIT( "::~CPresenceNotificationHandler Start" ) );

    Cancel();

    delete iResponse;
    delete iListId;
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "::~CPresenceNotificationHandler End" ) );
    }


// ---------------------------------------------------------------------------
// CPresenceNotificationHandler::DoCancel()
// ---------------------------------------------------------------------------
//
void CPresenceNotificationHandler::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::DoCancel Start" ) );

    iConnMan.DataHandler().CancelListening();

    IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::DoCancel End" ) );
    }

// ---------------------------------------------------------------------------
// CPresenceNotificationHandler::RunL()
// ---------------------------------------------------------------------------
//
void CPresenceNotificationHandler::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::RunL Start" ) );

    // something is coming from the WV server

    HandleIncomingDataL();
    StartListeningL();

    IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::RunL End" ) );
    }

// ---------------------------------------------------------------------------
// CPresenceNotificationHandler::ProcessIncomingDataL()
// ---------------------------------------------------------------------------
//
void CPresenceNotificationHandler::ProcessIncomingDataL()
    {
    IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::ProcessIncomingDataL Start" ) );

    User::LeaveIfError( iStatus.Int() );

    // get the incoming response from the server with transaction id equals zero

    iResponse = iConnMan.DataHandler().ResponseL( 0 );

    if ( !iResponse )
        {
        StartListeningL();
        return;
        }

    if ( iParser->DecodeL( *iResponse, KPresenceNotificationRequest, ETrue ) )
        {
        //CPrFwTestContextWrapper* wrapper = iWrapperMgr->GetWrapperL( 0 );
        MXIMPClient* aPresClient = MXIMPClient::NewClientL();
        CleanupDeletePushL( aPresClient );
        MXIMPContext* context = aPresClient->NewPresenceContextLC();

        // MXIMPContext* context = wrapper->GetContext();

//   MServicePresenceInfo* srvInfo = context->ObjectFactory().NewServicePresenceInfoLC();
//   srvInfo->SetServiceTypeL( KServiceType );  // test

        // MDevicePresenceInfo* devinfo = context->ObjectFactory().NewDevicePresenceInfo();
        if ( iParser->DecodeL( iParser->ResultL(),
                               KPresenceXMLTag,
                               ETrue ) )
            {
            if ( iParser->DecodeL( *iResponse, KUserIDXMLTag, EFalse ) )
                {
                TInt countResults( iParser->Count() );
                MPEngXMLParser* subParser = CreateXMLParserLC();
                for ( TInt i = 0 ; i < countResults ; ++i )
                    {
                    //NPresenceInfo* info = context->ObjectFactory().NewPresenceInfoLC();
                    MPresenceInfo* info = iConnMan.HandleToHost().ProtocolPresenceDataHost().PresenceObjectFactory().NewPresenceInfoLC();
                    MPresenceObjectFactory& prfwObjFact = iConnMan.HandleToHost().ProtocolPresenceDataHost().PresenceObjectFactory();
                    MXIMPObjectFactory& objFactory = context->ObjectFactory();
                    MXIMPIdentity* contactIdentity;

                    contactIdentity = objFactory.NewIdentityLC();//1
                    iParser->DecodeL( *iResponse, KUserIDXMLTag, EFalse );
                    /**
                    * UserId string from the presence notification information
                    */
                    HBufC16* UserId1;
                    UserId1 = iParser->ResultAsUnicodeTextL( i );
                    contactIdentity->SetIdentityL( *UserId1 );
                    UserId1->Des().Fold();
                    IMPS_DP( D_IMPS_LIT( " CPresenceNotificationHandler::ProcessIncomingDataL:Userid1::%S" ), UserId1 );
                    contactIdentity->SetIdentityL( *UserId1 );
                    delete UserId1;
                    MPersonPresenceInfo* persInfo = prfwObjFact.NewPersonPresenceInfoLC();

                    // looking online status of user
                    if ( iParser->DecodeL( *iResponse, KOnlineStatusXMLTag, ETrue ) )
                        {
                        if ( subParser->DecodeL( iParser->ResultL(), KQualifierXMLTag, EFalse ) )
                            {
                            if ( subParser->ResultL().CompareF( KXMLValueTrue ) == 0 )
                                {
                                if ( subParser->DecodeL( iParser->ResultL(), KPresenceValueXMLTag, EFalse ) )
                                    {
                                    if ( subParser->ResultL().CompareF( KXMLValueTrue ) == 0 )
                                        {
                                        MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();//1
                                        MPresenceInfoFieldValueText* statusTextField = prfwObjFact.NewTextInfoFieldLC();
                                        statusTextField->SetTextValueL( _L( "T" ) );
                                        infoField->SetFieldTypeL( KCommCapStatusXMLTag );
                                        infoField->SetFieldValue( statusTextField );
                                        persInfo->Fields().AddOrReplaceFieldL( infoField );
                                        CleanupStack::Pop( 2 );
                                        }
                                    else
                                        {
                                        MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();//1
                                        MPresenceInfoFieldValueText* statusTextField = prfwObjFact.NewTextInfoFieldLC();
                                        statusTextField->SetTextValueL( _L( "F" ) );
                                        infoField->SetFieldTypeL( KCommCapStatusXMLTag );
                                        infoField->SetFieldValue( statusTextField );
                                        persInfo->Fields().AddOrReplaceFieldL( infoField );
                                        CleanupStack::Pop( 2 );

                                        }
                                    }

                                }
                            }
                        }

                    //looking for user availability


                    if ( iParser->DecodeL( *iResponse, KUserAvailabilityXMLTag, ETrue ) )
                        {
                        if ( subParser->DecodeL( iParser->ResultL(), KQualifierXMLTag, EFalse ) )
                            {
                            if ( subParser->ResultL().CompareF( KXMLValueTrue ) == 0 )
                                {
                                if ( subParser->DecodeL( iParser->ResultL(), KPresenceValueXMLTag, EFalse ) )
                                    {
                                    if ( subParser->ResultL().CompareF( KAvailabilityValueAvailable ) == 0 )
                                        {
                                        MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();//1
                                        MPresenceInfoFieldValueText* useravailibility = prfwObjFact.NewTextInfoFieldLC();
                                        useravailibility->SetTextValueL( _L( "AVAILABLE" ) );
                                        infoField->SetFieldTypeL( NPresenceInfo::NFieldType::KAvailability );
                                        infoField->SetFieldValue( useravailibility );
                                        persInfo->Fields().AddOrReplaceFieldL( infoField );
                                        CleanupStack::Pop( 2 );
                                        }
                                    else if ( subParser->ResultL().CompareF( KAvailabilityValueDiscreet ) == 0 )
                                        {
                                        MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();//1
                                        MPresenceInfoFieldValueText* useravailibility = prfwObjFact.NewTextInfoFieldLC();
                                        useravailibility->SetTextValueL( _L( "BUSY" ) );
                                        infoField->SetFieldTypeL( NPresenceInfo::NFieldType::KAvailability );
                                        infoField->SetFieldValue( useravailibility );
                                        persInfo->Fields().AddOrReplaceFieldL( infoField );
                                        CleanupStack::Pop( 2 );
                                        }
                                    else
                                        {
                                        MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();//1
                                        MPresenceInfoFieldValueText* useravailibility = prfwObjFact.NewTextInfoFieldLC();
                                        useravailibility->SetTextValueL( _L( "NOTAVAILABLE" ) );
                                        infoField->SetFieldTypeL( NPresenceInfo::NFieldType::KAvailability );
                                        infoField->SetFieldValue( useravailibility );
                                        persInfo->Fields().AddOrReplaceFieldL( infoField );
                                        CleanupStack::Pop( 2 );
                                        }
                                    }

                                }
                            }
                        }

                    //for status text

                    if ( iParser->DecodeL( *iResponse, KStatusTextXMLTag, ETrue ) )
                        {

                        if ( subParser->DecodeL( iParser->ResultL(), KQualifierXMLTag, EFalse ) )
                            {
                            if ( subParser->ResultL().CompareF( KXMLValueTrue ) == 0 )
                                {
                                if ( subParser->DecodeL( iParser->ResultL(), KPresenceValueXMLTag, EFalse ) )
                                    {
                                    HBufC16* TextValue;
                                    TextValue = subParser->ResultAsUnicodeTextL();
                                    TextValue->Des().Fold();
                                    MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();//1
                                    MPresenceInfoFieldValueText* statusTextField = prfwObjFact.NewTextInfoFieldLC();
                                    statusTextField->SetTextValueL( *TextValue );
                                    infoField->SetFieldTypeL( NPresenceInfo::NFieldType::KStatusMessage );
                                    infoField->SetFieldValue( statusTextField );
                                    persInfo->Fields().AddOrReplaceFieldL( infoField );
                                    delete TextValue;
                                    CleanupStack::Pop( 2 );
                                    }
                                else
                                    {

                                    // in colibria server we are not getting presencevale tag
                                    MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();
                                    MPresenceInfoFieldValueText* statusTextField = prfwObjFact.NewTextInfoFieldLC();
                                    statusTextField->SetTextValueL( KNullDesC );
                                    infoField->SetFieldTypeL( NPresenceInfo::NFieldType::KStatusMessage );
                                    infoField->SetFieldValue( statusTextField );
                                    persInfo->Fields().AddOrReplaceFieldL( infoField );
                                    CleanupStack::Pop( 2 );
                                    }
                                }
                            } //KQualifierXMLTag


                        }  //KStatusTextXMLTag
                    else
                        {
                        // in Yahoo Status text is not coming when user is available
                        MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();
                        MPresenceInfoFieldValueText* statusTextField = prfwObjFact.NewTextInfoFieldLC();
                        statusTextField->SetTextValueL( KNullDesC );
                        infoField->SetFieldTypeL( NPresenceInfo::NFieldType::KStatusMessage );
                        infoField->SetFieldValue( statusTextField );
                        persInfo->Fields().AddOrReplaceFieldL( infoField );
                        CleanupStack::Pop( 2 );
                        }


                    //alias attribute
                    if ( iParser->DecodeL( *iResponse, KAliasXMLTag, ETrue ) )
                        {
                        if ( subParser->DecodeL( iParser->ResultL(), KQualifierXMLTag, EFalse ) )
                            {
                            if ( subParser->ResultL().CompareF( KXMLValueTrue ) == 0 )
                                {
                                if ( subParser->DecodeL( iParser->ResultL(), KPresenceValueXMLTag, EFalse ) )
                                    {
                                    HBufC16* Alias;
                                    TInt countResults( subParser->Count() );
                                    Alias = subParser->ResultAsUnicodeTextL(  );
                                    Alias->Des().Fold();
                                    MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();
                                    MPresenceInfoFieldValueText* aliasField = prfwObjFact.NewTextInfoFieldLC();
                                    aliasField->SetTextValueL( *Alias );
                                    infoField->SetFieldTypeL( KAliasXMLTag );
                                    infoField->SetFieldValue( aliasField );
                                    persInfo->Fields().AddOrReplaceFieldL( infoField );
                                    delete Alias;
                                    CleanupStack::Pop( 2 );
                                    }

                                }
                            }
                        }

                    //status mood

                    if ( iParser->DecodeL( *iResponse, KStatusMood, ETrue ) )
                        {
                        if ( subParser->DecodeL( iParser->ResultL(), KQualifierXMLTag, EFalse ) )
                            {
                            if ( subParser->ResultL().CompareF( KXMLValueTrue ) == 0 )
                                {
                                if ( subParser->DecodeL( iParser->ResultL(), KPresenceValueXMLTag, EFalse ) )
                                    {
                                    HBufC16* StatusMood;
                                    TInt countResults( subParser->Count() );
                                    StatusMood = subParser->ResultAsUnicodeTextL(  );
                                    StatusMood->Des().Fold();
                                    MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();
                                    MPresenceInfoFieldValueText* stausMoodField = prfwObjFact.NewTextInfoFieldLC();
                                    stausMoodField->SetTextValueL( *StatusMood );
                                    infoField->SetFieldTypeL( KStatusMood );
                                    infoField->SetFieldValue( stausMoodField );
                                    persInfo->Fields().AddOrReplaceFieldL( infoField );
                                    delete StatusMood;
                                    CleanupStack::Pop( 2 );
                                    }

                                }
                            }
                        }

                    //client info

                    if ( iParser->DecodeL( *iResponse, KClientInfoXMLTag, ETrue ) )
                        {
                        if ( subParser->DecodeL( iParser->ResultL(), KQualifierXMLTag, EFalse ) )
                            {
                            if ( subParser->ResultL().CompareF( KXMLValueTrue ) == 0 )
                                {

                                //client type
                                if ( subParser->DecodeL( iParser->ResultL(), KClientTypeXMLTag, EFalse ) )
                                    {
                                    HBufC16* ClientType;
                                    TInt countResults( subParser->Count() );
                                    ClientType = subParser->ResultAsUnicodeTextL(  );
                                    ClientType->Des().Fold();
                                    MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();
                                    MPresenceInfoFieldValueText* clientTypeField = prfwObjFact.NewTextInfoFieldLC();
                                    //MPresenceObjectFactory& prfwObjFact = iConnMan.HandleToHost().ProtocolPresenceDataHost().PresenceObjectFactory();
                                    MDevicePresenceInfo* devinfo = prfwObjFact.NewDevicePresenceInfoLC();
                                    clientTypeField->SetTextValueL( *ClientType );
                                    infoField->SetFieldTypeL( KClientTypeXMLTag );
                                    infoField->SetFieldValue( clientTypeField );
                                    devinfo->Fields().AddOrReplaceFieldL( infoField );
                                    devinfo->SetDeviceNameL( KClientTypeMobilePhoneXMLTag );
                                    info->AddDevicePresenceL( devinfo );
                                    delete ClientType;
                                    CleanupStack::Pop( 3 );
                                    }

                                // DevManufacturer


                                if ( subParser->DecodeL( iParser->ResultL(), KDevManufacturerXMLTag, EFalse ) )
                                    {
                                    HBufC16* DevMan;
                                    TInt countResults( subParser->Count() );
                                    DevMan = subParser->ResultAsUnicodeTextL(  );
                                    DevMan->Des().Fold();
                                    MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();
                                    MPresenceInfoFieldValueText* devmanufacturerField = prfwObjFact.NewTextInfoFieldLC();
                                    MDevicePresenceInfo* devinfo = prfwObjFact.NewDevicePresenceInfoLC();
                                    devmanufacturerField->SetTextValueL( *DevMan );
                                    infoField->SetFieldTypeL( KDevManufacturerXMLTag );
                                    infoField->SetFieldValue( devmanufacturerField );
                                    devinfo->Fields().AddOrReplaceFieldL( infoField );
                                    info->AddDevicePresenceL( devinfo );
                                    delete DevMan;
                                    CleanupStack::Pop( 3 );
                                    }
                                //model
                                if ( subParser->DecodeL( iParser->ResultL(), KDeviceModelXMLTag, EFalse ) )
                                    {
                                    HBufC16* DeviceModel;
                                    TInt countResults( subParser->Count() );
                                    DeviceModel = subParser->ResultAsUnicodeTextL(  );
                                    DeviceModel->Des().Fold();
                                    MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();
                                    MPresenceInfoFieldValueText* devicemodelField = prfwObjFact.NewTextInfoFieldLC();
                                    MDevicePresenceInfo* devinfo = prfwObjFact.NewDevicePresenceInfoLC();
                                    devicemodelField->SetTextValueL( *DeviceModel );
                                    infoField->SetFieldTypeL( KDeviceModelXMLTag );
                                    infoField->SetFieldValue( devicemodelField );
                                    devinfo->Fields().AddOrReplaceFieldL( infoField );
                                    info->AddDevicePresenceL( devinfo );
                                    delete DeviceModel;
                                    CleanupStack::Pop( 3 );
                                    }

                                //language

                                if ( subParser->DecodeL( iParser->ResultL(), KClientLanguageXMLTag, EFalse ) )
                                    {
                                    HBufC16* PrefLanguage;
                                    TInt countResults( subParser->Count() );
                                    PrefLanguage = subParser->ResultAsUnicodeTextL(  );
                                    PrefLanguage->Des().Fold();
                                    MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();
                                    MPresenceInfoFieldValueText* LangauageField = prfwObjFact.NewTextInfoFieldLC();
                                    MDevicePresenceInfo* devinfo = prfwObjFact.NewDevicePresenceInfoLC();
                                    LangauageField->SetTextValueL( *PrefLanguage );
                                    infoField->SetFieldTypeL( KClientLanguageXMLTag );
                                    infoField->SetFieldValue( LangauageField );
                                    devinfo->Fields().AddOrReplaceFieldL( infoField );
                                    info->AddDevicePresenceL( devinfo );
                                    delete PrefLanguage;
                                    CleanupStack::Pop( 3 );
                                    }


                                }
                            }
                        }

                    // communication capability

                    RArray<TPtrC8> commCaps;
                    CleanupClosePushL( commCaps );

                    // parse the commcap attribute values
                    if ( iParser->DecodeL( *iResponse, KCommCXMLTag, ETrue ) )
                        {
                        TInt count( iParser->Count() );
                        for ( TInt resCount( 0 ); resCount < count; resCount++ )
                            {
                            User::LeaveIfError( commCaps.Append( iParser->ResultL( resCount ) ) );
                            }

                        count = commCaps.Count();
                        for ( TInt i( 0 ); i < count; i++ )
                            {
                            if ( iParser->DecodeL( commCaps[i], KCommCapXMLTagCap, EFalse ) )
                                {
                                if ( iParser->ResultL().CompareF( KCommCapIMXMLTag ) == 0 )
                                    {
                                    if ( iParser->DecodeL( commCaps[i], KCommCapStatusXMLTag, EFalse ) )
                                        {
                                        if ( iParser->ResultL().CompareF( KCommCapStatusOpen ) == 0 )
                                            {

                                            MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();
                                            MPresenceInfoFieldValueText* StatusField = prfwObjFact.NewTextInfoFieldLC();
                                            MServicePresenceInfo* srvInfo = prfwObjFact.NewServicePresenceInfoLC();
                                            StatusField->SetTextValueL( _L( "OPEN" ) );
                                            infoField->SetFieldTypeL( KCommCapStatusXMLTag );
                                            infoField->SetFieldValue( StatusField );
                                            srvInfo->SetServiceTypeL( NPresenceInfo::NServiceType::KInstantMessaging );
                                            srvInfo->Fields().AddOrReplaceFieldL( infoField );
                                            info->AddServicePresenceL( srvInfo );
                                            CleanupStack::Pop( 3 );
                                            }
                                        else
                                            {

                                            MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();
                                            MPresenceInfoFieldValueText* StatusField = prfwObjFact.NewTextInfoFieldLC();
                                            MServicePresenceInfo* srvInfo = prfwObjFact.NewServicePresenceInfoLC();
                                            StatusField->SetTextValueL( _L( "CLOSED" ) );
                                            infoField->SetFieldTypeL( KCommCapStatusXMLTag );
                                            infoField->SetFieldValue( StatusField );
                                            srvInfo->SetServiceTypeL( NPresenceInfo::NServiceType::KInstantMessaging );
                                            srvInfo->Fields().AddOrReplaceFieldL( infoField );
                                            info->AddServicePresenceL( srvInfo );
                                            CleanupStack::Pop( 3 );
                                            }
                                        }
                                    }
                                }
                            }
                        }

                    CleanupStack::PopAndDestroy(); //commCaps
                    //end of comm cap

                    // status content




                    /*for future use
                    //time zone

                    if(iParser->DecodeL( *iResponse,  KTimeZone, ETrue ))
                        {
                     if(subParser->DecodeL( iParser->ResultL(), KQualifierXMLTag, EFalse))
                    	{
                    		if( subParser->ResultL().CompareF( KXMLValueTrue ) == 0 )
                    		{
                    		 if(subParser->DecodeL( iParser->ResultL(), KPresenceValueXMLTag, EFalse ))
                    		 {
                    		    HBufC16* TimeZone;
                    		 	TInt countResults( subParser->Count() );
                              TimeZone = subParser->ResultAsUnicodeTextL(  );
                              TimeZone->Des().Fold();
                              MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();
                    		 	MPresenceInfoFieldValueText* TimeZoneField = prfwObjFact.NewTextInfoFieldLC();
                    		 	MDevicePresenceInfo* devinfo = prfwObjFact.NewDevicePresenceInfoLC();
                    		 	TimeZoneField->SetTextValueL(*TimeZone);
                    		 	infoField->SetFieldTypeL( KTimeZone);
                              infoField->SetFieldValue( TimeZoneField);
                              devinfo->Fields().AddOrReplaceFieldL( infoField );
                              info->AddDevicePresenceL( devinfo );
                              delete TimeZone;
                              CleanupStack::Pop(3);
                    		 }

                    		}
                    	}
                        }

                    //GeoLocation

                    if(iParser->DecodeL( *iResponse,  KGeoLocation, ETrue ))
                        {
                    	if(subParser->DecodeL( iParser->ResultL(), KQualifierXMLTag, EFalse))
                    	{
                    		if( subParser->ResultL().CompareF( KXMLValueTrue ) == 0 )
                    		{
                    		 if(subParser->DecodeL( iParser->ResultL(), KPresenceValueXMLTag, EFalse ))
                    		 {
                    		    HBufC16* GeoLocation;
                    		 	TInt countResults( subParser->Count() );
                              GeoLocation = subParser->ResultAsUnicodeTextL(  );
                              GeoLocation->Des().Fold();
                              MPresenceInfoField* infoField = prfwObjFact.NewInfoFieldLC();
                    		 	MPresenceInfoFieldValueText* GeoLocField = prfwObjFact.NewTextInfoFieldLC();
                    		 	MDevicePresenceInfo* devinfo = prfwObjFact.NewDevicePresenceInfoLC();
                    		 	GeoLocField->SetTextValueL(*GeoLocation);
                    		 	infoField->SetFieldTypeL( KTimeZone);
                              infoField->SetFieldValue( GeoLocField);
                              devinfo->Fields().AddOrReplaceFieldL( infoField );
                              info->AddDevicePresenceL( devinfo );
                              delete GeoLocation;
                              CleanupStack::Pop(3);
                    		 }

                    		}
                    	}
                        }
                    */
                    info->SetPersonPresenceL( persInfo );
                    CleanupStack::Pop( 1 );//persInfo
                    const TDesC16* contactId;
                    contactId = &( contactIdentity->Identity() );
                    IMPS_DP( D_IMPS_LIT( " CPresenceNotificationHandler::ProcessIncomingDataL:contactId::%S" ), contactId );
                    iConnMan.HandleToHost().ProtocolPresenceDataHost().WatchingDataHost().HandleSubscribedPresentityPresenceL( contactIdentity, info );
                    CleanupStack::Pop( 2 ); //contactidentity,info

                    }  // end of for loop

                CleanupStack::PopAndDestroy( 1 );  //subParser
                } //end of userid if
            }
        CleanupStack::PopAndDestroy( 2 ); //context,aPresClient
        }
    else
        {
        if ( iParser->DecodeL( *iResponse,  KPresenceAuthRequest, ETrue ) )
            {
            IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::PresenceAuth Request :Start" ) );
            iParser->DecodeL( *iResponse, KUserIDXMLTag, EFalse );
            HBufC* AuthreqId;
            MXIMPObjectFactory& ObjFact = iConnMan.HandleToHost().ObjectFactory();
            MPresenceObjectFactory& prfwObjFact = iConnMan.HandleToHost().ProtocolPresenceDataHost().PresenceObjectFactory();

            MPresenceGrantRequestInfo* presenceGrant;
            presenceGrant = prfwObjFact.NewPresenceGrantRequestInfoLC();//1
            MXIMPIdentity* AuthReqIdentity;
            AuthReqIdentity = ObjFact.NewIdentityLC();//2
            AuthreqId = iParser->ResultAsUnicodeTextL();
            AuthReqIdentity->SetIdentityL( *AuthreqId );
            AuthreqId->Des().Fold();
            presenceGrant->SetRequestorIdL( AuthReqIdentity );
            TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
            MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );
            NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );
            serializer->StartTagL( KStatusTag );
            serializer->StartTagL( KResultXMLTag );
            serializer->StartTagL( KCodeXMLTag
                                 ).NarrowTextL( KWVCodeOK
                                              ).EndTagL( KCodeXMLTag );
            serializer->EndTagL( KResultXMLTag );
            serializer->EndTagL(  KStatusTag );
            serializer->EndTagL( KTransactionContent );
            iSendId = iConnMan.DataHandler().SendDataL( iStatus );
            SetActive();
            CleanupStack::PopAndDestroy( 1 ); //serializer
            iConnMan.HandleToHost().ProtocolPresenceDataHost().AuthorizationDataHost().HandlePresenceGrantRequestReceivedL ( presenceGrant );
            TInt count = iConnMan.PresenceAuthGrantReqList()->Count();
            iConnMan.PresenceAuthGrantReqList()->Append( *AuthreqId );
            count = iConnMan.PresenceAuthGrantReqList()->Count();
            delete AuthreqId;
            AuthreqId = NULL;
            CleanupStack::Pop( 2 ); //AuthReqIdentity,presenceGrant
            IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::PresenceAuth Request :End" ) );
            }
        }


    IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::ProcessIncomingDataL End" ) );
    }


// ---------------------------------------------------------------------------
// CPresenceNotificationHandler::RunError
// ---------------------------------------------------------------------------
//
TInt CPresenceNotificationHandler::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::RunError Start %d" ), aError );

    // restartt listening incoming data
    iConnMan.DataHandler().ListenIncomingData( iStatus );
    SetActive();

    IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CPresenceNotificationHandler::StartListeningL
// ---------------------------------------------------------------------------
//
void CPresenceNotificationHandler::StartListeningL()
    {
    IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::StartListeningL Start" ) );

    // Ready to receive notification from the server

    // signal the scheduler
    // Fix warning: C2874W: count may be used before being set
    TInt count = 0;
    if ( !( this->IsActive() ) )
        {
        count = iConnMan.DataHandler().ListenIncomingData( iStatus );
        SetActive();
        }

    for ( TInt x( 0 ) ;  x < count ; ++x )
        {
        HandleIncomingDataL();
        }

    IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::StartListeningL End" ) );
    }

// ---------------------------------------------------------------------------
// CPresenceNotificationHandler::HandleIncomingDataL
// ---------------------------------------------------------------------------
//
void CPresenceNotificationHandler::HandleIncomingDataL()
    {
    IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::HandleIncomingDataL Start" ) );

    ProcessIncomingDataL();

    IMPS_DP( D_IMPS_LIT( "CPresenceNotificationHandler::HandleIncomingDataL End" ) );
    }

// End of file
