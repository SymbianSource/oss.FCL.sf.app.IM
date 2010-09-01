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
* Description:  CCAContactItemPC, CCAContactListItemPC, CCAOwnDataItemPC,
*				 CCAConversationItemPC, CCAInviteItemPC
*                are the different types of array items stored in the MCAMainViewArrayPC
*
*/



#ifndef CCAARRAYITEMPC_H
#define CCAARRAYITEMPC_H
#include <e32base.h>

#include "MCAMainViewArrayPC.h"
#include "ChatDefinitions.h"

/**
* Base class for CCAContactItemPC, CCAContactListItemPC, CCAOwnDataItemPC
* to be stored into an array of <CCAArrayItemPC> on the process component side.
* The UI components will query for each item in this heterogeneous array to display
* information like contactlist, contact, conversation, groups etc in the listbox of
* the main view class
*/
class MCAArrayItemPC
    {

    public:

        /**
        * Returns the default type of the listbox item
        * @return TEnumsPC::TItem: type of the listbox item as either EContactitem, EContactListItem etc
        */
        virtual TEnumsPC::TItem Type() = 0;

        /**
        * Returns the name of the listbox item
        * @return TPtrC: name of the listbox item to be displayed
        */
        virtual TPtrC GetItemNameText() = 0 ;

        /**
        * Sets the name of the listbox item
        * @param aName: Name of the listbox item to be set
        * @return void
        */
        virtual void SetItemNameText( const TDesC& aName ) = 0;

        /**
        * Returns the array index of the listbox item on engine/storage side
        * @return TInt: index of the listbox item on engine/storage side
        */
        virtual TInt GetItemEngineIndex() = 0;

        /**
        * Sets the on engine/storage side index of the listbox item
        * @param aIndex: index of the listbox item on engine/storage side
        * @return void
        */
        virtual void SetItemEngineIndex( TInt aIndex ) = 0;

        virtual ~MCAArrayItemPC() {};

    };

/**
* CCAContactItemPC is a structure that will encapsualte information on a contact
* item in the single array on the process component side.
*/
class CCAContactItemPC : public MCAArrayItemPC, public CBase
    {

    public: // symbain constructor , destructor

        /*
        * Returns the ContactitemPC instance
        */
        static CCAContactItemPC* NewL();
        /*
        * symbain way of construction
        */
        void ConstructL();

        /*
        * destructor
        */
        virtual ~CCAContactItemPC();

    private:

        /*
        * C++ default constructor
        */
        CCAContactItemPC();

    public:

        /**
        * Returns the blocked status of the contact item
        * @return bool: true if the contact item is blocked else false
        */
        TBool IsBlocked();

        /**
        * Returns the pending messages status of the contact item
        * @return bool: true if the contact item has pending messages
        * else return false
        */
        TBool IsMsgPending();

        /**
        * Sets the blocked status of the contact item
        * @param aBlockedStatus: true if the contact is blocked else false
        */
        void SetBlockedStatus( TBool aBlockedStatus );

        /**
        * Sets the messages pending status of the contact item
        * @param aPendingMessageStatus: true if the contact has pending messages
        * else false
        */
        void SetMsgPending( TBool aPendingMessageStatus );

        /**
        * Returns the online status of the contact item
        * @return TEnumsPC::TOnlineStatus: online status of the contact item
        * to be displayed as either Online, Away,  Busy etc
        */
        TEnumsPC::TOnlineStatus GetOnlineStatus();

        /**
        * Sets the online status of the contact item
        * @parama OnlineStatus: online status of the contact item to
        * be set as Online, Away,  Busy etc
        */
        void SetOnlineStatus( TEnumsPC::TOnlineStatus aOnlineStatus );


    public: //From MCAArrayItemPC

        /**
        * Returns the name of the contact item
        * @return TPtrC: name of the contact item to be displayed
        */
        TPtrC GetItemNameText();

        /**
        * Sets the name of the contact item
        * @param aName: Name of the contact item to be set
        * @return void
        */
        void SetItemNameText( const TDesC& );

        /**
        * Returns the default type of the contact item
        * @return TEnumsPC::TItem: type of the contact item as either
        * EContactitem, EContactListItem etc
        */
        TEnumsPC::TItem Type();


        /**
        * Get the index of the contact item from engine side model
        * @return TInt: index
        */
        inline TInt GetItemEngineIndex()
            {
            return iEngineIndex;
            }

        /**
        * Set the index of the contact item from engine side model
        * @param TInt: aIndex
        */
        inline void SetItemEngineIndex( TInt aIndex )
            {
            iEngineIndex  = aIndex;
            }

    private:

        /*
        * Online status of contact
        */
        TEnumsPC::TOnlineStatus iOnlineStatus;

        /*
        * Blocked status of contact
        */
        TBool iBlockedStatus;

        /*
        * Pending messages status of contact
        */
        TBool iUnreadmsgFlag;

        /*
        * Index of the item in mainviewitemarray
        */
        int		  	iMainViewArrayIndex;

        /*
           * Index of the item in engines model class
           */
        TInt	  	iEngineIndex;

        /**
        * The name of the listbox item
        */
        HBufC* iItemNameTextContact;

    };

