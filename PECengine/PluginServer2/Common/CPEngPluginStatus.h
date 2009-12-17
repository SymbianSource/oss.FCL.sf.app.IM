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
* Description:  See class description below.
*
*/


#ifndef __CPENGPLUGINSTATUS_H__
#define __CPENGPLUGINSTATUS_H__

//  INCLUDES
#include <e32def.h>
#include <s32strm.h>

//  CLASS DEFINITION
/**
*  This is container where state of plugins is stored.
*  This is used for example to inform client if the plugin has been loaded
*  or if the loading has failed.
*
*  @lib PEngPlgSrv.lib
*  @since 2.1
*/

NONSHARABLE_STRUCT( TPEngPluginResult )
    {
    TUid iUid;
    TPEngPluginStatusCode iStatus;

    TPEngPluginResult( TUid aUid, TPEngPluginStatusCode aStatus )
            : iUid( aUid ), iStatus( aStatus )
        {
        }
    };

NONSHARABLE_CLASS( CPEngPluginStatus )
        : public CBase, public MPEngPluginStatus
    {
public:		// Constructors and destructors

    static CPEngPluginStatus* NewL();	// Static constructor
    static CPEngPluginStatus* NewLC();	// Static constructor
    virtual ~CPEngPluginStatus();		// Destructor (virtual)

public:	// New methods

    /**
    * Count if results in this object.
    *
    *
    * @since 2.8
    * @return TUint is count of results
    */
    TUint Count() const;

    /**
    * UID3 of plugin.
    *
    * @since 2.8
    * @param aIndex is index to result buffer.
    * @return TUid is plugin uid.
    */
    TUid Uid( TUint aIndex ) const;

    /**
    * Status of plugin.
    *
    *
    * @since 2.8
    * @param aIndex is index of result.
    * @return TPEngPluginStatusCode is last status of the plugin.
    *			Codes listed in MPEngPluginStatus.h
    */
    TPEngPluginStatusCode Status( TUint aIndex ) const;

    /**
    * Find status of certain plugin
    *
    * @since 2.8
    * @param aPluginUid is UID3 of the plugin
    * @return TInt is KErrNotFound or index.
    */
    TInt Find( TUid aPluginUid ) const;

    /**
    * CleanupStack support.
    *
    * !!! Destroys this object !!!
    *
    *
    * @since 2.8
    */
    void Close();

    /**
    * Add result to result container.
    *
    *
    * @since 2.8
    * @param aUid is UID3 of the plugin
    * @param aStatus is status of the plugin
    */
    void AddResultL( TUid aUid, TPEngPluginStatusCode aStatus );

    /**
    * Frees memory allocated to this result container.
    *
    * @since 2.8
    */
    void Zero();

    /**
    * Remove result from container.
    *
    * @since 2.8
    * @param aIndex is index of result to be removed.
    *		Will panic if you give invalid index.
    */
    void Remove( TUint aIndex );

    /**
    * Allocate space for result container.
    *
    * Used only when transferring results from server to client side.
    *
    * @since 2.8
    * @param aCount is count of results this container should hold.
    */
    void AllocateSpaceForResultsL( TUint aCount );

    /**
    * Returns buffer when server can write the results.
    *
    * @since 2.8
    * @param aCount tells how many results will be wrapped in this buffer.
    * @return TPtr8 is buffer where server can safely read or write results.
    */
    TPtr8 Buffer( TUint aCount = 0 ) const;

protected:	// Constructors and destructors

    CPEngPluginStatus();// Default constructor, protected to allow derivation
    void ConstructL();		// Second phase construct

private:	// Methods not implemented

    CPEngPluginStatus( const CPEngPluginStatus& );		// Copy constructor
    CPEngPluginStatus& operator=( const CPEngPluginStatus& );	// Assigment operator

private:	// Data

    //Array of results
    RArray<TPEngPluginResult> iResults;
    };

#endif      //  __CPENGPLUGINSTATUS_H__
