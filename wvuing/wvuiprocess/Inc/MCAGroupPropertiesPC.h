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
* Description:  Interface for PC Group properties dialog
*
*/


#ifndef MCAGGROUPPROPERTIESPC_H
#define MCAGGROUPPROPERTIESPC_H

#include "TEnumsPC.h"

//class declaration
/**
 *  @lib wvuiprocessng.lib
 *  @since 3.2
 */
class MCAGroupPropertiesPC
    {

    public:
        /**
        * virtual destructor
        */
        virtual ~MCAGroupPropertiesPC()
            {
            };


    public:

        /* To find the total no. of groups the user has joined in the current session
         * @return integer: total no. of group joined by the user in this session
         */
        virtual TInt NumberOfCreatedGroupsDuringSession() = 0;


        /**
        * Identification of contact. This is resolved from known
        * identifications.
        * Alias, nickname and contactId.
        * Nickname is shown if set.
        * @param aContactId Id of contact which identification needed.
        * @return Identification of contact.
        *      Alias if nickname is not set and alias is enabled.
        *      Wvid without domain and prefix if nickname does not
             exists and alias is not enabled or not exists.
        */
        virtual const TPtrC GetIdentificationL( const TDesC& aContactId ) = 0;


        /*
         * The active group's topic . The active groups properties are fetched into
         * a member object of class CImpsCommonGroupProps via the InitializeForEditting(groupid) method
         * @return topic
         */
        virtual TPtrC GetTopic() const = 0;

        /* The active group's welcome note.The active groups properties are fetched into
        * a member object of class CImpsCommonGroupProps via the InitializeForEditting(groupid) method
         * @return welcome note
         */
        virtual TPtrC GetWelcomeNote( ) const = 0;

        /* The active group's max no. of users. The active groups properties are fetched into
        * a member object of class CImpsCommonGroupProps via the InitializeForEditting(groupid) method
         * @return integer: maximum no. of users
         */
        virtual TInt GetMaxNbrOfUsers() const = 0;

        /* The active group's searchable property. The active groups properties are fetched into
        * a member object of class CImpsCommonGroupProps via the InitializeForEditting(groupid) method
        */
        virtual TEnumsPC::TImpsPropertyBoolPC IsSearchable( )  = 0;

        /* The ID of the group to be editted or created is passed on to this method during initialization
         * explicitly. This will update the member variable of class CImpsCommonGroupProps with all the
         * properties if its edit mode, else with default values if its create mode.
         */
        virtual TInt InitializeForEdittingL( const TDesC& aGroupId ) = 0;

        /*
         * To find if the user has joined the group with group id aGroupId
         * @param aGroupId: id of the group to which we have to verify if the user has joined.
         * @return bool: Return true if the user has joined the group, else return false.
         */
        virtual TBool IsJoined( const TDesC& aGroupId ) const = 0;

        /*
         * To find if the user is the owner/creator of the group with group id aGroupId
         * @param: The id of the group against whom the ownership/creator rights of the
         * user has to be verified.
         * @return bool: Return True if the user is the owner/creator of aGroupId else return false.
         */
        virtual TBool IsOwnGroup( const TDesC& aGroupId ) const  = 0;

        /*
         * To find if the user is the admin of the group with group id aGroupId
         * @param: The id of the group against whom the admin rights of the user has to be verified.
         * @return bool: Return True if the user is the admin of aGroupId else return false.
         */
        virtual TBool IsAdmin( const TDesC& aGroupId ) const  = 0;


        /*
        * To set the Group Name property of a group. The set of the property is
        * not done to the storage/engine directly but to the instance of
        * CImpsCommonGroupProps class. This instance is not owned by this class.
        * And it will be queried by the caller of Edit Chat Group Properties
        * ie., CCAGroupPropertiesDialog to get the changed settings. The caller
        * will then Save Changes to the storage component.
        * @param TDesC: the editted group name
        */
        virtual void SetGroupNameL( const TDesC& aName )  = 0;

        /*
         * To set the max no. of users for the group. The set of the property is
         * not done to the storage/engine directly but to the instance of
         * CImpsCommonGroupProps class. This instance is not owned by this class.
         * And it will be queried by the caller of Edit Chat Group Properties
         * ie., CCAGroupPropertiesDialog to get the changed settings. The caller
         * will then Save Changes to the storage component.
         * @param integer: The max no. of users to be set
         */
        virtual void SetMaxNbrOfUsers( TInt aSize )  = 0;


        /*
         * To set the Topic property of a group. The set of the property is not
         * done to the storage/engine directly but to the instance of
         * CImpsCommonGroupProps class. This instance is not owned by this class.
         * And it will be queried by the caller of Edit Chat Group Properties
         * ie., CCAGroupPropertiesDialog to get the changed settings. The caller
         * will then Save Changes to the storage component.
         * @param TDesC: the editted Topic
         */
        virtual void SetTopicL( const TDesC& aTopic )  = 0;


        /*
         * To set the Welcome note property of a group. The set of the property is
         * not done to the storage/engine directly but to the instance of
         * CImpsCommonGroupProps class. This instance is not owned by this class.
         * And it will be queried by the caller of Edit Chat Group Properties
         * ie., CCAGroupPropertiesDialog to get the changed settings. The caller
         * will then Save Changes to the storage component.
         * @param TDesC: the editted welcome note
         */
        virtual void SetWelcomeL( const TDesC& aWelcomeNote )  = 0;


        /*
         * To set the Private Messages allowed property of a group. The set of the
         * property is not done to the storage/engine directly but to the instance
         * of CImpsCommonGroupProps class. This instance is not owned by this class.
         * And it will be queried by the caller of Edit Chat Group Properties
         * ie., CCAGroupPropertiesDialog to get the changed settings. The caller will
         * then Save Changes to the storage component.
         * @param TImpsPropertyBoolPC: EImpsPropYes if the property has to be set
         * else EImpsPropNo
         */
        virtual void SetPrivateAllowed( TEnumsPC::TImpsPropertyBoolPC aValue ) = 0;


        /*
         * To set the Searchable property of a group. The set of the property is
         * not done to the storage/engine directly but to the instance of
         * CImpsCommonGroupProps class. This instance is not owned by this class.
         * And it will be queried by the caller of Edit Chat Group Properties
         * ie., CCAGroupPropertiesDialog to get the changed settings. The caller
         * will then Save Changes to the storage component.
         * @param TImpsPropertyBoolPC: EImpsPropYes if the property has to be set
         * else EImpsPropNo
         */
        virtual void SetSearchable( TEnumsPC::TImpsPropertyBoolPC aType )  = 0;

        /*
         * To set the Open property of a group. The set of the property is not
         * done to the storage/engine directly but to the instance of
         * CImpsCommonGroupProps class. This instance is not owned by this class.
         * And it will be queried by the caller of Edit Chat Group Properties
         * ie., CCAGroupPropertiesDialog to get the changed settings. The caller
         * will then Save Changes to the storage component.
         * @param TImpsPropertyBoolPC: EImpsPropYes if the property has to be set
         * else EImpsPropNo
         */
        virtual void SetOpen( TEnumsPC::TImpsPropertyBoolPC aType )  = 0;

        /*
        * To check if the specified group has been stored as a favorite group.
        * If yes, then the group will be stored as a persisent object by storage
        * manager. Else it wont be stored.
        * @return Etrue if the group is marked as favorite else return EFalse
        */
        virtual TBool IsFavouriteChatGroup( const TDesC& aGroupId ) const = 0;

        /*
        * To find if there is an already existing group with the same groupid
        * @return bool
        */
        virtual TBool FindGroup( const TDesC& aGroupId ) const = 0;

        /*
        * Name of the group with the given groupid
        * @return group name
        */
        virtual const TDesC& GetGroupName( const TDesC& aGroupId ) const = 0;

        /*
        * Returns EImpsPropYes if the private group messages are allowed else
        * returns EImpsPropNo.
        * If unknown type it will return EImpsUnknown
        * @return TImpsPropertyBool
        */
        virtual TEnumsPC::TImpsPropertyBoolPC IsPrivateAllowed() = 0;


        /*
         * Returns EImpsPropYes if the group is open else returns EImpsPropNo.
         * If unknown type it ll return EImpsUnknown
         * @return TImpsPropertyBool
         */
        virtual TEnumsPC::TImpsPropertyBoolPC IsOpen() = 0;

    };

#endif      // MCAGGROUPPROPERTIESDIALOGPC_H

// End of File