/**
* CCAOpenChatsListItemPC is a structure that will encapsualte information on a contact
* list in the single array on the process component side.
*/
class CCAOpenChatsListItemPC : public MCAArrayItemPC, public CBase
    {

    public:
        // symbain way of construction

        static CCAOpenChatsListItemPC* NewL();
        void ConstructL();
        virtual ~CCAOpenChatsListItemPC();

    private:

        /*
        * C++ default constructor
        */
        CCAOpenChatsListItemPC();
    public:

        /**
        * Returns the expanded or collapsed state information of the contact list
        * @return bool: true if the contact list is collapsed else return false
        */
        TBool IsCollapsed();

        /**
        * Sets the collapsed/expanded state of the contact list
        * @param aCollapsed: true if the contact list item is collapsed else false
        */
        void SetCollapsed( TBool aCollapsed );


    public: //From CCAArrayItemPC

        /**
        * Returns the name of the contact list item
        * @return TPtrC: name of the contact item to be displayed
        */
        TPtrC GetItemNameText();

        /**
        * Sets the name of the contact list item
        * @param aName: Name of the contact list item to be set
        * @return void
        */
        void SetItemNameText( const TDesC& );

        /**
        * Returns the default type of the contact list item
        * @return TEnumsPC::TItem: type of the contact list item as either
        * EContactitem, EContactListItem etc
        */
        TEnumsPC::TItem Type();

        /**
        * Get the index of the contact item from engine side model
        * @return TInt: index
        */
        inline TInt GetItemEngineIndex()
            {
            return iEngineIndex;
            }

        /**
        * Set the index of the contact item from engine side model
        * @param TInt: aIndex
        */
        inline void SetItemEngineIndex( TInt aIndex )
            {
            iEngineIndex  = aIndex;
            }

    private:

        /*
         * collapsed/expanded status of the contact list
        */
        TBool iCollapsedStatus;

        /*
         * Total no. of contacts in the contact list
        */
        TInt  iTotalContacts;

        /*
         * Total no. of contacts that are online in the contact list
        */
        TInt  iOnlineContacts;

        /*
         * name of the contact list followed by
         * the (No. of Online Contacts/ Total contacts) in that contact list
        */
        HBufC* iText;

        /*
           * Index of the item in mainviewitemarray
           */
        int	iMainViewArrayIndex;

        /*
           * Index of the item in engines model class
           */
        int	iEngineIndex;



        /**
        * The name of the listbox item
        */
        HBufC* iItemNameTextChat;

    };


