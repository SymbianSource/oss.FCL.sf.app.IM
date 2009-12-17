/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
* logger for imps client.
*
*/


// INCLUDE FILES
#include    <e32std.h>
#ifdef _DEBUG
#include    <flogger.h>
#endif
#include    "impsutils.h"
#include    "impsservercommon.h"
#include    "impsservices.h"
#include    "ImpsVariantAPI.h"


// ================= MEMBER FUNCTIONS =======================



//**********************************
// CImpsClientLogger
//**********************************
const TInt KLogBufferLength = 356;
//_LIT(KLogDir, "impsc");
//_LIT(KLogFile, "impsc.txt");

EXPORT_C void CImpsClientLogger::Log(TRefByValue<const TDesC> aFmt,...)
    {
    VA_LIST list;
    VA_START(list, aFmt);

    // Print to log file
    TBuf<KLogBufferLength> buf;
    buf.FormatList(aFmt, list);

#ifdef _DEBUG
    _LIT(KLogDir, "impsc");
    _LIT(KLogFile, "impsc.txt");
    // Write to log file
    RFileLogger::Write(KLogDir, KLogFile, EFileLoggingModeAppend, buf);
    RDebug::RawPrint( buf );    
#endif

    }


//**********************************
// CImpsSettingFile
//**********************************

EXPORT_C CImpsSettingFile* CImpsSettingFile::NewL( 
    RFs& aFs )
    {
    CImpsSettingFile* self = new (ELeave) CImpsSettingFile( aFs );
    return self;
    }

EXPORT_C CImpsSettingFile::~CImpsSettingFile()
    {
    if ( iOpen )
        {
        iReader.Close();
        }
    iOpen = EFalse;
    }

CImpsSettingFile::CImpsSettingFile( RFs& aFs)
: iFs( aFs ),
  iOpen( EFalse )
    {

    }

EXPORT_C void CImpsSettingFile::OpenL(TDesC& aResFile)
    {
    // open a file
    iFileName = aResFile;

    TInt myError = iReader.Open( iFs, 
                             iFileName, 
                             EFileShareReadersOnly );

    User::LeaveIfError( myError );
    iOpen = ETrue;
    
    }

EXPORT_C TPtrC8 CImpsSettingFile::KeyValueL( const TDesC8& aKey )
    {

    TPtrC8 myKey;
    TPtrC8 myValue;
    TBool getIt(EFalse);
    TInt err = 0;

    // Reset the reader
    OpenL( iFileName );

    // Start to search
    while ( !getIt) 
        {
        TRAP ( err, ReadRowL( myKey, myValue ));
        if ( err != KErrNone )
            {
            User::Leave( KErrNotFound );
            }
        if ( !myKey.CompareF( aKey ))
            {
            return myValue;
            }
        }
    return TPtrC8();
    }

void CImpsSettingFile::ReadRowL( TPtrC8& aKey, TPtrC8& aValue )
    {
    // READ ONE ROW
    TChar delim( 10 );
    iReader.ReadL( iRowBuffer, delim);
    TInt length = iRowBuffer.Length();
    if ( length > 2 )
        {
        // DROP CR+LF FROM THE END OF LINE
        iRowBuffer.Delete( length - 2, 2 );

        TInt pos = 0;
        pos = iRowBuffer.Find( _L8("=") );
        if ( pos > 0 )
            {
            aKey.Set( iRowBuffer.Left( pos ) );
            // Cut off separator
            aValue.Set( iRowBuffer.Mid( pos + 1 ) );
            }
        }
    }

//**********************************
// impsService
//**********************************

EXPORT_C GLDEF_C TImpsEventType impsService( const CImpsVariant* /*aVariant*/,
    TImpsMessageType aMessage )
    {
    TImpsEventType event(EImpsEventNone);
    // Notice: update this table when new methods supported


    switch ( aMessage )
        {
        case EImpsLoginReq:
        case EImpsLoginRes:
        case EImpsLogoutReq:
        case EImpsDisconnect:
        case EImpsClientCapabilityReq:   // this is not mistake,
                                         // part of login procedure
        case EImpsServiceReq:            // Same here.
            event = EImpsEventServerLogin;
            break;
        // IM
        case EImpsSendMessageReq:
        case EImpsSendMessageRes:
        case EImpsGetMessageReq:
        case EImpsNewMessage:
        case EImpsMessageDelivered:
        case EImpsGetBlockedListReq:
        case EImpsGetBlockedListRes:
        case EImpsBlockEntityReq:
        case EImpsDeliveryReportReq:
            event = EImpsEventMessage;
            break;
        // Presence
        case EImpsSubscribePresenceReq:
        case EImpsUnsubscribePresenceReq:
        case EImpsGetPresenceReq:
        case EImpsGetPresenceRes:
        case EImpsUpdatePresenceReq:
        case EImpsPresenceNotification:
        case EImpsCreateAttrListReq:
        case EImpsPureData:
        case EImpsGetWatcherListReq:
        case EImpsGetWatcherListRes:
        case EImpsPresenceAuthReq:
        case EImpsPresenceAuthUser:
        case EImpsCancelAuthReq:
        case EImpsGetListReq:
        case EImpsGetListRes:
        case EImpsCreateListReq:
        case EImpsDeleteListReq:
        case EImpsListManageReq:
        case EImpsListManageRes:
        case EImpsDeleteAttrListReq:
        case EImpsGetAttrListReq:
        case EImpsGetAttrListRes:
            event = EImpsEventPresence;
            break;
        // Group
        case EImpsCreateGroupReq:
        case EImpsDeleteGroupReq:
        case EImpsJoinGroupReq:
        case EImpsJoinGroupRes:
        case EImpsLeaveGroupReq:
        case EImpsLeaveGroupRes:
        case EImpsGroupMembersReq:
        case EImpsGroupMembersRes:
        case EImpsAddGroupMembersReq:
        case EImpsRemoveGroupMembersReq:
        case EImpsGroupMemberAccessReq:
        case EImpsGroupPropertiesReq:
        case EImpsGroupPropertiesRes:
        case EImpsSetGroupPropertiesReq:
        case EImpsGroupRejectListReq:
        case EImpsGroupRejectListRes:
        case EImpsSubsGroupNoticeReq:
        case EImpsSubsGroupNoticeRes:
        case EImpsGroupChangeNotice:
        case EImpsSubsGroupNoticeUnset:  
        case EImpsSubsGroupNoticeGet: 
        case EImpsSubsGroupNoticeSet:   
        case EImpsNewUsers:
        case EImpsLeftUsers:    
            event = EImpsEventGroup;
            break;
        // Fundamental
        case EImpsSearchReq:
        case EImpsStopSearchReq:
        case EImpsSearchRes:
        case EImpsInviteReq:
        case EImpsInviteRes:
        case EImpsInviteUserReq:
        case EImpsInviteUserRes:
        case EImpsCancelInviteReq:
        case EImpsCancelInviteUserReq:
            event = EImpsEventCommon;
            break;
        default:
            event = EImpsEventNone;
        }
    return event;
    }
  
//  End of File  

