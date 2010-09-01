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
* Description:  Provides methods for CSP WBXML data handling
*
*
*/


// INCLUDE FILES
#include    "ImpsWbXmlData.h"
#include    "ImpsCommonEnums.h"
#include    "ImpsWbXmlCommon.h"
#include    "ImpsPresence.h"
#include    "ImpsFundamental.h"

// ==================== LOCAL FUNCTIONS ====================


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CImpsWbXmlData::CImpsWbXmlData()
    {

    }

// default constructor can leave.
void CImpsWbXmlData::ConstructL()
    {

    }

// Two-phased constructor.
CImpsWbXmlData* CImpsWbXmlData::NewL()
    {
     
    CImpsWbXmlData* self = new (ELeave) CImpsWbXmlData;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    
    }

    
// Destructor
CImpsWbXmlData::~CImpsWbXmlData()
    {
    
    }


// ---------------------------------------------------------
// CImpsWbXmlData::GetWbXmlElementValue
// method returns wbxml element value token corresponding to
// given string
// ---------------------------------------------------------
//
TUint32 CImpsWbXmlData::GetWbXmlElementValue(TInt aToken,TDesC*& aValue)
   {

    switch(aToken)
    {

    case    KImpsWbXmlOnlineStatus:                                             // OnlineStatus
    case    KImpsWbXmlRegistration:                                             // Registration
            if(!aValue->CompareF(KImpsTrue))         return(KImpsWbXmlTrue);         // True
            if(!aValue->CompareF(KImpsFalse))        return(KImpsWbXmlFalse);        // False
            break;

    case    KImpsWbXmlClientInfo:                                               // ClientInfo
            if(!aValue->CompareF(KImpsMobilePhone))  return(KImpsWbXmlMobilePhone);  // MobilePhone
            if(!aValue->CompareF(KImpsComputer))     return(KImpsWbXmlComputer);     // Computer
            if(!aValue->CompareF(KImpsPda))          return(KImpsWbXmlPda);          // Pda
            if(!aValue->CompareF(KImpsCli))          return(KImpsWbXmlCli);          // Cli
            if(!aValue->CompareF(KImpsOther))        return(KImpsWbXmlOther);        // Other
            break;

    case    KImpsWbXmlUserAvailability:                                         // UserAvailabilty
            if(!aValue->CompareF(KImpsAvailable))    return(KImpsWbXmlAvailable); 
            if(!aValue->CompareF(KImpsNotAvailable)) return(KImpsWbXmlNotAvailable); 
            if(!aValue->CompareF(KImpsDiscreet))     return(KImpsWbXmlDiscreet);
            break;

    case    KImpsWbXmlName:                                                     // Name
            if(!aValue->CompareF(KImpsName))             return(KImpsWbXmlNameValue);        // extension Name
            if(!aValue->CompareF(KImpsType))             return(KImpsWbXmlType);             // Type
            if(!aValue->CompareF(KImpsAccessType))       return(KImpsWbXmlAccessType);       // AccessType
            if(!aValue->CompareF(KImpsTopic))            return(KImpsWbXmlTopic);            // Topic
            if(!aValue->CompareF(KImpsPrivateMessaging)) return(KImpsWbXmlPrivateMessaging); // PrivateMessaging
            if(!aValue->CompareF(KImpsSearchable))       return(KImpsWbXmlSearchable);       // Searchable
            if(!aValue->CompareF(KImpsActiveUsers))      return(KImpsWbXmlActiveUsers);      // ActiveUsers
            if(!aValue->CompareF(KImpsMaxActiveUsers))   return(KImpsWbXmlMaxActiveUsers);   // MaxActiveUsers
            if(!aValue->CompareF(KImpsAutoJoin))         return(KImpsWbXmlAutoJoin);         // AutoJoin
            if(!aValue->CompareF(KImpsPrivilegeLevel))   return(KImpsWbXmlPrivilegeLevel);   // PriviledgeLevel
            if(!aValue->CompareF(KImpsAutoDelete))   	 return(KImpsWbXmlAutoDelete);       // AutoDelete
            if(!aValue->CompareF(KImpsValidity))   	 	 return(KImpsWbXmlValidityValue);    // Validity
            break;

    case    KImpsWbXmlAcceptedTransferEncoding:                                         // AcceptedTransferEncoding
    case    KImpsWbXmlContentEncoding:                                                  // ContentEncoding
            if(!aValue->CompareF(KImpsBASE64))           return(KImpsWbXmlBASE64);           // BASE64
            if(!aValue->CompareF(KImpsApplicationVndWapMmsMessage))	return KImpsWbXmlApplicationVndWapMmsMessage;
            if(!aValue->CompareF(KImpsApplicationXSms))	 return KImpsWbXmlApplicationXSms;
            break;

    case    KImpsWbXmlAcceptedContentType:                                              // AcceptedContentType
    case    KImpsWbXmlContentType:                                                      // ContentType
            if(!aValue->CompareF(KImpstextplain))        return(KImpsWbXmlTextPlain);        // text/plain
            break;

    case    KImpsWbXmlValue:                                                            // Value
            if(!aValue->CompareF(KImpsTrue))             return(KImpsWbXmlTrue);             // T
            if(!aValue->CompareF(KImpsFalse))            return(KImpsWbXmlFalse);            // F
            if(!aValue->CompareF(KImpsOpen16))           return(KImpsWbXmlOpen);             // Open
            if(!aValue->CompareF(KImpsRestricted))       return(KImpsWbXmlRestricted);       // Restricted
            if(!aValue->CompareF(KImpsPublic))           return(KImpsWbXmlPublic);           // Public
            if(!aValue->CompareF(KImpsPrivate))          return(KImpsWbXmlPrivate);          // Private
            if(!aValue->CompareF(KImpsAdmin))            return(KImpsWbXmlAdminValue);       // Admin
            if(!aValue->CompareF(KImpsMod))              return(KImpsWbXmlModValue);         // Mod
            break;

        default:
            break;

        }
        
    return(KImpsWbXmlDataNotFound);
    
    }