/**
* CCAConversationItemPC is a structure that will encapsualte information on a contact
* item in the single array on the process component side.
*/
class CCAConversationItemPC : public MCAArrayItemPC, public CBase
    {

    public:
        // symbain way of construction

        static CCAConversationItemPC* NewL( const TDesC& aUserId );
        void ConstructL( const TDesC& aUserId );
        virtual ~CCAConversationItemPC();
    private:

        /*
        * C++ default constructor
        */
        CCAConversationItemPC(  );

    public:

        /**
        * Returns the pending messages status of the contact item
        * @return bool: true if the contact item has pending messages
        * else return false
        */
        TBool IsMsgPending();

        /**
        * Sets the messages pending status of the contact item
        * @param aPendingMessageStatus: true if the contact has pending messages
        * else false
        */
        void SetMsgPending( TBool aPendingMessageStatus );

    public: //From CCAArrayItemPC

        /**
        * Returns the name of the contact item
        * @return TPtrC: name of the contact item to be displayed
        */
        TPtrC GetItemNameText();

        /**
        * Sets the name of the contact item
        * @param aName: Name of the contact item to be set
        * @return void
        */
        void SetItemNameText( const TDesC& );


        /**
        * Returns the default type of the contact item
        * @return TEnumsPC::TItem: type of the contact item as either
        * EContactitem, EContactListItem etc
        */
        TEnumsPC::TItem Type();

        /**
        * Get the index of the contact item from engine side model
        * @return TInt: index
        */
        inline TInt GetItemEngineIndex()
            {
            return iEngineIndex;
            }

        /**
        * Set the index of the contact item from engine side model
        * @param TInt: aIndex
        */
        inline void SetItemEngineIndex( TInt aIndex )
            {
            iEngineIndex  = aIndex;
            }

    public:


        /**
        * Returns the UserId of the conversation item
        * @return TPtrC: UserId to be displayed
        */
        const TDesC& UserId() const;

        /**
        * Returns the identification of the conversation item
        * @return TPtrC: identification to be displayed
        */
        const TDesC& Identification() const;

    private:

        /*
        * Pending messages status of contact
        */
        TBool iUnreadmsgFlag;

        /*
         * Index of the item in mainviewitemarray
         */
        int	iMainViewArrayIndex;

        /*
           * Index of the item in engines model class
           */
        int	iEngineIndex;


        /**
        * The name of the listbox item
        */
        HBufC* iItemNameTextConv;

        /**
        * The iUserId of the contact
        */
        HBufC* iUserId;

    };

/**
 * CCAInviteItemPC is a structure that will encapsualte information
 */
class CCAInviteItemPC : public MCAArrayItemPC, public CBase
    {

    public:
        // symbain way of construction

        static CCAInviteItemPC* NewL( const TDesC& aUserId,
                                      const TDesC& aInviteId );
        void ConstructL();
        virtual ~CCAInviteItemPC();
    private:

        /*
        * C++ default constructor
        */
        CCAInviteItemPC( const TDesC& aUserId, const TDesC& aInviteId );

    public: //From CCAArrayItemPC

        /**
        * Returns the name of the contact item
        * @return TPtrC: name of the contact item to be displayed
        */
        TPtrC GetItemNameText();

        /**
        * Sets the name of the contact item
        * @param aName: Name of the contact item to be set
        * @return void
        */
        void SetItemNameText( const TDesC& aName ) ;

        /**
        * Returns the default type of the contact item
        * @return TEnumsPC::TItem: type of the contact item as either
        * EContactitem, EContactListItem etc
        */
        TEnumsPC::TItem Type();

        MCAInvitation* GetInvitation();

        void  SetInvitation( const MCAInvitation* aInvitation );

        /**
        * Get the index of the contact item from engine side model
        * @return TInt: index
        */
        inline TInt GetItemEngineIndex()
            {
            return iEngineIndex;
            }

        /**
        * Set the index of the contact item from engine side model
        * @param TInt: aIndex
        */
        inline void SetItemEngineIndex( TInt aIndex )
            {
            iEngineIndex  = aIndex;
            }

    public:


        /**
        * Returns the UserId of the invitation item
        * @return TPtrC: UserId to be displayed
        */
        const TDesC& UserId() const;

        /**
        * Returns the UserId of the invitation item
        * @return TPtrC: UserId to be displayed
        */
        const TDesC& GetInviteID() const;
        /**
        * Returns the identification of the invitation item
        * @return TPtrC: identification to be displayed
        */
        const TDesC& Identification() const;

    private:

        // Flag for telling if invitation is opened.

        /*
           * Index of the item in mainviewitemarray
           */
        int	iMainViewArrayIndex;

        /*
           * Index of the item in engines model class
           */
        int	iEngineIndex;

        //The buffer has been changed to a heap buffer for optimization

        /**
        * The name of the listbox item
        */
        HBufC* iItemNameTextInvite;

        /**
        * The iUserId of the contact
        */
        const TDesC& iUserId;
        /**
        * The InviteId of the invitation
        */
        const TDesC& iInviteID;

        const MCAInvitation* iInvitation;

    };


