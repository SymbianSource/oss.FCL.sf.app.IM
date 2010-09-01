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
* Description:  Implementation for PC Group properties dialog
*
*/


#ifndef CCAGGROUPPROPERTIESPC_H
#define CCAGGROUPPROPERTIESPC_H

// INCLUDES
#include	"mcagrouppropertiespc.h"
#include	"MCAChatObserver.h"
#include	"MCAGroupEventObserver.h"
#include	"MCAMessageErrorObserver.h"
#include	"MCAMessageRWInterfacePC.h"
#include    <impsgroupprops.h>


// FORWARD DECLARATIONS
class CCAEngine;
class MCAStoredGroups;
class MCAStoredGroup;
class MCAGroupManagerInterface;
class MCAGroupOperations;
class MCAStoredContacts;


//class declaration
/**
 *  @lib wvuiprocessng.lib
 *  @since 3.2
 */
class CCAGroupPropertiesPC:  public CBase,
            public MCAGroupPropertiesPC

    {

    public:

        /**
        * Symbian Two Phase Constructor
        * @param aEngine : a reference to chat engine
        */
        static CCAGroupPropertiesPC* NewL( MCAGroupManagerInterface& aGroupMgrInterface  );


        /**
        * Second phased symbian constructor.
        */
        void ConstructL();

        /**
        * virtual destructor
        */
        virtual ~CCAGroupPropertiesPC();

    public: //From MCAGroupPropertiesPC

        /**
         * @see MCAGroupPropertiesPC
         */
        TInt NumberOfCreatedGroupsDuringSession();


        /**
        * @see MCAGroupPropertiesPC
        */
        const TPtrC GetIdentificationL( const TDesC& aContactId );


        /**
        * @see MCAGroupPropertiesPC
        */
        TEnumsPC::TImpsPropertyBoolPC IsSearchable( ) ;

        /**
         * @see MCAGroupPropertiesPC
         */
        virtual TInt InitializeForEdittingL( const TDesC& aGroupId );

        /**
         * @see MCAGroupPropertiesPC
         */
        virtual TBool IsJoined( const TDesC& aGroupId ) const;

        /**
        * @see MCAGroupPropertiesPC
        */
        virtual TBool IsOwnGroup( const TDesC& aGroupId ) const ;

        /**
         * @see MCAGroupPropertiesPC
         */
        virtual TBool IsAdmin( const TDesC& aGroupId ) const ;

        /**
         * @see MCAGroupPropertiesPC
         */
        virtual TBool IsFavouriteChatGroup( const TDesC& aGroupId ) const;

        /**
        * @see MCAGroupPropertiesPC
        */
        virtual TBool FindGroup( const TDesC& aGroupId ) const;

        /**
        * @see MCAGroupPropertiesPC
        */
        virtual const TDesC& GetGroupName( const TDesC& aGroupId ) const;

        /**
        * @see MCAGroupPropertiesPC
        */
        virtual TEnumsPC::TImpsPropertyBoolPC IsPrivateAllowed();

        /**
         * @see MCAGroupPropertiesPC
         */
        virtual TEnumsPC::TImpsPropertyBoolPC IsOpen();

        /**
         * @see MCAGroupPropertiesPC
         */
        virtual void SetOpen( TEnumsPC::TImpsPropertyBoolPC aType );

    public:

        /**
         * To convert the impsengine side TImpsPropertyBool enum to PC side
         * TImpsPropertyBoolPC
         * @param aEventSource: enum type in impsengine TImpsPropertyBool
         * @return TEnumsPC::TImpsPropertyBoolPC
         */
        TEnumsPC::TImpsPropertyBoolPC ConvertTImpsPropertyBoolToTEnumsPC
        ( TImpsPropertyBool aEventSource );

        /*
         * This method will be called by the grouputilspc component after the
         * groupproperties dialog has exited. The member variable
         * iImpsCommonGropuProps of this class will contain the edited or newly
         * created group's properties. This method will return the same instance
         * of editted properties to the grouputilspc.
         * @return CImpsCommonGroupProps instance: holds the saved changes to
         * the properties of a group
         */
        CImpsCommonGroupProps* GetEdittedProperties();

    public:
        //common group properties to be used by edit group properties only
        CImpsCommonGroupProps* iGroupPropsForGrpPropDialog;

    public: //include inline functions file
#include "CCAGroupPropertiesPC.inl"

    private:

        /**
        * default constructor
        * @param aGroupMgrInterface : a reference to group manager interface in the engine component
        */
        CCAGroupPropertiesPC( MCAGroupManagerInterface& aGroupMgrInterface  );

    private:


        //Doesnt own,Handle to chat groups interface
        MCAStoredGroups* iChatGroups;

        //Doesn't own. Group interface
        MCAGroupManagerInterface& iGroupMgrInterface;

        //Doesnt own, Handle to Interface for stored contacts
        MCAStoredContacts* iStoredContacts;


        //private group props to be used by edit group properties only
        CImpsPrivateGroupProps* iPrivatePropsForGrpPropDialog;

    };

#endif      // CCAGGROUPPROPERTIESDIALOGPC_H

// End of File