// ---------------------------------------------------------
// CImpsWbXmlData::GetWbXmlElementValue
// method returns wbxml element value token corresponding to given
// imps engine value and possible max string length
// ---------------------------------------------------------
//
TUint32 CImpsWbXmlData::GetWbXmlElementValue(TInt aToken,TUint32 aValue, TInt& aMaxLength)
   {

    switch(aToken)
    {

    case    KImpsWbXmlSessionType:                              // SessionType
            if(aValue == EImpsInband) return(KImpsWbXmlInband);     // Inband
            if(aValue == EImpsOutband) return(KImpsWbXmlOutband);   // Outband
//            break;

    case    KImpsWbXmlUserID:                           // UserID
    case    KImpsWbXmlName:                             // Name
    case    KImpsWbXmlSessionID:                        // SessionID
    case    KImpsWbXmlGroupID:                          // GroupID
    case    KImpsWbXmlMessageID:                        // MessageID
    case    KImpsWbXmlTransactionID:                    // TransactionID
    case    KImpsWbXmlPassword:                         // Password
    case    KImpsWbXmlSessionCookie:                    // SessionCookie
    case    KImpsWbXmlSName:                            // SName
    case    KImpsWbXmlValue:                            // Value
            aMaxLength = KImpsWbXmlMaxLength50;             // max 50 characters
            break;

    case    KImpsWbXmlSearchString:                     // SearchString
    case    KImpsWbXmlMessageURI:                       // MessageURI
    case    KImpsWbXmlContactList:                      // ContactList
    case    KImpsWbXmlDefaultContactList:               // DefaultContactList
    case    KImpsWbXmlInviteID:                         // InviteID
            aMaxLength = KImpsWbXmlMaxLength100;            // max 100 characters
            break;

    case    KImpsWbXmlDescription:                      // Description
    case    KImpsWbXmlDigestBytes:                      // DigestBytes
    case    KImpsWbXmlNonce:                            // Nonce
            aMaxLength = KImpsWbXmlMaxLength200;            // max 200 characters
            break;

    case    KImpsWbXmlInviteNote:                       // InviteNote
    case    KImpsWbXmlResponseNote:                     // ResponseNote
            aMaxLength = KImpsWbXmlMaxLength400;            // max 400 characters
            break;

    case    KImpsWbXmlTransactionMode:  // TransactionMode
            if(aValue == EImpsRequest) return(KImpsWbXmlRequest);   // Request
            if(aValue == EImpsResponse) return(KImpsWbXmlResponse); // Response
//            break;

    case    KImpsWbXmlInitialDeliveryMethod:            // InitialDeliveryMethod
    case    KImpsWbXmlDeliveryMethod:                   // DeliveryMethod
            if(aValue == EImpsP) return(KImpsWbXmlDeliveryMethodP); // P
            if(aValue == EImpsN) return(KImpsWbXmlDeliveryMethodN); // N
//            break;

    case    KImpsWbXmlPoll:                             // Poll
    case    KImpsWbXmlQualifier:                        // Qualifier
    case    KImpsWbXmlJoinGroup:                        // JoinGroup
    case    KImpsWbXmlJoinedRequest:                    // JoinedRequest
    case    KImpsWbXmlInUse:                            // InUse
    case    KImpsWbXmlCapabilityRequest:                // CapabilityRequest
    case    KImpsWbXmlAllFunctionsRequest:              // AllFunctionsRequest
    case    KImpsWbXmlAnyContent:                       // AnyContent
    case    KImpsWbXmlAcceptance:                       // Acceptance
    case    KImpsWbXmlCompletionFlag:                   // CompletionFlag
    case    KImpsWbXmlDefaultList:                      // DefaultList
    case    KImpsWbXmlDeliveryReport:                   // DeliveryReport
    case    KImpsWbXmlSubscribeNotification:            // SubscribeNotification
    case KImpsWbxmlReceiveList: 
    case KImpsWbxmlAutoSubscribe:
            if(aValue == TRUE) return(KImpsWbXmlTrue);      // True
            if(aValue == FALSE) return(KImpsWbXmlFalse);    // False
//            break;

    case    KImpsWbXmlClientType:                                       // ClientType
            if(aValue == EImpsMOBILE_PHONE) return(KImpsWbXmlMobilePhone);  // True
            if(aValue == EImpsCOMPUTER)     return(KImpsWbXmlComputer);     // Computer
            if(aValue == EImpsPDA)          return(KImpsWbXmlPda);          // Pda
            if(aValue == EImpsCLI)          return(KImpsWbXmlCli);          // Cli
            if(aValue == EImpsOTHER)        return(KImpsWbXmlOther);        // Other
//            break;

    case    KImpsWbXmlInviteType:                                   // InviteType
            if(aValue == EImpsGR)           return(KImpsWbXmlGR);       // GR
            if(aValue == EImpsIM)           return(KImpsWbXmlIM);       // IM
            if(aValue == EImpsPR)           return(KImpsWbXmlPR);       // PR
            if(aValue == EImpsSC)           return(KImpsWbXmlSC);       // SC
//            break;

    case    KImpsWbXmlSupportedBearer:                              // SupportedBearer
            if(aValue == EImpsSMS)          return(KImpsWbXmlSMS);      // SMS
            if(aValue == EImpsWSP)          return(KImpsWbXmlWSP);      // WSP
            if(aValue == EImpsHTTP)         return(KImpsWbXmlHTTP);     // HTTP
            // if(aValue == EImpsHTTPS)     return(KImpsWbXmlHTTPS);    // HTTPS
//            break;

    case    KImpsWbXmlSupportedCIRMethod:                           // SupportedCIRMethod
            if(aValue == EImpsWAPSMS)       return(KImpsWbXmlWAPSMS);   // WAPSMS
            if(aValue == EImpsWAPUDP)       return(KImpsWbXmlWAPUDP);   // WAPUDP
            if(aValue == EImpsSUDP)         return(KImpsWbXmlSUDP);     // SUDP
            if(aValue == EImpsSTCP)         return(KImpsWbXmlSTCP);     // STCP
//            break;

    case    KImpsWbXmlCap:                                          // Cap
            if(aValue == EImpsCapSMS)       return(KImpsWbXmlSMS);      // SMS
            if(aValue == EImpsCapMMS)       return(KImpsWbXmlMMS);      // MMS
            if(aValue == EImpsCapIM)        return(KImpsWbXmlCapIM);    // IM
            if(aValue == EImpsCapEMAIL)     return(KImpsWbXmlEMAIL);    // EMAIL
            if(aValue == EImpsCapCALL)      return(KImpsWbXmlCALL);     // CALL
//            break;

    case    KImpsWbXmlStatus:                                       // Presence Status
            if(aValue == 1) return(KImpsWbXmlOpen);                     // Open
            if(aValue == 2) return(KImpsWbXmlClosed);                   // Closed
//            break;

    case    KImpsWbXmlSubscribeType:                                    // SubscribeType
            if(aValue == EImpsG)                    return(KImpsWbXmlG);    // G
            if(aValue == EImpsS)                    return(KImpsWbXmlS);    // S
            if(aValue == EImpsU)                    return(KImpsWbXmlU);    // U
//            break;

    case    KImpsWbXmlSearchElement:                                                    // SearchElement
            if(aValue == EImpsUserID)               return(KImpsWbXmlUSER_ID);              // USER_ID
            if(aValue == EImpsUserFirstName)        return(KImpsWbXmlUSER_FIRST_NAME);      // USER_FIRST_NAME
            if(aValue == EImpsUserLastName)         return(KImpsWbXmlUSER_LAST_NAME);       // USER_LAST_NAME
            if(aValue == EImpsUserEmailAddress)     return(KImpsWbXmlUSER_EMAIL_ADDRESS);   // USER_EMAIL_ADDRESS
            if(aValue == EImpsUserAlias)            return(KImpsWbXmlUSER_ALIAS);           // USER_ALIAS
            if(aValue == EImpsUserOnlineStatus)     return(KImpsWbXmlUSER_ONLINE_STATUS);   // USER_ONLINE_STATUS
            if(aValue == EImpsUserMobileNumber)     return(KImpsWbXmlUSER_MOBILE_NUMBER);   // USER_MOBILE_NUMBER
            if(aValue == EImpsGroupID)              return(KImpsWbXmlGROUP_ID);             // GROUP_ID
            if(aValue == EImpsGroupName)            return(KImpsWbXmlGROUP_NAME);           // GROUP_NAME
            if(aValue == EImpsGroupTopic)           return(KImpsWbXmlGROUP_TOPIC);          // GROUP_TOPIC
            if(aValue == EImpsGroupUserIDJoined)    return(KImpsWbXmlGROUP_USER_ID_JOINED); // GROUP_USER_ID_JOINED
            if(aValue == EImpsGroupUserIDOwner)     return(KImpsWbXmlGROUP_USER_ID_OWNER);  // GROUP_USER_ID_OWER
//            break;

    case    KImpsWbXmlURL:                                      // URL
            aMaxLength = KImpsWbXmlMaxLength200;                    // max 200 characters
            return(KImpsWbXmlHttp);                                 // http://
//            break;

    case    KImpsWbXmlAcceptedTransferEncoding:                 // AcceptedTransferEncoding
    case    KImpsWbXmlContentEncoding:                          // ContentEncoding
            aMaxLength = KImpsWbXmlMaxLength50;                     // default max length = 50
            if(aValue == EImpsNone) return(KImpsWbXmlNone);         // None
            if(aValue == EImpsBASE64) return(KImpsWbXmlBASE64);     // BASE64
//            break;

    case    KImpsWbXmlContentData:
        case KImpsWbXmlDirectContent:
        case KImpsWbXmlReferredContent:
            aMaxLength = KImpsWbXmlNoMaxLength;                     // no max length
            return(KImpsWbXmlDataNotFound);
//            break;

        default:
            aMaxLength = KImpsWbXmlNoMaxLength;                     // no max length
//            return(KImpsWbXmlDataNotFound);
            break;
        }

        return(KImpsWbXmlDataNotFound);
   }