/**
 * CCAGroupItemPC will encapsualte information
 * about a group item
 */
class CCAGroupItemPC : public CBase, public MCAArrayItemPC
    {

    public:
        /**
        * Two-phased constructor.
        * @param aEngine - Reference to chat engine component
        */
        static CCAGroupItemPC* NewL( const TDesC& aGroupId );

        /*
        * Symbian second phased constructor
        */
        void ConstructL( const TDesC& aGroupId );

        /*
        * Destructor
        */
        virtual ~CCAGroupItemPC();

    private:

        /*
        * C++ default constructor
        */
        CCAGroupItemPC();

    public:

        /**
        * Returns the pending messages status of the contact item
        * @return bool: true if the contact item has pending messages
        * else return false
        */
        TBool IsMsgPending();

        /**
        * Sets the messages pending status of the contact item
        * @param aPendingMessageStatus: true if the contact has pending messages
        * else false
        */
        void SetMsgPending( TBool aPendingMessageStatus );

    public: //From CCAArrayItemPC

        /**
        * Returns the name of the group item
        * @return TPtrC: name of the contact item to be displayed
        */
        TPtrC GetItemNameText();

        /**
        * Sets the name of the group item
        * @param aName: Name of the contact item to be set
        * @return void
        */
        void SetItemNameText( const TDesC& aName ) ;

        /**
        * Returns the default type of the group item
        * @return TEnumsPC::TItem: type of the item as either
        * EContactitem, EContactListItem etc
        */
        TEnumsPC::TItem Type();

        /**
        * Get the index of the group item from engine side model
        * @return TInt: index
        */
        inline TInt GetItemEngineIndex()
            {
            return iEngineIndex;
            }

        /**
        * Set the index of the group item from engine side model
        * @param TInt: aIndex
        */
        inline void SetItemEngineIndex( TInt aIndex )
            {
            iEngineIndex  = aIndex;
            }

        /**
        * Returns the GroupId of the conversation item
        * @return TPtrC: GroupId to be displayed
        */
        const TDesC& GroupId() const;

        /**
        * Returns the identification of the invitation item
        * @return TPtrC: identification to be displayed
        */
        const TDesC& Identification() const;
    private:

        /*
        * Pending messages status of contact
        */
        TBool iUnreadmsgFlag;

        /*
         * Index of the item in mainviewitemarray
         */
        TInt  iMainViewArrayIndex;

        /*
           * Index of the item in engines model class
           */
        TInt  iEngineIndex;

        /**
        * The name of the group item
        */
        HBufC* iItemNameTextGroup;

        /**
        * Owns the iGroupID
        */
        HBufC* iGroupId;

    };

