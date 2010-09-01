/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Attribute asynhronous data set hook.
*
*/

#ifndef CPENGATTRMODELASYNCDATASETHOOK_H
#define CPENGATTRMODELASYNCDATASETHOOK_H

//  INCLUDES
#include <E32Base.h>


//FORWARD DECLARATION
class CPEngPresenceAttrModel;
class MPEngPresenceAttrModelTypeImp;



// CLASS DECLARATION
/**
 * Attribute asynhronous data set hook.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttrModelAsyncDataSetHook ) : public CActive
    {
public: //Constructor and destructor

    /**
     * C++ default constructor.
     */
    CPEngAttrModelAsyncDataSetHook();


    /**
     * Destructor.
     * Cancels any outstanding request.
     */
    ~CPEngAttrModelAsyncDataSetHook();


public: // New functions

    /**
     * Initializess the hook.
     *
     * Sets the request status where to hook in
     * and also the attribute objects which to callback
     * when the request is completed or gets canceled.
     *
     * If hook is already active, leaves with KErrInUse.
     *
     * @since 3.0
     * @param aStatus The request status to hook in.
     * @param aAttributeModel The attribute model owning the hook.
     * @param aTypeImp The attribute model implementation
     * which data set to hook in.
     */
    void InitializeLC( TRequestStatus& aRequestStatus,
                       CPEngPresenceAttrModel& aAttributeModel,
                       MPEngPresenceAttrModelTypeImp& aTypeImp );



    /**
     * Gets the hooks own request status.
     *
     * This request status is given prior
     * starting the hook to concrete asynchronous
     * request.
     *
     * @since 3.0
     * @param The hook own request status.
     */
    TRequestStatus& Status();


    /**
     * Starts the data set hook.
     *
     * The hook must be properly initialized before a
     * call to start.
     *
     * Registered attribute object callbacks are called
     * when something happens in the hook.
     *
     * If the hook wasn't properly initialized before
     * starting, panics.
     *
     * @since 3.0
     */
    void Activate();


protected:  // Functions from CActive

    /**
     * Gets called when actual request completes.
     *
     * @since 3.0
     */
    void RunL();


    /**
     * Gets called when hook is cancelled.
     *
     * @since 3.0
     */
    void DoCancel();


protected:  // Helpers
    void Reset();
    static void ResetTheHook( TAny* aHook );


private:    // Data

    //REF: Attribute model (owner) to update after complete
    CPEngPresenceAttrModel*  iHookOwner;

    //REF: Attribute model implementation to cancel
    MPEngPresenceAttrModelTypeImp* iHookedImp;

    //REF: Request status to complete after hook operation
    TRequestStatus* iStatusToComplete;

    };

#endif      // CPENGATTRMODELASYNCDATASETHOOK_H

// End of File