// ---------------------------------------------------------
// CImpsWbXmlData::GetImpsEngineValue
// method returns Imps engine value corresponding to given
// wbxml element value token if exists
// ---------------------------------------------------------
//
TInt CImpsWbXmlData::GetImpsEngineValue(TInt aToken,TInt aValue)
   {


    switch(aToken)
    {

    case    KImpsWbXmlSessionType:                              // SessionType
            if(aValue == KImpsWbXmlInband)  return(EImpsInband);    // Inband
            if(aValue == KImpsWbXmlOutband) return(EImpsOutband);   // Outband
            break;

    case    KImpsWbXmlTransactionMode:  // TransactionMode
            if(aValue == KImpsWbXmlRequest)  return(EImpsRequest);  // Request
            if(aValue == KImpsWbXmlResponse) return(EImpsResponse); // Response
            break;

    case    KImpsWbXmlInitialDeliveryMethod:                        // InitialDeliveryMethod
    case    KImpsWbXmlDeliveryMethod:                               // DeliveryMethod
            if(aValue == KImpsWbXmlDeliveryMethodP)  return(EImpsP);    // P
            if(aValue == KImpsWbXmlDeliveryMethodN)  return(EImpsN);    // N
            break;

    case    KImpsWbXmlPoll:                                 // Poll
    case    KImpsWbXmlQualifier:                            // Qualifier
    case    KImpsWbXmlCapabilityRequest:                    // CapabilityRequest
    case    KImpsWbXmlAllFunctionsRequest:                  // AllFunctionsRequest
    case    KImpsWbXmlJoinGroup:                            // JoinGroup
    case    KImpsWbXmlJoinedRequest:                        // JoinedRequest
    case    KImpsWbXmlInUse:                                // InUse
    case    KImpsWbXmlAnyContent:                           // AnyContent
    case    KImpsWbXmlAcceptance:                           // Acceptance
    case    KImpsWbXmlCompletionFlag:                       // CompletionFlag
    case    KImpsWbXmlDefaultList:                          // DefaultList
    case    KImpsWbXmlDeliveryReport:                       // DeliveryReport
    case    KImpsWbXmlSubscribeNotification:                // SubscribeNotification
            if(aValue == KImpsWbXmlTrue)  return(ETrue);        // True
            if(aValue == KImpsWbXmlFalse) return(EFalse);       // False
            break;

    case    KImpsWbXmlClientType:           // ClientType
            if(aValue == KImpsWbXmlMobilePhone) return(EImpsMOBILE_PHONE);  // MobilePhone
            if(aValue == KImpsWbXmlComputer)    return(EImpsCOMPUTER);      // Computer
            if(aValue == KImpsWbXmlPda)         return(EImpsPDA);           // Pda
            if(aValue == KImpsWbXmlCli)         return(EImpsCLI);           // Cli
            if(aValue == KImpsWbXmlOther)       return(EImpsOTHER);         // Other
            break;

    case    KImpsWbXmlInviteType:           // InviteType
            if(aValue == KImpsWbXmlGR )         return(EImpsGR);        // GR
            if(aValue == KImpsWbXmlIM )         return(EImpsIM);        // IM
            if(aValue == KImpsWbXmlPR )         return(EImpsPR);        // PR
            if(aValue == KImpsWbXmlSC )         return(EImpsSC);        // SC
            break;

    case    KImpsWbXmlSupportedBearer:      // SupportedBearer
            if(aValue == KImpsWbXmlSMS )        return(EImpsSMS);       // SMS
            if(aValue == KImpsWbXmlWSP )        return(EImpsWSP);       // WSP
            if(aValue == KImpsWbXmlHTTP )       return(EImpsHTTP);      // HTTP
            // if(aValue == KImpsWbXmlHTTPS )       return(EImpsHTTPS);     // HTTPS
            break;

    case    KImpsWbXmlSupportedCIRMethod:   // SupportedCIRMethod
            if(aValue == KImpsWbXmlWAPSMS)      return(EImpsWAPSMS);    // WAPSMS
            if(aValue == KImpsWbXmlWAPUDP)      return(EImpsWAPUDP);    // WAPUDP
            if(aValue == KImpsWbXmlSUDP)        return(EImpsSUDP);      // SUDP
            if(aValue == KImpsWbXmlSTCP)        return(EImpsSTCP);      // STCP
            break;

    case    KImpsWbXmlSubscribeType:        // SubscribeType
            if(aValue == KImpsWbXmlG)           return(EImpsG);         // G
            if(aValue == KImpsWbXmlS)           return(EImpsS);         // S
            if(aValue == KImpsWbXmlU)           return(EImpsU);         // U
            break;

    case    KImpsWbXmlSearchElement:        // SearchElement
            if(aValue == KImpsWbXmlUSER_ID )                return(EImpsUserID);                // USER_ID
            if(aValue == KImpsWbXmlUSER_FIRST_NAME )        return(EImpsUserFirstName);         // USER_FIRST_NAME
            if(aValue == KImpsWbXmlUSER_LAST_NAME )         return(EImpsUserLastName);          // USER_LAST_NAME
            if(aValue == KImpsWbXmlUSER_EMAIL_ADDRESS )     return(EImpsUserEmailAddress);      // USER_EMAIL_ADDRESS
            if(aValue == KImpsWbXmlUSER_ALIAS )             return(EImpsUserAlias);             // USER_ALIAS
            if(aValue == KImpsWbXmlUSER_ONLINE_STATUS )     return(EImpsUserOnlineStatus);      // USER_ONLINE_STATUS
            if(aValue == KImpsWbXmlUSER_MOBILE_NUMBER )     return(EImpsUserMobileNumber);      // USER_MOBILE_NUMBER
            if(aValue == KImpsWbXmlGROUP_ID )               return(EImpsGroupID);               // GROUP_ID
            if(aValue == KImpsWbXmlGROUP_NAME )             return(EImpsGroupName);             // GROUP_NAME
            if(aValue == KImpsWbXmlGROUP_TOPIC )            return(EImpsGroupTopic);            // GROUP_TOPIC
            if(aValue == KImpsWbXmlGROUP_USER_ID_JOINED )   return(EImpsGroupUserIDJoined);     // GROUP_USER_ID_JOINED
            if(aValue == KImpsWbXmlGROUP_USER_ID_OWNER )    return(EImpsGroupUserIDOwner);      // GROUP_USER_ID_OWER
            break;

    case    KImpsWbXmlAcceptedTransferEncoding:                 // AcceptedTransferEncoding
    case    KImpsWbXmlContentEncoding:                          // ContentEncoding
            if(aValue ==KImpsWbXmlNone )    return(EImpsNone);      // None
            if(aValue ==KImpsWbXmlBASE64 )  return(EImpsBASE64);    // BASE64 
            break;

    case    KImpsWbXmlCap:                  // Cap
            if(aValue == KImpsWbXmlSMS)     return(EImpsCapSMS);    // SMS
            if(aValue == KImpsWbXmlMMS)     return(EImpsCapMMS);    // MMS
            if(aValue == KImpsWbXmlCapIM)   return(EImpsCapIM);     // IM / (CommC/Cap)
            if(aValue == KImpsWbXmlIM)
            {
                TInt value = EImpsCapIM;
                if((aToken & KImpsMaskCodePage) == KImpsCodePagePresence)
                {
                    value = EImpsCapIM;
                }
                return(value);
            }
            if(aValue == KImpsWbXmlEMAIL)   return(EImpsCapEMAIL);  // EMAIL
            if(aValue == KImpsWbXmlCALL)    return(EImpsCapCALL);   // CALL
            break;

    case    KImpsWbXmlStatus:                                   // Presence Status
            if(aValue == KImpsWbXmlOpen)    return(1);              // Open
            if(aValue == KImpsWbXmlClosed)  return(2);              // Closed
            break;

        default:
            break;
        }

        return(KErrNotFound);   
   }