/**
* CCAContactListItemPC is a structure that will encapsualte information on a contact
* list in the single array on the process component side.
*/
class CCAContactListItemPC : public MCAArrayItemPC, public CBase
    {

    public:
        // symbain way of construction

        static CCAContactListItemPC* NewL();
        void ConstructL();
        virtual ~CCAContactListItemPC();

    private:

        /*
        * C++ default constructor
        */
        CCAContactListItemPC();

    public:

        /**
        * Returns the expanded or collapsed state information of the contact list
        * @return bool: true if the contact list is collapsed else return false
        */
        TBool IsCollapsed();

        /**
        * Returns the pending messages status for the contacts in the contact list
        * @return bool: true if the contacts int he contact list has pending messages
        * else return false
        */
        TBool IsMsgPending();

        /**
        * Sets the collapsed/expanded state of the contact list
        * @param aCollapsed: true if the contact list item is collapsed else false
        */
        void SetCollapsed( TBool aCollapsed );

        /**
        * Sets the message pending status of the contacts in the contact list
        * @param aMessagePending: true if the contacts in the contact list have
        * pending messages else false
        */
        void SetMsgPending( TBool aMessagePending );

    public: //From CCAArrayItemPC

        /**
        * Returns the name of the contact list item
        * @return TPtrC: name of the contact item to be displayed
        */
        TPtrC GetItemNameText();

        /**
        * Sets the name of the contact list item
        * @param aName: Name of the contact list item to be set
        * @return void
        */
        void SetItemNameText( const TDesC& );

        /**
        * Returns the default type of the contact list item
        * @return TEnumsPC::TItem: type of the contact list item as either
        * EContactitem, EContactListItem etc
        */
        TEnumsPC::TItem Type();

        /**
        * Get the index of the contact item from engine side model
        * @return TInt: index
        */
        inline TInt GetItemEngineIndex()
            {
            return iEngineIndex;
            }

        /**
        * Set the index of the contact item from engine side model
        * @param TInt: aIndex
        */
        inline void SetItemEngineIndex( TInt aIndex )
            {
            iEngineIndex  = aIndex;
            }

    private:

        /*
         * collapsed/expanded status of the contact list
        */
        TBool iCollapsedStatus;

        /*
         * Pending messages status for contacts in the list
        */
        TBool iUnreadmsgFlag;

        /*
         * Total no. of contacts in the contact list
        */
        TInt  iTotalContacts;

        /*
         * name of the contact list followed by
         * the (No. of Online Contacts/ Total contacts) in that contact list
        */
        HBufC* iText;

        /*
           * Index of the item in mainviewitemarray
           */
        int	iMainViewArrayIndex;

        /*
           * Index of the item in engines model class
           */
        int	iEngineIndex;


        /**
        * The name of the listbox item
        */
        HBufC* iItemNameTextList;
    };

/**
* CCAOwnDataItemPC is a structure that will encapsualte information on Own data
* item in the single array on the process component side.
*/
class CCAOwnDataItemPC : public MCAArrayItemPC, public CBase
    {

    public:
        // symbain way of construction

        static CCAOwnDataItemPC* NewL();
        void ConstructL();
        virtual ~CCAOwnDataItemPC();

    private:

        // C++ default constructor

        CCAOwnDataItemPC();

    public:

        /**
        * Returns the online status of the own data item
        * @return TEnumsPC::TOnlineStatus: online status of own data item
        */
        TEnumsPC::TOnlineStatus GetOwnStatus();

        /**
        * Set the online status of the own data item
        * @param aOnlineStatus: online status of own data item
        */
        void SetOwnStatus( TEnumsPC::TOnlineStatus aOnlineStatus );

    public: //From CCAArrayItemPC

        /**
        * Returns the name of the contact list item
        * @return TPtrC: name of the contact item to be displayed
        */
        TPtrC GetItemNameText();

        /**
        * Sets the name of the contact list item
        * @param aName: Name of the contact list item to be set
        * @return void
        */
        void SetItemNameText( const TDesC& );

        /**
        * Returns the default type of the contact list item
        * @return TEnumsPC::TItem: type of the contact list item as either
        * EContactitem, EContactListItem etc
        */
        TEnumsPC::TItem Type();

        /**
        * Get the index of the contact item from engine side model
        * @return TInt: index
        */
        inline TInt GetItemEngineIndex()
            {
            return iEngineIndex;
            }

        /**
        * Set the index of the contact item from engine side model
        * @param TInt: aIndex
        */
        inline void SetItemEngineIndex( TInt aIndex )
            {
            iEngineIndex  = aIndex;
            }

    private:
        /*
        * online status of own data item
        */
        TEnumsPC::TOnlineStatus iOwnStatus;


        /*
           * Index of the item in mainviewitemarray
           */
        int	iMainViewArrayIndex;

        /*
           * Index of the item in engines model class
           */
        int	iEngineIndex;


        /**
        * The name of the listbox item
        */
        HBufC* iItemNameTextOwnData;


    };

#endif  //CCAArrayItemPC_H