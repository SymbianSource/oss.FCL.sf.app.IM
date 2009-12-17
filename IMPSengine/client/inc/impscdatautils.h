/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* const definition class for imps inner data structure.
*
*/


#ifndef TImpsCDataUtils_H
#define TImpsCDataUtils_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include "impsdatautils.h"
#include "impsdataaccessorapi.h"
#include "impsdataaccessor.h"
#include "impsservercommon.h"
#include "impsdetailed.h"
#include "ImpsCommonEnums.h"

// CONSTANTS

const TImpsContent KJoinGroupUserListElements[] = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyJoinGroup_Response,
      EImpsKeyUserList };
                                                          
const TImpsContent KLeaveGroupGroupIdElements[]  = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyLeaveGroup_Response };

const TImpsContent KGroupMembersUserListElements[] = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyGetGroupMembers_Response,
      EImpsKeyUsers };

const TImpsContent KGroupPropElements[]  = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyGetGroupProps_Response };

const TImpsContent KGroupRejectUserListElements[] = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyRejectList_Response,
      EImpsKeyUserList };

const TImpsContent KGroupChangeNoticeElements[]  = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyGroupChangeNotice };

const TImpsContent KGroupChangeNoticeJoinedElements[] = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyGroupChangeNotice,
      EImpsKeyJoined };

const TImpsContent KGroupChangeNoticeLeftElements[] = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyGroupChangeNotice,
      EImpsKeyLeft,
      EImpsKeyUserList };

const TImpsContent KGetPresenceReponseElements[] = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyGetPresence_Response };

const TImpsContent KPresenceNotificationReqElements[]  = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyPresenceNotification_Request };

//<< ------------------ FUNDAMENTAL --------------- >>

//Invite-Request elements
const TImpsContent KInviteRequestElements[] = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyInvite_Request };

//Invite-Request elements
const TImpsContent KInviteCancelRequestElements[] = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyCancelInvite_Request };

//InviteUserResponse
const TImpsContent KInviteCancelUserRequestElements[]  = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyCancelInviteUser_Request };

//InviteUserResponse
const TImpsContent KInviteUserResponseElements[] = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyInviteUser_Response };

//InviteUserResponse
const TImpsContent KInviteUserRequestElements[] = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyInviteUser_Request };

//Invite-Response elements
const TImpsContent KInviteResponseElements[] = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyInvite_Response };

//SearchRequest
const TImpsContent KSearchRequestElements[] = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeySearch_Request };

//SearchResponse
const TImpsContent KSearchResponseElements[] = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeySearch_Response };

//StopSearchRequest
const TImpsContent KSearchStopRequestElements[] = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyStopSearch_Request };

//JoinGroupResponse
const TImpsContent KJoinGroupResponseElements[] = 
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyJoinGroup_Response };

// FORWARD DECLARATIONS
class MImpsKey;
class CImpsCommonGroupPropsInt;
class CImpsPrivateGroupPropsInt;
class TImpsServices;
class TImpsDataUtils;

// DATA TYPES
// This is the way to make class name changes in stages

// TODO check if this can be removed
typedef TImpsDataUtils CImpsDataUtils;

// CLASS DECLARATION