// ---------------------------------------------------------
// CImpsWbXmlData::GetWbXmlStringValue
// method returns possible string corresponding to given token
// ---------------------------------------------------------
//
TPtrC8 CImpsWbXmlData::GetWbXmlStringValue(TInt aToken,TInt aValue)
   {
        
    TPtrC8 p(KNullDesC8);
    switch(aToken)
    {

    case    KImpsWbXmlDigestSchema:                                    // DigestSchema
            if(aValue == EImpsPWD)          return TPtrC8(KImpsPWD);       // PWD
            if(aValue == EImpsSHA)          return TPtrC8(KImpsSHA);       // SHA
            if(aValue == EImpsMD4)          return TPtrC8(KImpsMD4);       // MD4
            if(aValue == EImpsMD5)          return TPtrC8(KImpsMD5);       // MD4
            if(aValue == EImpsMD6)          return TPtrC8(KImpsMD6);       // MD4
            break;

    case    KImpsWbXmlSupportedBearer:                                 // SupportedBearer
            if(aValue == EImpsHTTPS)        return TPtrC8(KImpsHTTPS);     // HTTPS
            break;

        default:
            return(p);   
        }
        return(p);
   }

// ---------------------------------------------------------
// CImpsWbXmlData::GetImpsEngineDesc8Value
// method returns possible prefix string corresponding to given token
// ---------------------------------------------------------
//
TPtrC8 CImpsWbXmlData::GetImpsEngineDesc8Value(TInt aToken,TInt aValue)
   {
        
    TPtrC8 p(KNullDesC8);
    switch(aToken)
    {

    case    KImpsWbXmlURL:          // URL
            if(aValue == KImpsWbXmlHttp)    return TPtrC8(KImpsHttp8);     // http
            if(aValue == KImpsWbXmlHttps)   return TPtrC8(KImpsHttps8);    // Https
            break;
        
    default:    
        return(p);   
        
    }

    return(p);
   
    }

