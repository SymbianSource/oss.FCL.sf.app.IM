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
* Description:  Interface to provide recorded chat information.
*
*/


#ifndef MCARECORDEDCHATPROVIDER_H
#define MCARECORDEDCHATPROVIDER_H

//Forward declarations
class MCAMessagesReadInterface;
class CEikMenuPane;

// CLASS DECLARATION

/**
*  Interface to provide recorded chat information.
*
*  @lib chat.app
*  @since 2.5
*/
class MCARecordedChatProvider
    {
    public:
        /**
         *	Get interface to handle recorded chat messages.
         */
        //virtual MCAMessagesReadInterface& RecordedChatMessageInterfaceL() const = 0;

        /**
         *	Get name of recorded chat.
         */
        virtual const TDesC& RecordedChatName() const = 0;

        /**
         *	Get time when chat recording ended.
         */
        virtual TTime RecordedEndDate() const = 0;

        /**
         * Delete current chat.
         * @return ETrue if deletion is ok, EFalse if not.
         */
        virtual TBool DeleteChatL() = 0;

        /**
         *	Send current chat via MTM
         *  @param aTargetUid Id for MTM to send with.
         */
        virtual void SendChatViaMTML( TUid aTargetUid = KNullUid ) = 0;

        /**
         * Dynamic initialization of send MTM submenu.
         * @since 2.6
         * @param aMenuPane Target menu for initialization of send MTM submenu.
         */
        virtual void DynInitSendMenuL( CEikMenuPane& aMenuPane ) = 0;

        /**
         * Deletes message container of current recorded chat
         * from engine. Used to free memory when exiting current
         * recorded chat view i.e. called from DoDeactivate method
         * of view class.
         * @since 3.1
         */
        virtual void DeleteRecordedChatContainerL() = 0;

    protected:

        /**
         *	Destructor
         */
        virtual ~MCARecordedChatProvider() {};
    };

#endif //MCARECORDEDCHATPROVIDER_H