/**
*  A set of static methods that get certain data elements from data accessor. 
*/
class TImpsCDataUtils : public TImpsDataUtils
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        TImpsCDataUtils();
        

    public: // New functions

   
        /**
        * Get recipients from internal data
        * @param aPredecessor, element array, 
        *        parent node of particular Recipient element 
        * @param aAc data accessor
        * @param aGroups Recipient(Group*(GroupID)) output,
        * @param aUsers  Recipient(User*(UserID)) output
        * @param aSnGroups Recipient(Group*(ScreenName(GroupId))) output
        * @param aScreenNs Recipient(Group*(ScreenName(SName))) output
        */
        static void GetRecipientsL(
            MImpsKey* aPredecessor, 
            MImpsDataAccessor* aAc,
            CPtrC16Array* aGroups,        
            CPtrC16Array* aUsers,         
            CPtrC16Array* aSnGroups,       
            CPtrC16Array* aScreenNs );    
    
        /**
        * Set recipients data
        * @param aPredecessor, element array, 
        *        parent node of particular Recipient element
        * @param aAc data accessor  
        * @param aGroups groupID list
        * @param aUserIds userID list
        * @param aSnGroup ScreenName( GroupId )
        * @param aScreenNames ScreenName( SName )
        */
        static void SetRecipientsL(
            MImpsKey* aPredecessor,
            MImpsDataAccessor* aAc,
            const MDesC16Array* aGroups,
            const MDesC16Array* aUserIds,  
            const MDesC16Array* aSnGroups,         
            const MDesC16Array* aScreenNames );    

        /**
        * Set recipients data
        * @param aPredecessor key element path so far 
        * @param aAc message data
        * @param @aSender UserID or SName
        * @param aGroupId ScreenName( GroupdID)
        */
        static void SetSenderL(
            MImpsKey* aPredecessor,
            MImpsDataAccessor* aAc,
            const TDesC* aSender,
            const TDesC* aGroupId );  

        /**
        * Get user list from internal data
        * @param aPredecessor, parent node of particular Recipient element 
        * @param aAc data accessor
        * @param aUsers users output
        * @param aScreenNames screenname output
        */
        static void GetUserListL(
            MImpsKey* aPredecessor,
            MImpsDataAccessor* aAc,
            CDesCArray* aUsers,
            CDesCArray* aScreenNames );

        /**
        * Get Moderators and admins from internal data
        * @param aPredecessor, parent node of particular Recipient element 
        * @param aAc data accessor
        * @param aModerators, moderators output
        * @param aAdmins, Admins output
        */
        static void GetModAndAdmL(
            MImpsKey* aPredecessor,
            MImpsDataAccessor* aAc,
            CDesCArray* aModerators,
            CDesCArray* aAdmins
            );
 
        /**
        * Get users from internal data
        * @param aPredecessor, parent node of particular Recipient element 
        * @param aAc data accessor
        * @param aUsers, users output
        */
        static void GetUsersL(
            MImpsKey* aPredecessor,
            MImpsDataAccessor* aAc,
            CDesCArray* aUsers
            );

        /**
        * Get group properties from internal data
        * @param aPredecessor, parent node of particular Recipient element 
        * @param aAc data accessor
        * @param aCommonP, common properties output
        * @param aPrivateP, private properties output
        */
        static void GetGroupPropertiesL(
            MImpsKey* aPredecessor,
            MImpsDataAccessor* aAc,
            CImpsCommonGroupPropsInt* aCommonP,
            CImpsPrivateGroupPropsInt* aPrivateP
            );


        /**
        * Get only UserIDs from reponse. 
        * @param aKey, key that will be initialized!  
        * @param aAc data accessor
        * @param aBlockEntity list of blocked UserIDs
        * @param aBlockedlistInUse ETrue if block list in use
        * @param aGrantedEntityList list of granted UserIDs
        * @param aGrantedlistInUse ETrue if grant list in use
        */
        static void GetBlockedListResponseL( 
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            CPtrC16Array*                 aBlockEntity,
            TBool&                        aBlockedlistInUse,
            CPtrC16Array*                 aGrantedEntityLis,
            TBool&                        aGrantedlistInUse
            );

        /**
        * Get only UserIDs from reponse. Notice!!!!!
        * @param aKey, key that will be initialized!  
        * @param aAc data accessor
        * @param aBlockEntity list of blocked UserIDs
        * @param aUnBlockEntity list of unblocked UserIDs
        * @param aBlockedlistInUse ETrue if block list in use
        * @param aGrantedEntityList list of granted UserIDs
        * @param aUnGrantEntity list of ungranted UserIDs
        * @param aGrantedlistInUse ETrue if grant list in use
        */
        static void SetBlockedListRequestL( 
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            const MDesCArray* aBlockEntity,
            const MDesCArray* aUnBlockEntity,
            TBool aBlockedListInUse,
            const MDesCArray* aGrantedEntityList,
            const MDesCArray* aUnGrantEntity,
            TBool aGrantedListInUse
            );

        /**
        * Get Detailed result
        * @param aKey, key that will be initialized!  
        * @param aAc data accessor
        * @param ADescription
        * @param ADetailed error descrition
        * @return ETrue if found, else EFalse
        */
        static TBool GetDetailedResultL( 
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            TPtrC& aDescription,
            CImpsDetailed*& aDetailed );

        /**
        * Set ContentType for NewMessage(C-S)
        * @param aAc data accessor
        * @param ContentType 
        */   
        static void SetContentTypeL( 
            MImpsDataAccessor* aAc,
            TPtrC& aContentType );
        
        /**
        * Get ContentEncoding from NewMessage(C-S)
        * @param aAc data accessor
        * @param aContentEncoding 
        */       
        static void GetContentEncodingL( 
            MImpsDataAccessor* aAc,
            TInt& aEncoding );
        
        /**
        * Get UrlList from InviteUserRequest
        * @param aAc data accessor
        * @param aUrlList 
        */       
        static void GetUrlListL(
            MImpsDataAccessor* aAc,
            CDesCArray* aUrlList );
            
        /**
        * Get user list from internal data
        * @param aPredecessor, parent node of particular Recipient element 
        * @param aAc data accessor
        * @param aUsers users output
        * @param aScreenNames screenname output
        */
        static void GetUserMapListL(
            MImpsKey* aPredecessor,
            MImpsDataAccessor* aAc,
            CDesCArray* aUsers,
            CDesCArray* aScreenNames );
            
    private:


    };

#endif        
            
// End of File