// ---------------------------------------------------------
// CImpsWbXmlData::GetImpsEngineDescValue
// method returns possible string corresponding to given token
// ---------------------------------------------------------
//
TPtrC CImpsWbXmlData::GetImpsEngineDescValue(TInt aToken,TInt aValue)
   {
    
    TPtrC p(KNullDesC);
    
    switch(aToken)
    {
        
    case    KImpsWbXmlOnlineStatus:                                        // OnlineStatus
    case    KImpsWbXmlRegistration:                                        // Registration
            if(aValue == KImpsWbXmlTrue)        return TPtrC(KImpsTrue);       // True
            if(aValue == KImpsWbXmlFalse)       return TPtrC(KImpsFalse);      // False
            break;

    case    KImpsWbXmlClientInfo:                                           // ClientInfo
            if(aValue == KImpsWbXmlMobilePhone) return TPtrC(KImpsMobilePhone); // MobilePhone
            if(aValue == KImpsWbXmlComputer)    return TPtrC(KImpsComputer);    // Computer
            if(aValue == KImpsWbXmlPda)         return TPtrC(KImpsPda);         // Pda
            if(aValue == KImpsWbXmlCli)         return TPtrC(KImpsCli);         // Cli
            if(aValue == KImpsWbXmlOther)       return TPtrC(KImpsOther);       // Other
            break;

    case    KImpsWbXmlUserAvailability:                                     // UserAvailabilty
            if(aValue == KImpsWbXmlAvailable)   return TPtrC(KImpsAvailable);          // 
            if(aValue == KImpsWbXmlNotAvailable) return TPtrC(KImpsNotAvailable);     // 
            if(aValue == KImpsWbXmlDiscreet)    return TPtrC(KImpsDiscreet);          // 
            break;

    case    KImpsWbXmlURL:                                                  // URL
            if(aValue == KImpsWbXmlHttp)        return TPtrC(KImpsHttp);        // Http
            if(aValue == KImpsWbXmlHttps)       return TPtrC(KImpsHttps);       // Https
            break;

    case    KImpsWbXmlName:                                                 // Name
            if(aValue == KImpsWbXmlNameValue)      return TPtrC(KImpsName);        // extension Name
            if(aValue == KImpsWbXmlType)           return TPtrC(KImpsType);        // Type
            if(aValue == KImpsWbXmlAccessType)     return TPtrC(KImpsAccessType);  // Type
            if(aValue == KImpsWbXmlTopic)          return TPtrC(KImpsTopic);       // Topic
            if(aValue == KImpsWbXmlPrivateMessaging)    return TPtrC(KImpsPrivateMessaging);    // PrivateMessaging
            if(aValue == KImpsWbXmlSearchable)     return TPtrC(KImpsSearchable);   // Searchable
            if(aValue == KImpsWbXmlActiveUsers)    return TPtrC(KImpsActiveUsers);  // ActiveUsers
            if(aValue == KImpsWbXmlMaxActiveUsers) return TPtrC(KImpsMaxActiveUsers);  // MaxActiveUsers
            if(aValue == KImpsWbXmlAutoJoin)       return TPtrC(KImpsAutoJoin);        // AutoJoin
            if(aValue == KImpsWbXmlPrivilegeLevel) return TPtrC(KImpsPrivilegeLevel);    // PriviledgeLevel
            if(aValue == KImpsWbXmlDisplayName)    return TPtrC(KImpsDisplayName);      // DisplayName
            if(aValue == KImpsWbXmlDefault)        return TPtrC(KImpsDefault);          // Default
            if(aValue == KImpsWbXmlAutoDelete)     return TPtrC(KImpsAutoDelete);       // AutoDelete
            if(aValue == KImpsWbXmlValidityValue)       return TPtrC(KImpsValidity);         // Validity
             
            break;

    case    KImpsWbXmlValue:                                                // Value
            if(aValue == KImpsWbXmlTrue)        return TPtrC(KImpsTrue);        // T
            if(aValue == KImpsWbXmlFalse)       return TPtrC(KImpsFalse);       // F
            if(aValue == KImpsWbXmlOpen)        return TPtrC(KImpsOpen16);      // Open
            if(aValue == KImpsWbXmlRestricted)  return TPtrC(KImpsRestricted);  // Restricted
            if(aValue == KImpsWbXmlPublic)      return TPtrC(KImpsPublic);      // Public
            if(aValue == KImpsWbXmlPrivate)     return TPtrC(KImpsPrivate);     // Private
            if(aValue == KImpsWbXmlAdminValue)  return TPtrC(KImpsAdmin);       // Admin
            if(aValue == KImpsWbXmlModValue)    return TPtrC(KImpsMod);         // Mod
            break;

    case    KImpsWbXmlContentType:                                          // ContentType
    case    KImpsWbXmlAcceptedContentType:                                  // AcceptedContentType
            if(aValue == KImpsWbXmlTextPlain)   return TPtrC(KImpstextplain);   // text/plain
            if (aValue == KImpsWbXmlApplicationVndWapMmsMessage)	return TPtrC(KImpsApplicationVndWapMmsMessage);
            if (aValue == KImpsWbXmlApplicationXSms)	return TPtrC(KImpsApplicationXSms);
            break;
    
    default:
            if ( aValue == KImpsWbXmlIM ) 
                {
                return TPtrC(KImpsIM);
                }
            return(p);   

    }
    
    return(p);
   
    }

// ---------------------------------------------------------
// CImpsWbXmlData::GetImpsEngineValue
// method returns imps engine value corresponding to
// given string
// ---------------------------------------------------------
//
TUint32 CImpsWbXmlData::GetImpsEngineValue(TInt aToken,TPtrC8 aValue)
   {

    
    switch(aToken)
    {

    case    KImpsWbXmlStatus:                                  // Status
            if(!aValue.CompareF(TPtrC8(KImpsOpen)))    return(1);   // True
            if(!aValue.CompareF(TPtrC8(KImpsClosed)))  return(2);   // False
            break;

    case    KImpsWbXmlDigestSchema:                                 // DigestSchema
            if(!aValue.CompareF(TPtrC8(KImpsPWD)))     return(EImpsPWD); // PWD
            if(!aValue.CompareF(TPtrC8(KImpsSHA)))     return(EImpsSHA); // SHA
            if(!aValue.CompareF(TPtrC8(KImpsMD4)))     return(EImpsMD4); // MD4
            if(!aValue.CompareF(TPtrC8(KImpsMD5)))     return(EImpsMD5); // MD5
            if(!aValue.CompareF(TPtrC8(KImpsMD6)))     return(EImpsMD6); // MD6
            break;

    case    KImpsWbXmlSupportedBearer:                                 // SupportedBearer
            if(!aValue.CompareF(TPtrC8(KImpsHTTPS)))   return(EImpsHTTPS);  // HTTPS
            break;
        
    case    KImpsWbXmlContentEncoding:
    case	KImpsWbXmlAcceptedTransferEncoding:
        if(!aValue.CompareF(TPtrC8(KImpsBASE64_8)))   return(EImpsBASE64); // BASE64
        if(!aValue.CompareF(TPtrC8(KImpsNone_8)))   return(EImpsNone);  // None
        break;
            
    default:
            break;
    
    }

    return(KImpsWbXmlDataNotFound);

    }


// ---------------------------------------------------------
// CImpsWbXmlData::GetWbXmlAttributeTokenValue
// method returns attribute token value (integer)
// ---------------------------------------------------------
//
TUint CImpsWbXmlData::GetWbXmlAttributeTokenValue(TInt aToken, TImpsCspVersion aCspVersion)
   {
    
    switch(aToken)
    {
        case    KImpsWbXmlWV_CSP_Message:       // WV-CSP-Message
            return aCspVersion == EImpsCspVersion11 ? KImpsWbXmlCsp11Xmlns : KImpsWbXmlCsp12Xmlns;

        case    KImpsWbXmlPresenceSubList:      // PresenceSubList
            return aCspVersion == EImpsCspVersion11 ? KImpsWbXmlPa11Xmlns : KImpsWbXmlPa12Xmlns;

        case    KImpsWbXmlTransactionContent:   // TransactionContent
            return aCspVersion == EImpsCspVersion11 ? KImpsWbXmlTrc11Xmlns : KImpsWbXmlTrc12Xmlns;
            
//    case KImpsWbXmlExtBlock: 
            
        default:
            break;
        
    }
    
    return(0);   
   
    }

// ---------------------------------------------------------
// CImpsWbXmlData::GetWbXmlAttributeStringValue
// method returns attribute token value (string)
// ---------------------------------------------------------
//
TPtrC8 CImpsWbXmlData::GetWbXmlAttributeStringValue(TInt aToken, TImpsCspVersion aCspVersion)
   {
    
    TPtrC8 p(KNullDesC8);
    
    switch(aToken)
    {
        case    KImpsWbXmlWV_CSP_Message:       // WV-CSP-Message
        case    KImpsWbXmlPresenceSubList:      // PresenceSubList
        case    KImpsWbXmlTransactionContent:   // TransactionContent
            return aCspVersion == EImpsCspVersion11 ? TPtrC8(KImpsMessageVersion11): TPtrC8(KImpsMessageVersion12);

            default:
            break;
        
    }
    
    return(p);   
   
    }

/*
const TDesC8& CImpsWbXmlData::GetAttributeStringByToken(TInt aToken)
    {
    switch (aToken) 
        {
        case KImpsWbXmlTrc12Xmlns:
            return KImpsTRC12Xmlns();
            break;
        case KImpsWbXmlPa12Xmlns:
            return KImpsPA12Xmlns();
            break;

        case KImpsWbXmlCsp12Xmlns:
            return KImpsCSP12Xmlns();
        default:;
        }
    }
*/
//  End of File  

